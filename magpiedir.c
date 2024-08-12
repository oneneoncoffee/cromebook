#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/statvfs.h>
#include <fnmatch.h>
#include <limits.h>
#define version "1.25"
// Function to check if the pattern matches only itself (indicating possible shell expansion)
int pattern_was_expanded(const char *pattern, const char *matched_file) {
    // If the pattern matches exactly one file and that file is the same as the pattern itself
    return (strcmp(pattern, matched_file) == 0);
}

void format_size(char *buffer, size_t size, long bytes) {
    const char *sizes[] = {"Bytes", "KB", "MB", "GB", "TB"};
    int i = 0;
    double dbl_size = bytes;

    while (dbl_size >= 1024 && i < 4) {
        dbl_size /= 1024;
        i++;
    }

    snprintf(buffer, size, "%.2f %s", dbl_size, sizes[i]);
}

void print_file_info(const char *path, const struct stat *file_stat) {
    char size_str[20];
    format_size(size_str, sizeof(size_str), file_stat->st_size);
    printf("%-40s\t%-12s\t", path, size_str);
    printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
}

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    struct statvfs fs_stat;
    char filepath[1024];
    long total_size = 0;
    const char *pattern = NULL;
    char resolved_path[PATH_MAX];
    int skip_total_size = 0;
    int match_count = 0;
    char first_match[1024] = {0};

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <directory> [\"<pattern>\"]\n", argv[0]);
        printf("For an advanced directory listing please use magpiell.\n");
        printf("Version:" version ".\n");
        return 1;
    }

    if (argc == 3) {
        pattern = argv[2];
    }

    if (realpath(argv[1], resolved_path) == NULL) {
        perror("realpath");
        return 1;
    }

    printf("Listing directory: %s\n\n", resolved_path);

    if ((dir = opendir(argv[1])) == NULL) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], entry->d_name);

        if (pattern && fnmatch(pattern, entry->d_name, 0) != 0) {
            continue;
        }

        if (stat(filepath, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        if (match_count == 0) {
            strncpy(first_match, entry->d_name, sizeof(first_match) - 1);
        }
        match_count++;

        if (S_ISREG(file_stat.st_mode)) {
            total_size += file_stat.st_size;
        }

        print_file_info(entry->d_name, &file_stat);

        // Introduce delay if more than 20 files are being listed
        if (match_count > 20) {
            usleep(100000);  // Sleep for 100 milliseconds (100,000 microseconds)
        }
    }

    closedir(dir);

    // Check if the pattern was expanded by the shell
    if (pattern && match_count == 1 && pattern_was_expanded(pattern, first_match)) {
        printf("\nTotal size of files: N/A \nSyntx error: malformed search \n(skipped due to unquoted pattern and/or malformed search pattern)\n");
    } else {
        char total_size_str[20];
        format_size(total_size_str, sizeof(total_size_str), total_size);
        printf("\nTotal size of files: %s\n", total_size_str);
    }

    if (statvfs(argv[1], &fs_stat) != 0) {
        perror("statvfs");
        return 1;
    }

    char free_space_str[20];
    format_size(free_space_str, sizeof(free_space_str), fs_stat.f_bsize * fs_stat.f_bavail);
    printf("Free space: %s\n", free_space_str);

    return 0;
}
