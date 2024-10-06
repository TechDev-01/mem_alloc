#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Define the data structure for our block of memory
/*A linked list to allocate the size, if its free or not and a pointer
 to the next header to keep track of our blocks of memory

+--------------------+        +--------------------+           +--------------------+
| Block 1:           |        | Block 2:           |           | Block 3:           |
| +----------------+ |        | +----------------+ |           | +----------------+ |
| |   Header       | |------> | |   Header       | |-------->  |  | Header       | |
| +----------------+ |        | +----------------+ |           | +----------------+ |
| |   Data         | |        | |   Data         | |           | |   Data         | |
| |                | |        | |                | |           | |                | |
+--------------------+        +--------------------+           +--------------------+
*/

//Align the block of memory to 16 bytes
typedef char ALIGN[16]; 

/*Here we wrapped the header in a union with a stub variable of 16 bytes 
 * this makes the header end up on a memory address of 16 bytes
 * the end of the header is where the actual memory block is begins and the provided by the caller will be aligned to 16 bytes*/
union header{
  struct{
    size_t size;
    unsigned is_free;
    union header *next;
  }s;
  ALIGN stub; 
};

typedef union header header_t; //Define the header_t as a union header

header_t *head, *tail; //Define the head and tail of the linked list to keep track of the blocks of memory
pthread_mutex_t global_malloc_lock; //Define a mutex lock to prevent two or more threads from accessing the memory at the same time

void *mem_alloc(size_t size); //Function to allocate memory
void free(void *block); //Function to free memory
header_t *get_free_block(size_t size); //Function to get a free block of memory

int main(){
  //Allocate memory
  void *arr = mem_alloc(40);
  void *arr1 = mem_alloc(10);
  void *arr2 = mem_alloc(20);

  //Free memory
  free(arr);  
  free(arr1);
  free(arr2);
  void *arr3 = mem_alloc(30);
  void *arr4 = mem_alloc(10);
  return 0;
}

header_t *get_free_block(size_t size){
  header_t *current = head;
  while(current){
 printf("[DEBUG] Revisando bloque. Dirección: %p, Tamaño: %zu, Libre: %u\n", (void *)(current + 1), current->s.size, current->s.is_free);
    //Search for a free block of memory that is big enough to hold the size of the block of memory that we want to allocate
    if(current->s.is_free && current->s.size >= size){
      return current;
    };
    current = current->s.next;
  };
  return NULL;
};

void *mem_alloc(size_t size){
  size_t total_size;
  void *block;
  header_t *header;
  
  if(!size)
    return NULL;
  pthread_mutex_lock(&global_malloc_lock);
  
  printf("[DEBUG] Buscando bloque libre de tamaño: %zu\n", size);
  header = get_free_block(size);

  //If we have a free block of memory we mark it as not free and return the pointer to the block of memory
  //unlock the mutex lock and return the pointer that will refer to the header part of the block of memory
  //that we found
  if(header){
    printf("[DEBUG] Bloque libre encontrado en dirección: %p\n", (void *)(header + 1));
    header->s.is_free = 0;
    pthread_mutex_unlock(&global_malloc_lock);
    printf("[ALLOC] Reutilizando bloque libre. Tamaño: %zu, Dirección: %p\n", size, (void *)(header + 1));
    return (void*)(header + 1);
  };

  printf("[DEBUG] No se encontró bloque libre. Extendiendo heap. Creando nuevo bloque de tamaño: %zu\n", size);
  //If we don't have a free block of memory we'll extend the heap by the size of the header and the size of the block of memory
  total_size = sizeof(header_t) + size;
  block = sbrk(total_size);
  if(block == (void*)-1){
    pthread_mutex_unlock(&global_malloc_lock);
    return NULL;
  };
  //We update the size, is_free and next pointer of the header
  header = block;
  header->s.size = size;
  header->s.is_free = 0;
  header->s.next = NULL;
  
  if(!head){
    head = header;
  }
  if(tail)
    tail->s.next = header;
  
  //We update the tail pointer to the new block of memory
  tail = header;
  
  //Print the size and address of the new block of memory
  printf("[ALLOC] Nuevo bloque asignado. Tamaño: %zu, Dirección: %p\n", size, (void *)(header + 1));

  //We unlock the mutex lock and return the pointer that will refer to the header part of the block of memory
  pthread_mutex_unlock(&global_malloc_lock);
  return (void*)(header + 1);

  /*Initial State: (before adding a new block)
        +------------------+        +------------------+
        | head (NULL)      |        | tail (NULL)      |
        +------------------+        +------------------+

        After `head = header`: (if head is NULL)
        +------------------+        +------------------+
        | head             |------> | Header Block 1   | 
        |                  |        | +--------------+ |
        |                  |        | |  Header      | |
        |                  |        | +--------------+ |
        |                  |        | |  Data        | |
        +------------------+        | |              | |
                                    +------------------+

        After `tail->s.next = header` and `tail = header`: (if tail exists)
        +------------------------+        +------------------------+
        | tail                   |        | New Header Block 2      |
        | +--------------------+ |        | +--------------------+  |
        | | tail->s.next =     | |------> | |    Header          |  |
        | | New Header Block   | |        | +--------------------+  |
        | | (Header Block 2)   | |        | |    Data            |  |
        | +--------------------+ |        | |                    |  |
        |                        |        +------------------------+
        | Updated tail = Block 2 |
        +------------------------+
 */
}

//Function to get a free block of memory
void free(void *block){
  header_t *header, *tmp;
  void *program_break;
  
  //Get the header of the block of memory to be freed
  if(!block){
    return;
  }
  pthread_mutex_lock(&global_malloc_lock);
  header = (header_t*)block - 1;

  program_break = sbrk(0);
  if((char*)block + header->s.size == program_break){
    printf("[DEBUG FREE] Liberando último bloque de memoria. Dirección: %p\n", (void *)(header + 1));
    //If the block of memory to be freed is the last block of memory we'll remove it from the linked list and reduce the size of the heap
    if(head == tail){
      head = tail = NULL;
    }else{
      tmp = head;
      while(tmp){
          if(tmp->s.next == tail){
          tmp->s.next = NULL;
          tail = tmp;
        }
        tmp = tmp->s.next;
      }
    }
    sbrk(0 - sizeof(header_t) - header->s.size);
    pthread_mutex_unlock(&global_malloc_lock);
    return;
  }
  //If the block of memory to be freed is not the last block of memory we'll just mark it as free
  header->s.is_free = 1;
  printf("[DEBUG FREE] Marcando bloque como libre. Dirección: %p\n", (void *)(header + 1));
  pthread_mutex_unlock(&global_malloc_lock);
}


