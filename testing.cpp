#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

void* create_shared_memory(size_t size)
{
    int protection = PROT_READ | PROT_WRITE;

    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    return mmap(NULL, size, protection, visibility, -1, 0);
}

int main()
{
    char parent_message[] = "hello";
    char child_message[] = "goodbye";

    //Creating the shared memory
    void* shmem = create_shared_memory(128);

    //Assigning data to that shared memory that canbe used by both processes
    memcpy(shmem, parent_message, sizeof(parent_message));

    int pid = fork();

    if(pid == 0)
    {
        //Outputs value of shared memory currently
        printf("Child read: %s\n", shmem);
        //Writes to shared memory
        memcpy(shmem, child_message, sizeof(child_message));
        //Outputs new value
        printf("Child wrote: %s\n", shmem);
    }
    else
    {
        printf("Parent read: %s\n", shmem);
        sleep(1);
        printf("After 1s, parent read: %s\n", shmem);
    }

    return 0;
}