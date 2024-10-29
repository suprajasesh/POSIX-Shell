Implemented a custom shell program using C++

main.cpp
    - the terminal prompt is printed by getting username and hostname.
    - the file also maintains the sutom terminal home directory and the relative path to other directories within it if needed.
    - contains execution of other terminal commands which have not been custom implemented. This handles background processes also.
    - contains code for signal handling:
        - CTRL-Z pushs any running foreground job into the background, and changes its state from running to stopped.
        - CTRL-C interrupts any running foreground job, by sending it the SIGINT signal.
        - CTRL-D logs out of the custom shell, without any effect on the actual terminal.
    - based on the command entered by the user, the corresponding custom functions are called to implement them.

cd.cpp
    - contains implementation of cd command using chdir() system call
    - contains pwd implementation which prints the current directory relative to the custom terminal.
    - echo is also implemented in this. The content after the first non space character is printed.

ls.cpp
    - ls -l and ls -a implementation are present.
    - based on the flags given by the user the corresponding function is called.
    - stat along with getpwuid(), getgrgid() is l=used to print the long format of ls.

pinfo.cpp
    - checks if a program is foreground or background by checking the pid against the pid of the process running in the terminal.
    - from the /proc/pid/status and /proc/pid/exe files, the corresponding data for status, memory, executable path is read and displayed.
    - if the process is foreground, it contains a + in the status.
    - contains code for search also, where each directory is opened and recursively read to check if the name matches. This fuction returns true if it exists, false if the directory does not exist.

ioredirect.cpp
    - based on the redirect symbol given as input, corresponding flags are set and the input and output files are decided.
    - the files are created or opened for write and read respectively, and the file descriptors are redirected to perform the corresponding operation.
    - this calls the execvp() to implement the commands given by user and the redirection is taken care of.

mypipe.cpp
    - check_pipe() is a custom function used to handle piping. The output of a command is written to the pipe and the read end of the next command reads it and writes its output to the write end. The input and output file descriptors used to manage the ends of the pipe appropriately.
    - contains a function for handling redirect with pipe also which checks if the command has redirect symbols, and calls the code to redirect. The input and output of the redirect is handled in a similar way as done for normal pipes.

history.cpp
    - for every command the user enters, read it and write to a file called maintaiin_history.txt. The file maintains 20 commands executed by the user.
    - when the history is called the last 10 commands of the file are displayed.
    - if the user gives a number lesser than 20, then that many number of last executed commands are printed.
    - if the user has entered more than 20 commands, the last 20 are only maintained in the file.

others.cpp
    - contains function to tokenize string based on delimiters.
    - checks if a command is a background process or not
    - searches files within current directory to see if it matches with input for autocomplete

To run the program, give the command make main in terminal, followed by ./a.out.



