#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define MAX_ARG_LENGTH 512

// echo Hello World | xargs echo
// echo Hello World: stdout
// xargs echo: stdin (command)

// convert Hello World to behind xargs echo from stdin
// xargs echo Hello World\n

// first step: argc = 2; argv = ["xargs", "echo"]
// if argc < 2 => error command
// if argv[1] != "-n" => index = 1 or index = 3
// the goal is to set the "echo" command by index

// if using -n, must have argument after -n. Eg: 1 2 3 4 5 | xargs -n 1 echo

// Step 2: after that, copy the command to _argv
// and then, copy the rest of the arguments to _argv
// if the number of arguments is greater than MAXARG - 1, print error message

// Step 3: read from stdin
// if the character is '\n', replace it with '\0'
// fork a new process
// copy the arguments to _argv
// execute the command
// if exec failed, print error message
// wait for the child process to finish
// if the input is too long, print error message


// argc: index của argument
// argv: mảng các argument
int main(int argc, char *argv[]){
    int index = 0;
    int _argc = 0;
    char *_argv[MAXARG];


    // normaly xargs command is used like this: xargs -n 1 command
    // or xargs ./program command 
    // From xargs -> argc++
    // if xargs -n 1 command -> index = 3
    if (argc < 2){
        fprintf(2, "Not enough arguments provided.\n");
        exit(1);
    }

    
    if (strcmp(argv[1], "-n") == 0){
        index = 3;
    }else{
        index = 1;
    }

    // check if the command is provided
    // if not, print error message
    if (index >= argc){
        fprintf(2, "No command specified.\n");
        exit(1);
    }

    // copy the command to _argv
    _argv[_argc++] = argv[index];

    // copy the rest of the arguments to _argv
    // if the number of arguments is greater than MAXARG - 1, print error message
    // especially, it behind the command
    for (int i = index + 1; i < argc; i++){
        if (_argc >= MAXARG - 1){
            fprintf(2, "xargs: too many arguments\n");
            exit(1);
        }
        _argv[_argc++] = argv[i];
    }

    // read from stdin
    char buf[MAX_ARG_LENGTH];
    int i = 0;

    
    while (read(0, &buf[i], 1)){
        // if the character is '\n', replace it with '\0'
        if (buf[i] == '\n'){
            buf[i] = '\0';
            // fork a new process
            if (fork() == 0){
                // copy the arguments to _argv
                _argv[_argc] = buf;
                // execute the command
                exec(_argv[0], _argv);
                fprintf(2, "exec failed\n");
                exit(1);
            }else{
                wait(0);
            }
            i = 0;

        // the number of tokens is too long 
        }else{
            i++;
            if (i >= MAX_ARG_LENGTH - 1){
                fprintf(2, "Input too long.\n");
                exit(1);
            }
        }
    }

    exit(0);
}