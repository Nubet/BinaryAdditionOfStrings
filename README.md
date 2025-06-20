# Binary Addition Of Strings

C program that reads arbitrarily long binary numbers from standard input, filters invalid characters, and computes their sum using custom arena-based memory allocation and fault-injecting malloc wrappers.

## Features

* **Dynamic Line Reading**: `get_line()` reads input character-by-character, expanding buffers on demand—no fixed-size limits.
* **Binary Filtering & Validation**: strips whitespace, rejects any non-‘0’/‘1’ characters, and exits with an error message on invalid input.
* **Big-Integer Addition**: aligns and sums binary strings of different lengths, handling carries correctly and producing exact results for inputs of any size.
* **Custom Arena Allocator**: fast bump-pointer allocator (`arena_init`, `arena_alloc`, `arena_realloc`, `arena_free`) that manages memory in power-of-two chunks.
* **Fault Injection for Testing**: `rand_malloc.c` wraps `malloc`, `calloc`, and `realloc` with a configurable failure rate (`ALLOCATION_ERROR_RATE`) to simulate allocation errors.

## Repository Structure

```plaintext
BinaryAdder/
├── main.c            # Program entry, I/O loop, binary addition logic
├── rand_malloc.c     # Wrappers around standard allocators for error injection
├── rand_malloc.h     # Macro overrides for malloc/calloc/realloc
├── Makefile          # Building and linking
└── README.md         # Project documentation
```

## Build & Run

Requires: GCC (C11), Make

```bash
git clone https://github.com/Nubet/BinaryAdditionOfStrings.git
cd BinaryAdder
make  

# Provide binary numbers separated by newlines in file or enter them manually line-by-line when compiling via IDE
./main < input.txt
```

* **make** compiles `main.c` and `rand_malloc.c` into the target defined in `Makefile`.
* **make clean** - removes binary files

## Implementation Details

### `get_line()`

* Reads characters until `\n` or EOF using `getchar()`, expanding buffer via `arena_realloc()` when full.
* Returns dynamically allocated null-terminated string or `NULL` at EOF.

### Binary Processing

1. **Filter**: `filter_binary()` retains only ‘0’ and ‘1’; on other non-whitespace, frees arena and exits.
2. **Collect**: inputs stored in a dynamically growing array of `char*`, allocated from global arena.
3. **Add**: `binary_addition()` pads shorter operand with zeros, sums bitwise with carry, builds result in arena, then reverses.

### Arena Allocator

* **Chunks**: each `arena_chunk` holds a power-of-two data block; new chunks allocated as needed.
* **Headers**: each allocation stores its size before the returned pointer for `arena_realloc()`.
* **Free**: `arena_free()` walks chunks to `free()` all at program end.

### Fault Injection & Safety Testing

* **Purpose**: `rand_malloc.c` injects controlled allocation failures to evaluate the program’s resilience under low-memory conditions and verify that the arena allocator prevents leaks and double-free errors.
* **Failure Rate**: macro `ALLOCATION_ERROR_RATE` sets percentage chance for any `malloc`, `calloc`, or `realloc` call to return `NULL`.
* **Wrappers**: via `#define malloc rand_malloc` (and similarly for `calloc`, `realloc`), all standard allocations are funneled through `rand_malloc()` functions.
* **Memory Safety**: by simulating real-world allocation failures, this mechanism demonstrates that the custom arena allocator (`arena_alloc`, `arena_realloc`) manages lifetimes correctly - no partial allocations escape the arena, and all memory is reclaimed at program end.

## Testing & Error Handling## Testing & Error Handling

* Exits with code 1 and error message if invalid characters are encountered.
* All dynamic allocations check for `NULL` and free memory before exiting.

---
