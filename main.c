#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/wait.h>

int main() {

    const size_t WRITE_SIZE = 1;
    char buffer[2 * WRITE_SIZE + 1];

    //fopen returns FILE*, open returns file descriptors.
    //int fd = open( "pipe.txt", O_WRONLY | O_TRUNC| O_CREAT, S_IWGRP );
    FILE* out = fopen( "pipe.txt", "a");
    int fd[2];
    pipe( fd );

    pid_t pid_a, pid_b;
    pid_a = fork();

    int CBYTES;

    switch (pid_a) {

        case -1:
            // This is a fail state
            perror( "Error forking child a: ");
            return  -1;

        case 0:
            //child process always returns 0 from a fork() call
            //child_a

            //child a should close input.
            close( fd[0] );
            int BBYTES = write(fd[1], "B", WRITE_SIZE );

            //DEBUG:
            printf("B branch has executed and written %d bytes\n", BBYTES);
            break;

        default:
            //for the parent, which apparently has an arbitrary PID
            //printf("Parent PID is: %u\n", pid_a);

            //Now spawn a second child only in the parent code
            pid_b = fork();

            //Here's where it gets ugly.  I don't love the nestedness here
            //TODO: refactor?
            switch (pid_b) {

                case -1:

                    perror( "Error on second fork: ");
                    return -1;


                case 0:
                    //code for child b
                    //close( fd[0] );
                    CBYTES = write(fd[1], "C", WRITE_SIZE );

                    //DEBUG:
                    printf("C branch has executed and written %d bytes\n", CBYTES);
                    break;

                default:
                    //code for parent

                    wait(&pid_a);
                    wait(&pid_b);

                    //null terminate the buffer so that file functions behave
                    write( fd[1], "\0", 1 );
                    close( fd[1] );

                    int RBYTES = read(fd[0], buffer, sizeof(buffer) );
                    fwrite ( "A", 1, 1, out );

                    //DEBUG:
                    printf("Read branch has executed and read %d bytes\n", RBYTES);
                    printf("Buffer is of length %d and currently contains: %s", (int) sizeof(buffer), buffer );

                    //Reminder, "buffer" decays into a pointer to its first element.
                    // `buffer` is the same as referencing `&buffer[0]'
                    fwrite( buffer, 1, sizeof(buffer) - 1, out );
                    break;
            }

            break;

    }

    return 0;
}




