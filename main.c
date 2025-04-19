#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "rand_malloc.h"

bool proper_memory_allocation = true;

void reverse_string(char *string)
{
  size_t length = strlen(string);
  size_t middle = length / 2;
  char temp;

  // Swaps characters at the beginning of the string with their corresponding 
  // character at the end of the string, e.g. the first character with the 
  // last character, the second character with the second last character, and 
  // on and on until the middle of the string is reached, at which point the 
  // string will be reversed
  for (size_t i = 0; i < middle; i++)
  {
    temp = string[i];
    string[i] = string[length - i - 1];
    string[length - i - 1] = temp;
  }
}
char* resize_buffer(char* buffer, size_t new_capacity)
{
	char* temp = realloc(buffer, new_capacity * sizeof(char));
	if (NULL == temp) {
		proper_memory_allocation = false;
		free(buffer);
		fprintf(stderr, "Memory reallocation error\n");
		exit(1);
	}
	return temp;
}

void fill_with_zeros(char **s, size_t zeros_to_add)
{
	size_t orginal_length = strlen(*s);
	size_t new_length = orginal_length + zeros_to_add;
	
	char *buffer = malloc(new_length+1);
	if (NULL == buffer) {
		fprintf(stderr, "Memory allocation error\n");
		exit(1);
	}
	
	for(size_t i=0; i < zeros_to_add; i++){
		buffer[i] = '0';
	}
	
	size_t j=0;
	for(size_t i=zeros_to_add; i < new_length; i++){
			buffer[i] =  (*s)[j++];
	}
	buffer[new_length] = '\0';
	
	free(*s);
	*s = buffer;
	//printf("eqalized string: %s", buffer);
	
}
void equalize_lengths(char **s1, char **s2){
	size_t len1 = strlen(*s1);
	size_t len2 = strlen(*s2);
	
	if(len1 < len2){
		fill_with_zeros(s1, len2-len1);
		//printf("eqalized string: %s \n", *s1);
	}
	else if (len2 < len1){
		fill_with_zeros(s2, len1-len2);
		//printf("eqalized string: %s \n", *s2);
	}
}


char* binary_addition(char *s1, char *s2) {
    equalize_lengths(&s1, &s2);

    int last_index_s1 = strlen(s1) - 1;
    int last_index_s2 = strlen(s2) - 1;

    int current_sum, carry = 0;
    int length = strlen(s1);
    //+2  (1 for '\0' and 1 for carry)
    char *res = malloc(length + 2);
		if (NULL == res) {
		fprintf(stderr, "Memory allocation error\n");
		exit(1);
	}
	

    int result_index = 0;

    //while (last_index_s1 >= 0) should be fine, since we equalize the length of both strings
    while (last_index_s1 >= 0 || last_index_s2  >= 0) {
        current_sum = carry; // carry from previous addition
        if (last_index_s1 >= 0) {
            current_sum += s1[last_index_s1] - '0';
            last_index_s1--;
        }
        if (last_index_s2  >= 0) {
            current_sum += s2[last_index_s2] - '0';
            last_index_s2--;
        }
        
        
        //e.g. 1+1 = 2 (DECIMAL) = 10 (BINARY)
        // OPERATION 2 % 2 = 0 
        //so what ve recieved is last digit (BIT) of 10
        //and thats what we are instrested in.
        
        //The first number of 10 so '1' is our carry
        //We obtain it by dividing sum by 2
        // 2 / 2 = 1 
        
		//ALL POSIBLE SUMS
        // 0+1 = 1 (decimal) = 01 (binary) -> 1/2 = 0 (no carry)
        // 1+1 = 2 (decimal) = 10 (binary) -> 2/2 = 1 (carry)
        // 1+1+1 = 3 (decimal) = 11 (binary) -> 3 / 2 = 1 (carry)
        
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
	char* buffer = malloc(capacity * sizeof(char));
	if (NULL == buffer) {
		fprintf(stderr, "Memory allocation error\n");
		exit(1);
	}
	//getchar() reads one character from stdin
	while ((character = getchar()) != EOF) {
		buffer[current_length++] = (char)character;

		//  RESIZING BUFFER
		if (current_length >= capacity) {
			capacity *= 2;
			buffer = resize_buffer(buffer,capacity);
		}

		if (character == '\n') {
			break;
		}
	}

	// EXECUTED WHEN WHILE LOOP BODY NEVER HAVE BEEN EXECUTED ( getchar()) == EOF )
	if (current_length == 0 && character == EOF) {
		free(buffer);
		return NULL;
	}

	// WE MAKE SURE THAT WE HAVE ENOUGH SPACE FOR NULL TERMINATOR
	if (current_length >= capacity) {
		capacity += 1;
		buffer = resize_buffer(buffer,capacity);
	}
	buffer[current_length] = '\0';

	return buffer;
}

char* filter_binary(const char* line)
{
	size_t len = strlen(line);
	
	//+1 becasue we need to have space for null terminator
	char* binary_string = malloc(len + 1);
	if (NULL == binary_string) {
		fprintf(stderr, "Memory allocation error\n");
		exit(1);
	}

	size_t index = 0;
	for (size_t i = 0; i < len; i++) {
		if (line[i] == '0' || line[i] == '1') {
			binary_string[index++] = line[i];
		}
		else if(!isspace(line[i])){
			fprintf(stderr, "ONLY 0 AND 1 IN INPUT STREAM ARE ALL0WED \n");
			exit(1);
		}
	}
	binary_string[index] = '\0';
	return binary_string;
}

	void add_binary_string_to_array(char*** inputs, size_t* capacity, size_t* count, char* binary_string) {
    if (*count >= *capacity) {
        *capacity *= 2;
        *inputs = realloc(*inputs, *capacity * sizeof(char*));
        if (*inputs == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(1);
        }
    }
    (*inputs)[*count] = binary_string;
    (*count)++;
}

void update_binary_sum_with_new_string(char** previous_line, char** binary_sum, char* binary_string) {
    // If this is the first string, initialize the sum
    if (*previous_line == NULL) {
        *previous_line = strdup(binary_string);
        if (*binary_sum != NULL) {
            free(*binary_sum); // Free the previous sum if it exists
        }
        *binary_sum = *previous_line; // Update the binary sum to the new string
    } else {
        // Make a copy of binary_string to avoid modifying the original user input buffer
        char *a = *previous_line;
        char *b = strdup(binary_string);

        equalize_lengths(&a, &b);
        char *sum = binary_addition(a, b);
        free(a);
        free(b);

        // store the new sum
        *previous_line = sum;
        *binary_sum   = sum;
    }
}


void print_inputs(char** inputs, size_t count) {
    printf("INPUTS:\n");
    for (size_t i = 0; i < count; i++) {
        printf("%s\n", inputs[i]);
        free(inputs[i]);
    }
    free(inputs);
}

void print_binary_sum(char* binary_sum, char* previous_line, size_t count) {
	printf("Sum:\n");
    if (binary_sum != NULL) {
        printf("%s\n", binary_sum);
        free(binary_sum);
    } else if (count == 1) {
        printf("%s\n", previous_line);
        free(previous_line);
    }
}
    
int main()
{
	size_t count = 0;
	char* line;
	char* previous_line = NULL;
	char* binary_sum = NULL;
	
	size_t capacity_of_strings = 2;
	// We want to create an array that can hold 3 string
	// Each string is represented by a pointer to char (char*).
	// So, we need an array of pointers to char
    char** inputs = (char**)malloc(capacity_of_strings * sizeof(char*));
    if (inputs == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
	
	while ((line = get_line()) != NULL) {
		char* binary_string = filter_binary(line);
		free(line);

		// filtered line is empty
		if (strlen(binary_string) == 0) {
			free(binary_string);
			continue;
		}
		add_binary_string_to_array(&inputs, &capacity_of_strings, &count, binary_string);
        update_binary_sum_with_new_string(&previous_line, &binary_sum, binary_string);
   
	}
	
	print_binary_sum(binary_sum, previous_line, count);
	print_inputs(inputs, count);
	

	return 0;
}
