
# Mem_alloc
This is a very basic implementation of a memory allocator in C made for learning purposes so there are probably better ways to do this like mmap

# What's dynamic allocation?
Dynamic allocation allows more flexibilty when we're working with memory, it involves allocating memory from a region called the heap wich is larger and can save more data

```plaintext
+-------------------+  High Memory
|       Stack       |  (grows downwards)
|                   |
|                   |
|                   |
|                   |
+-------------------+
|       Heap        |  (grows upwards)
+-------------------+
|       Data        |  (initialized variables)
+-------------------+
|       Text        |  (instructions)
+-------------------+
```

## Explanation
- **Stack**: Grows downwards from higher addresses to lower ones, typically used for local functions calls, local variables etc.
- **Heap**: Grows upwards from lower adresses to higher ones, contain the dynamic allocated data `mem_alloc()`.
- **Data**: This is where all initialized global and static variables are stored.
- **Text**: The part that contains the binary instructions to be executed by the processor.

## How it works?
The memory blocks are linked each other by a pointer that refers to the memory address of the **header** of the next memory block, doing this we can keep track of all the memory blocks and know which ones are free and which ones not.
When a user calls the `mem_alloc()` function to allocate data the program will increment the program break to the size requested and return a pointer to the memory address where the heap starts.
In case of freeing memory `free()` it'll only remove the block if it's at the end of the heap, otherwise it will mark it as **free**.

```plaintext
Heap Start
|
V
+------------------------------------------------------------------------------------------+
|                                   Memory Block 1                                        |
| +----------------+     +--------------------------------------------------------------+ |
| |     Header     | --> | next (points to Block 2)                                   | |
| +----------------+     +--------------------------------------------------------------+ |
| |     Data       |                                                              |      |
| |  (requested    |                                                              |      |
| |   memory)      |                                                              |      |
| +----------------+                                                              |      |
+------------------------------------------------------------------------------------------+
|                                   Memory Block 2                                        |
| +----------------+     +--------------------------------------------------------------+ |
| |     Header     | --> | next (points to Block 3)                                   | |
| +----------------+     +--------------------------------------------------------------+ |
| |     Data       |                                                              |      |
| |  (requested    |                                                              |      |
| |   memory)      |                                                              |      |
| +----------------+                                                              |      |
+------------------------------------------------------------------------------------------+
|                                   Memory Block 3                                        |
| +----------------+     +--------------------------------------------------------------+ |
| |     Header     | --> | next = NULL (end of heap)                                 | |
| +----------------+     +--------------------------------------------------------------+ |
| |     Data       |                                                              |      |
| |  (requested    |                                                              |      |
| |   memory)      |                                                              |      |
| +----------------+                                                              |      |
+------------------------------------------------------------------------------------------+
|                                 End of Heap (sbrk(0))                                  |
+------------------------------------------------------------------------------------------+
```

## Resources
- https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory
- http://wiki.osdev.org/Memory_Allocation
