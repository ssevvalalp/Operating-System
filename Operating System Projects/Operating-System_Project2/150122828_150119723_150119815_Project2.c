#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>   
#include <signal.h>   
#include <fcntl.h>

#define MAX_LINE 80
#define MAX_BOOKMARKS 100  
char *bookmarks[MAX_BOOKMARKS];
int bookmark_count = 0;
int background_processes_count = 0;


pid_t foreground_pid = -1;

char* find_command(const char *command);
void sigtstp_handler(int sig);
void exit_shell();
void search_directory(const char *dir_path, const char *pattern, int recursive);
void search_in_file(const char *file_path, const char *pattern);
void delete_bookmark(int index);
void execute_bookmark(int index);
void list_bookmarks() ;
void add_bookmark(const char *command);
void handle_redirection(char *args[], int *in_fd, int *out_fd, int *err_fd);


void setup(char inputBuffer[], char *args[],int *background)

{
    int length, 
        i,      
        start,  
        ct;     
    
    ct = 0;
        
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);

    start = -1;
    if (length == 0)
        exit(0);       


    if ( (length < 0) && (errno != EINTR) ) {
        perror("error reading the command");
    exit(-1);           // terminate with error code of -1
    }

    printf(">>%s<<",inputBuffer);
    for (i=0;i<length;i++){ // examine every character in the inputBuffer

        switch (inputBuffer[i]){
        case ' ':
        case '\t' :               // argument separators 
        if(start != -1){
                    args[ct] = &inputBuffer[start];    // set up the pointer
            ct++;
        }
                inputBuffer[i] = '\0'; // add a null char; make a C string
        start = -1;
        break;

            case '\n':                 // Should be the final character examined
        if (start != -1){
                    args[ct] = &inputBuffer[start];     
            ct++;
        }
                inputBuffer[i] = '\0';
                args[ct] = NULL; //No more arguments can be added
        break;

        default :         
        if (start == -1)
            start = i;
                if (inputBuffer[i] == '&'){
            *background  = 1;
                    inputBuffer[i-1] = '\0';
        }
    } 
     }    
     args[ct] = NULL;

    for (i = 0; i <= ct; i++)
        printf("args %d = %s\n",i,args[i]);
} 
 


int main(void) {
    char inputBuffer[MAX_LINE]; // Buffer to hold the command entered
    char *args[MAX_LINE / 2 + 1]; 
    int background; 
    int in_fd = -1, out_fd = -1, err_fd = -1;

   while (1) {
        // Reset file descriptors for each command
        in_fd = out_fd = err_fd = -1;
        
        printf("myshell: ");
        fflush(stdout);
        
        setup(inputBuffer, args, &background);
        handle_redirection(args, &in_fd, &out_fd, &err_fd);
        
         if (args[0] == NULL) {
            continue; // If no command is entered, restart the loop
        }


        if (strcmp(args[0], "exit") == 0) {
            exit_shell();
        } else if (strcmp(args[0], "bookmark") == 0) {
            // Handle bookmark commands
            if (args[1] == NULL) {
                printf("No bookmark command specified.\n");
            } else if (strcmp(args[1], "-l") == 0) {
                list_bookmarks();
            } else if (strcmp(args[1], "-i") == 0) {
                if (args[2] != NULL) {
                    int index = atoi(args[2]);
                    execute_bookmark(index);
                } else {
                    printf("No bookmark index specified.\n");
                }
            } else if (strcmp(args[1], "-d") == 0) {
                if (args[2] != NULL) {
                    int index = atoi(args[2]);
                    delete_bookmark(index);
                } else {
                    printf("No bookmark index specified.\n");
                }
            } else {
                add_bookmark(args[1]); // Add a new bookmark
            }
        } else if (strcmp(args[0], "search") == 0) {
            // Handle search command
            if (args[1] == NULL) {
                printf("No search pattern specified.\n");
            } else {
                int recursive = 0;
                if (strcmp(args[1], "-r") == 0) {
                    recursive = 1;
                    if (args[2] == NULL) {
                        printf("No search pattern specified for recursive search.\n");
                    } else {
                        search_directory(".", args[2], recursive);
                    }
                } else {
                    search_directory(".", args[1], recursive);
                }
            }
        } else {
    pid_t pid = fork();
        if (pid == 0) {
            // Set up redirection for the the child process
            if (in_fd != -1) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (out_fd != -1) {
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }
            if (err_fd != -1) {
                dup2(err_fd, STDERR_FILENO);
                close(err_fd);
            }

            char* command_path = find_command(args[0]);
            if (command_path != NULL) {
                execv(command_path, args);
                perror("execv");
                free(command_path);  // Free allocated memory
            } else {
                fprintf(stderr, "Command not found: %s\n", args[0]);
            }
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            //  Close file descriptors if they were opened in the parent process
            if (in_fd != -1) close(in_fd);
            if (out_fd != -1) close(out_fd);
            if (err_fd != -1) close(err_fd);
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            // For background process, don't wait here
            printf("Started background process with PID: %d\n", pid);
        }
    } else {
        // Fork failed
        perror("fork");
    }
}
    }

    return 0;
}


// Define a function to find a command in the system's PATH
char* find_command(const char *command) {

    // Retrieve the PATH environment variable
    char *path = getenv("PATH");
    
    // Make a copy of the path to avoid modifying the original environment variable
    char *path_copy = strdup(path);
    
    // Tokenize the path to get the first directory
    char *dir = strtok(path_copy, ":");


    // Iterate over each directory in the PATH
    while (dir != NULL) {

        // Allocate memory for the full path of the command
        char *full_path = malloc(strlen(dir) + strlen(command) + 2);

        // Construct the full path of the command
        sprintf(full_path, "%s/%s", dir, command);
        
        // Check if the command exists and is executable in this directory
        if (access(full_path, X_OK) == 0) {

            // If found, clean up and return the full path
            free(path_copy);
            return full_path;
        }
        // Free the allocated memory if command is not found in this directory
        free(full_path);

        // Move to the next directory in the PATH
        dir = strtok(NULL, ":");
    }
    // Clean up and return NULL if the command cannot be found in any directory
    free(path_copy);
    return NULL;
}



// Define a signal handler for the SIGTSTP signal
void sigtstp_handler(int sig) {

    // Check if there is a foreground process running 
    if (foreground_pid > 0) {

        // Send the SIGTSTP signal to the foreground process
        kill(foreground_pid, SIGTSTP);
    }
}


// Define a function to handle exiting the shell
void exit_shell() {

    // Check if there are any background processes running
    if (background_processes_count > 0) {

        // If there are background processes, notify the user
        printf("There are background processes still running.\n");
    } else {

        // If there are no background processes, exit the shell
        exit(0);
    }
}

// Define a function to search for a pattern in a file
void search_in_file(const char *file_path, const char *pattern) {

    // Open file for reading
    FILE *file = fopen(file_path, "r");

    if (!file) {
        perror("Error opening file");
        return;
    }

    // Define a variable to hold the compiled regular expression
    regex_t regex;
    // Compile the regular expression; if it fails, report an error and exit
    if (regcomp(&regex, pattern, REG_EXTENDED)) {
        fprintf(stderr, "Could not compile regex\n");
        fclose(file); // Close the file before returning
        return;
    }

    // Variables for reading lines 
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_number = 1;

    // Read each line from the file
    while ((read = getline(&line, &len, file)) != -1) {
        // If the regular expression matches the line
        if (regexec(&regex, line, 0, NULL, 0) == 0) {
            // Print the line number, file path, and the line itself
            printf("%d: %s -> %s", line_number, file_path, line);
        }
        // Increment the line number 
        line_number++;
    }

    // free the allocated line buffer, close the file, and free the regex
    free(line);
    fclose(file);
    regfree(&regex);
}


// Define a function to search for a pattern in files within a directory
void search_directory(const char *dir_path, const char *pattern, int recursive) {
    // Open the directory
    DIR *dir = opendir(dir_path);
    // Check if the directory was successfully opened
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        // Buffer to store the full path of the entry
        char path[1024];
        // Construct the full path of the entry
        int len = snprintf(path, sizeof(path)-1, "%s/%s", dir_path, entry->d_name);
        path[len] = 0;

        // Check if the entry is a directory
        if (entry->d_type == DT_DIR) {
            // If recursive search is enabled and the directory is not '.' or '..'
            if (recursive && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                // Recursively search the directory
                search_directory(path, pattern, recursive);
            }
        } else {
            // Check if the file is a C or header file based on its extension
            if (strstr(entry->d_name, ".c") || strstr(entry->d_name, ".C") ||
                strstr(entry->d_name, ".h") || strstr(entry->d_name, ".H")) {
                // Search for the pattern in the file
                search_in_file(path, pattern);
            }
        }
    }

    
    closedir(dir);
}


void add_bookmark(const char *command) {

    //if the bookmark list has reached its maximum capacity
    if (bookmark_count >= MAX_BOOKMARKS) {

        // Inform the user that the bookmark list is full
        printf("Bookmark list is full.\n");
        return;
    }
    // Add the command to the bookmarks list by duplicating the command string
    bookmarks[bookmark_count] = strdup(command);
    
    bookmark_count++;
}

// Define a function to list all stored bookmarks
void list_bookmarks() {

    // Iterate each bookmark in the bookmarks array
    for (int i = 0; i < bookmark_count; i++) {

        // Print the index and the bookmark command
        printf("%d \"%s\"\n", i, bookmarks[i]);
    }
}

// Define function to execute a command stored in bookmarks list by its index
void execute_bookmark(int index) {

    // Check if the provided index is within the valid range
    if (index < 0 || index >= bookmark_count) {
        // Inform the user if the index is invalid
        printf("Invalid bookmark index.\n");
        return;
    }
    // Execute the bookmarked command using the system function
    system(bookmarks[index]);
}

// Define a function to delete a bookmark from the bookmarks list based on its index
void delete_bookmark(int index) {

    // Check if the provided index is within the valid range
    if (index < 0 || index >= bookmark_count) {
        // Inform the user if the index is invalid
        printf("Invalid bookmark index.\n");
        return;
    }
    // Free the memory 
    free(bookmarks[index]);

    // Shift all bookmarks after the deleted one to fill the gap
    for (int i = index; i < bookmark_count - 1; i++) {
        bookmarks[i] = bookmarks[i + 1];
    }
    // Decrement the count of bookmarks
    bookmark_count--;
}

// Define a function to handle input/output redirection in a command
void handle_redirection(char *args[], int *in_fd, int *out_fd, int *err_fd) {

    // Iterate through each argument in the args array
    for (int i = 0; args[i] != NULL; i++) {

        // Check if the argument is '>' for standard output redirection
        if (strcmp(args[i], ">") == 0) {

            // Open the file for writing, create it if it doesn't exist, truncate if it does
            *out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            args[i] = NULL; 
        }

            // Check if the argument is '>>' for appending to the standard output
        else if (strcmp(args[i], ">>") == 0) {

            // Open the file, create if it doesn't exist
            *out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            args[i] = NULL;
        }
            
        else if (strcmp(args[i], "<") == 0) {

            // Open file for reading
            *in_fd = open(args[i + 1], O_RDONLY);
            args[i] = NULL;
        }
            // Check if the argument is '2>' for standard error redirection
        else if (strcmp(args[i], "2>") == 0) {

            // Open file for writing error output, create if it doesn't exist, truncate if it does
            *err_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            args[i] = NULL;
        }
    }
}
