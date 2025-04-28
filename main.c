#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "rand_malloc.h"

struct arena {
    void* mem_block;
    size_t mem_size;
    void* mem_ptr;
};

void arena_init(struct arena* arena, size_t size);
void* arena_alloc(struct arena* arena, size_t size);
void* arena_realloc(struct arena* arena, void* ptr, size_t size);
void arena_free(struct arena* arena);

void arena_init(struct arena* arena, size_t size) {
    arena->mem_block = malloc(size);
    if (!arena->mem_block) {
        fprintf(stderr, "Arena initialization failed\n");
        exit(1);
    }
    arena->mem_size = size;
    arena->mem_ptr  = arena->mem_block;
}

void* arena_alloc(struct arena* arena, size_t size) {
    /* align to pointer size */
    size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    if ((char*)arena->mem_ptr + size > (char*)arena->mem_block + arena->mem_size) {
        arena_free(arena);
        fprintf(stderr, "Arena out of memory\n");
        exit(1);
    }
    void* ptr = arena->mem_ptr;
    arena->mem_ptr = (char*)arena->mem_ptr + size;
    return ptr;
}

void* arena_realloc(struct arena* arena, void* ptr, size_t size) {
    void* new_mem = arena_alloc(arena, size);
    memcpy(new_mem, ptr, size);
    return new_mem;
}

void arena_free(struct arena* arena) {
    if (arena->mem_block) {
        free(arena->mem_block);
        arena->mem_block = NULL;
        arena->mem_ptr   = NULL;
        arena->mem_size  = 0;
    }
}

static struct arena GLOBAL_ARENA = { 0 };

static char* arena_strdup(const char* s) {
    size_t length = strlen(s) + 1;
    char* p = arena_alloc(&GLOBAL_ARENA, length);
    memcpy(p, s, length);
    return p;
}

void reverse_string(char *string)
{
    size_t length = strlen(string);
    size_t middle = length / 2;
    char temp;

    for (size_t i = 0; i < middle; i++)
    {
        temp = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = temp;
    }
}

char* resize_buffer(char* buffer, size_t new_capacity)
{
    return arena_realloc(&GLOBAL_ARENA, buffer, new_capacity);
}

void fill_with_zeros(char **s, size_t zeros_to_add)
{
    size_t orginal_length = strlen(*s);
    size_t new_length = orginal_length + zeros_to_add;
    char *buffer = arena_alloc(&GLOBAL_ARENA, new_length + 1);

    for (size_t i = 0; i < zeros_to_add; i++) {
        buffer[i] = '0';
    }
    size_t j = 0;
    for (size_t i = zeros_to_add; i < new_length; i++) {
        buffer[i] = (*s)[j++];
    }
    buffer[new_length] = '\0';

    *s = buffer;
}

void equalize_lengths(char **s1, char **s2)
{
    size_t len1 = strlen(*s1);
    size_t len2 = strlen(*s2);

    if (len1 < len2) {
        fill_with_zeros(s1, len2 - len1);
    }
    else if (len2 < len1) {
        fill_with_zeros(s2, len1 - len2);
    }
}

char* binary_addition(char *s1, char *s2) {
    equalize_lengths(&s1, &s2);

    int last_index_s1 = strlen(s1) - 1;
    int last_index_s2 = strlen(s2) - 1;

    int current_sum, carry = 0;
    int length = strlen(s1);
    char *res = arena_alloc(&GLOBAL_ARENA, length + 2);

    int result_index = 0;

    while (last_index_s1 >= 0 || last_index_s2 >= 0) {
        current_sum = carry;
        if (last_index_s1 >= 0) {
            current_sum += s1[last_index_s1] - '0';
            last_index_s1--;
        }
        if (last_index_s2 >= 0) {
            current_sum += s2[last_index_s2] - '0';
            last_index_s2--;
        }

        res[result_index++] = (current_sum % 2) + '0';
        carry = current_sum / 2;
    }

    if (carry != 0) {
        res[result_index++] = '1';
    }
    res[result_index] = '\0';

    reverse_string(res);
    return res;
}

char* get_line(void)
{
    int character;
    size_t capacity = 16;
    size_t current_length = 0;
    char* buffer = arena_alloc(&GLOBAL_ARENA, capacity);

    while ((character = getchar()) != EOF) {
        buffer[current_length++] = (char)character;

        if (current_length + 1 >= capacity) {
            capacity *= 2;
            buffer = resize_buffer(buffer, capacity);
        }

        if (character == '\n') {
            break;
        }
    }

    if (current_length == 0 && character == EOF) {
        return NULL;
    }

    if (current_length + 1 >= capacity) {
        buffer = resize_buffer(buffer, current_length + 1);
    }
    buffer[current_length] = '\0';

    return buffer;
}

char* filter_binary(const char* line)
{
    size_t len = strlen(line);
    char* binary_string = arena_alloc(&GLOBAL_ARENA, len + 1);
    size_t index = 0;

    for (size_t i = 0; i < len; i++) {
        if (line[i] == '0' || line[i] == '1') {
            binary_string[index++] = line[i];
        }
        else if(!isspace((unsigned char)line[i])){
            arena_free(&GLOBAL_ARENA);
            fprintf(stderr, "ONLY 0 AND 1 IN INPUT STREAM ARE ALLOWED \n");
            exit(1);
        }
    }
    binary_string[index] = '\0';
    return binary_string;
}

void add_binary_string_to_array(char*** inputs, size_t* capacity, size_t* count, char* binary_string) {
    if (*count >= *capacity) {
        *capacity *= 2;
        *inputs = arena_realloc(&GLOBAL_ARENA, *inputs, *capacity * sizeof(char*));
    }
    (*inputs)[*count] = binary_string;
    (*count)++;
}

void update_binary_sum_with_new_string(char** previous_line, char** binary_sum, char* binary_string) {
    if (*previous_line == NULL) {
        *previous_line = arena_strdup(binary_string);
        *binary_sum = *previous_line;
    } else {
        char *a = *previous_line;
        char *b = arena_strdup(binary_string);

        equalize_lengths(&a, &b);
        char *sum = binary_addition(a, b);
        *previous_line = sum;
        *binary_sum   = sum;
    }
}

void print_inputs(char** inputs, size_t count) {
    printf("Input numbers:\n");
    for (size_t i = 0; i < count; i++) {
        printf("%s\n", inputs[i]);
    }
}

void print_binary_sum(char* binary_sum, char* previous_line, size_t count) {
    printf("Sum:\n");
    if (binary_sum != NULL) {
        printf("%s\n", binary_sum);
    } else if (count == 1) {
        printf("%s\n", previous_line);
    }
}

int main()
{
    arena_init(&GLOBAL_ARENA, 10000000);

    size_t count = 0;
    char* line;
    char* previous_line = NULL;
    char* binary_sum = NULL;

    size_t capacity_of_strings = 2;
    char** inputs = arena_alloc(&GLOBAL_ARENA, capacity_of_strings * sizeof(char*));

    while ((line = get_line()) != NULL) {
        char* binary_string = filter_binary(line);
        if (strlen(binary_string) == 0) {
            continue;
        }
        add_binary_string_to_array(&inputs, &capacity_of_strings, &count, binary_string);
        update_binary_sum_with_new_string(&previous_line, &binary_sum, binary_string);
    }

    print_binary_sum(binary_sum, previous_line, count);
    print_inputs(inputs, count);

    arena_free(&GLOBAL_ARENA);
    return 0;
}
