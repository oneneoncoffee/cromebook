//
//  MAGPIE 16
//  Open source for Cromebooks and/or Linux platforms
//
//////////////////////

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/stat.h>

#define CHUNK_SIZE 16
#define DELAY_MICROSECONDS 80000*2
#define CHUNK_SIZE_2 1024

void print_file_size(long size) {
    if (size < 1024) {
        printf("File Size: %ld bytes\n", size);
    } else if (size < 1024 * 1024) {
        printf("File Size: %.2f KB\n", (float)size / 1024);
    } else if (size < 1024 * 1024 * 1024) {
        printf("File Size: %.2f MB\n", (float)size / (1024 * 1024));
    } else if (size < 1024LL * 1024 * 1024 * 1024) {
        printf("File Size: %.2f GB\n", (float)size / (1024 * 1024 * 1024));
    } else {
        printf("File Size: %.2f TB\n", (float)size / (1024LL * 1024 * 1024 * 1024));
    }
}

uint32_t crc32_table[256];

void generate_crc32_table() {
    const uint32_t polynomial = 0xEDB88320;

    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j) {
            crc = (crc & 1) ? (crc >> 1) ^ polynomial : crc >> 1;
        }
        crc32_table[i] = crc;
    }
}

uint32_t calculate_crc32(const uint8_t *data, size_t size) {
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < size; ++i) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

uint16_t calculate_crc16(const uint8_t *data, size_t size) {
    uint16_t crc = 0;

    for (size_t i = 0; i < size; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; ++j) {
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
        }
    }

    return crc;
}

void hex_dump(FILE *file, size_t offset, size_t size, double percentage) {
    fseek(file, offset, SEEK_SET);

    uint8_t buffer[CHUNK_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        printf("%04lx | ", offset);
        for (size_t i = 0; i < bytesRead; ++i) {
            printf("%02X ", buffer[i]);
        }

        printf("| ");
        for (size_t i = 0; i < bytesRead; ++i) {
            if (buffer[i] >= 32 && buffer[i] <= 126) {
                printf("%c", buffer[i]);
            } else {
                printf(".");
            }
        }

        uint16_t crc16 = calculate_crc16(buffer, bytesRead);

        printf(" | CRC16: 0x%04" PRIx16 "\n", crc16);

        offset += bytesRead;

        percentage = (double)offset / size * 100;
        if (percentage > 100.0) {
            percentage = 100.0;  // Cap at 100.00%
        }

        printf("Dumped %.2f%%  ", percentage);

        usleep(DELAY_MICROSECONDS);
    }

    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    struct stat fileStat;
    if (stat(argv[1], &fileStat) == 0) {
    if (S_ISREG(fileStat.st_mode)) {
    printf("File %s matching a regular file type.\n", argv[1]);
    }
    if (S_ISDIR(fileStat.st_mode)) {
    printf("This is a directory  [%s]\n", argv[1]);
    }
    }
    const char *filename = argv[1];
    FILE *file = fopen(filename, "rb");

    if (!file) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (file_size == 0) {
    printf("The file is A directory or zero total bytes?\n");
    }
    printf("File %s Size: %lu ..\n", argv[1], file_size);

    usleep(DELAY_MICROSECONDS);
    size_t offset = 0;
    double percentage = 0.0;

    hex_dump(file, offset, file_size, percentage);
    fclose(file);

    const char *filename2 = argv[1];
    FILE *file2 = fopen(filename2, "rb");

    if (!file2) {
        perror("Error opening file");
        return 1;
    }

    generate_crc32_table();

    uint8_t buffer[CHUNK_SIZE_2];
    size_t bytesRead;
    uint32_t crc32 = 0xFFFFFFFF;

    while ((bytesRead = fread(buffer, 1, CHUNK_SIZE_2, file2)) > 0) {
        crc32 = calculate_crc32(buffer, bytesRead);
    }

    fclose(file2);
    printf("File name: %s CRC32: 0x%08" PRIX32 "\n", argv[1],crc32);
    if (stat(argv[1], &fileStat) == 0) {
    print_file_size(fileStat.st_size);
    if(fileStat.st_size == 0) {
    printf("3rd file check empty okay exiting..\n");
    return 0;
    }
    printf("Number of Links: %ld", fileStat.st_nlink);
    printf(" File inode: %ld\n", fileStat.st_ino);
    printf("File Permissions: %o\n", fileStat.st_mode);
    } else {
    perror("stat");
    return 1;
    }
    return 0;
}
