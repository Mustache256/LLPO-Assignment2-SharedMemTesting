#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

char* msg1 = "piped message";

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

    float f1 = 1.0f;
    float f2 = 2.0f;
    float f3 = 3.0f;

    char inbuf[sizeof(msg1)];
    float fbuf[sizeof(float)];
    //Creating the shared memory
    void* shmem = create_shared_memory(128);
    size_t msgsize = sizeof(msg1);
    //Assigning data to that shared memory that canbe used by both processes
    memcpy(shmem, parent_message, sizeof(parent_message));

    int p[2];
    int fp[2];
    int nbytes;

    if(pipe(p) < 0)
    {
        printf("Pipe failed");
        exit(1);
    }

    if(pipe(fp) < 0)
    {
        printf("Float Pipe failed");
        exit(1);
    }

    int pid = fork();

    if(pid == 0)
    {
        close(p[0]);
        write(p[1], msg1, strlen(msg1) + 1);
        close(p[1]);

        close(p[0]);
        write(fp[1], &f1, sizeof(float));
        write(fp[1], &f2, sizeof(float));
        write(fp[1], &f3, sizeof(float));
        close(p[0]);
        //Outputs value of shared memory currently
        printf("Child read: %s\n", shmem);
        //Writes to shared memory
        memcpy(shmem, child_message, sizeof(child_message));
        //Outputs new value
        printf("Child wrote: %s\n", shmem);
    }
    else
    {
        close(p[1]);
        while((nbytes = read(p[0], inbuf, strlen(msg1) + 1)) > 0)
            printf("% s\n", inbuf);
        if(nbytes != 0)
            exit(2);
        close(p[0]);

        close(fp[1]);
        /*while((nbytes = read(fp[0], fbuf, sizeof(10))) > 0)
            printf("%d\n", fbuf);
        if(nbytes != 0)
            exit(2);*/
        read(fp[0], fbuf, sizeof(float));
        printf("%f\n", fbuf);
        read(fp[0], fbuf, sizeof(float));
        printf("%f\n", fbuf);
        read(fp[0], fbuf, sizeof(float));
        printf("%f\n", fbuf);
        close(fp[0]);

        printf("Finished reading from pipe\n");
        printf("Parent read: %s\n", shmem);
        sleep(1);
        printf("After 1s, parent read: %s\n", shmem);
    }

    return 0;
}