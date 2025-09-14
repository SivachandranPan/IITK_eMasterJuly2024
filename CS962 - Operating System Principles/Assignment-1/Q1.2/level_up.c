#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void reverse_str(char* str, int len) {
    int i = 0, j = len - 1;
    while (i < j) {
        char tmp = str[i];
        str[i++] = str[j];
        str[j--] = tmp;
    }
}

// Program expects 5 arguments, argv[1] -> Search_term, argv[2] -> file_name, argv[3] -> start_offset,
// argv[4] -> end_offset, argv[5] -> order (0 - normal, 1 - reverse before the start of search)
// It prints Error and exist, while the arguments provided are invalid.
int main (int argc, char *argv[]) {
    if (argc != 6) {
        printf("Error\n");
        return 1;
    }

    char *search_term = argv[1];
    char *file_name = argv[2];
    int start_offset = atoi(argv[3]);
    int end_offset = atoi(argv[4]);
    int order = atoi(argv[5]);

    // Negative offset values are invalid and prints Error
    if (start_offset < 0 || end_offset < 0 || (order != 0 && order != 1)) {
        printf("Error\n");
        return 1;
    }

    // Opens with Read only option
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        printf("Error\n");
        return 1;
    }

    // Identifying filse size with the help of lseek command
    // Checks start_offset & end_offset are within the boundary of file size, and notifies Invalid offset
    int file_size = lseek(fd, 0, SEEK_END);
    if (start_offset >= file_size || end_offset >= file_size || start_offset > end_offset) {
        printf("Invalid Offset\n");
        close(fd);
        return 1;
    }

    // calculates read section length, and allocates extra one byte for '\0' terminator
    int range_len = end_offset - start_offset + 1;
    char *buffer = (char *) malloc(range_len + 1);
    if (!buffer) {
        printf("Error\n");
        close(fd);
        return 1;
    }

    // Moves file pointer to start_offset, and reads exact range_len bytes
    lseek(fd, start_offset, SEEK_SET);
    int bytes_read = read(fd, buffer, range_len);
    if (bytes_read != range_len) {
        printf("Error\n");
        free(buffer);
        close(fd);
        return 1;
    }

    // NULL termination of buffer
    buffer[range_len] = '\0';

    // If order = 1, calls reverse_str function to swap characters both ends
    if (order == 1) {
        reverse_str(buffer, range_len);
    }

    // with the help of strstr command checks search_term exists in the selected text area or not
    // and announce the result of found or not found 
    if (strstr(buffer, search_term) != NULL) {
        printf("FOUND\n");
    }
    else {
        printf("NOT FOUND\n");
    }

    // free memory and close file
    free(buffer);
    close(fd);

    return 0;

}
