#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

bool file_exists(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

void undo(char *filename1, char *filename2) {
    rename(filename2, filename1);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: swapfile <filename1> <filename2>\n");
        exit(1);
    }

    char *filename1 = argv[1];
    char *filename2 = argv[2];

    if (!file_exists(filename1) || !file_exists(filename2)) {
        printf("Error: One or both files do not exist\n");
        exit(1);
    }

    size_t tmpPathNameLen = sizeof(filename1) + 5;
    char *tmpPathName = malloc(tmpPathNameLen);
    if (!tmpPathName) {
        perror("Error allocating memory foir temporary file name");
        exit(1);
    }

    snprintf(tmpPathName, tmpPathNameLen, "%s.tmp", filename1);

    if (rename(filename1, tmpPathName) != 0) {
        perror("Error renaming the first file to temporary file");
        free(tmpPathName);
        exit(1);
    }

    if (rename(filename2, filename1) != 0) {
        perror("Error renaming the second file to the first file");
        undo(tmpPathName, filename1);
        free(tmpPathName);
        exit(1);
    }

    if (rename(tmpPathName, filename2) != 0) {
        perror("Error renaming the temporary file to the second file");
        undo(filename1, tmpPathName);
        undo(filename2, filename1);
        free(tmpPathName);
        exit(1);
    }

    remove(tmpPathName);
    free(tmpPathName);
    return 0;
}


