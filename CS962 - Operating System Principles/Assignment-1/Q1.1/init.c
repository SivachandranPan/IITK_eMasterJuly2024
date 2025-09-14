#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

// Program expects two arguements argv[1]-> Search_term & argv[2] -> file_name to search
// If not provided, prints Error and then exit
int main (int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error\n");
        exit(1);
    }
    
    char *search_term = argv[1];
    char *file_name = argv[2];
    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int found = 0;
    int search_term_len = strlen(search_term);

    // Opens file in read-only mode & will exit in case could not Open
    fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error\n");
        exit(1);
    }
    
    // Reads the file chunk by chunk
     while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        
        // Compares substring of buffer with search_term
        for (int i = 0; i <= (bytes_read - search_term_len); i++) {
            // Check if the current character matches the start of the search term
            if (buffer[i] == search_term[0]) {
                // Copy substring into temp and null-terminate it
                char temp[search_term_len + 1];
                int j;
                for (j = 0; j < search_term_len; j++) {
                    temp[j] = buffer[i + j];
                }
                temp[j] = '\0'; // Null-terminate

                //Use strcmp to compare the substring (temp & search_term)
                if (strcmp(temp, search_term) == 0) {
                    found = 1; // Found the string, no need to continue searching
                    break; 
                }
                
            }
        }
        // search_term already found, and stops reading further
        if (found) {
            break;
        }
    }
    
    // Announcing the result of search
    if (found) {
        printf("FOUND\n");
    }
    else {
        printf("NOT FOUND\n");
    }

    if (close(fd) == -1) {
        fprintf(stderr, "Error\n");
        exit(1);
    }

    close(fd);
    return 0;
    
}
