
# Mem_alloc
This is a very basic implementation of a memory allocator in C made for learning purposes so there are probably better ways to do this like mmap

# What's dynamic allocation?
Dynamic allocation allows more flexibilty when we're working with memory, it involves allocating memory from a region called the heap wich is larger and can save more data

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

## Explanation
- **Stack**: Grows downwards from higher addresses to lower ones, typically used for local functions calls, local variables etc.
- **Heap**: Grows upwards from lower adresses to higher ones, contain the dynamic allocated data `mem_alloc()`.
- **Data**: This is where all initialized global and static variables are stored.
- **Text**: The part that contains the binary instructions to be executed by the processor.

## Resources
- https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory
- http://wiki.osdev.org/Memory_Allocation
