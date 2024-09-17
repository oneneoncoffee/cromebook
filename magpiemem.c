//*
//* Program Notes:
//* Install ncurses library if you don't have it!
//* sudo apt-get install libncurses5-dev libncursesw5-dev
//* How do I use gcc to make this program:
//* gcc magpiemem.c -o magpiemem -lncurses

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <ncurses.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MEM_SIZE 1024
#define PAGE_SIZE 16
void print_process_memory(const char *proc_path, size_t offset) {
    char mem_file[256];
    snprintf(mem_file, sizeof(mem_file), "/proc/%s/mem", proc_path);

    int fd = open(mem_file, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    unsigned char buffer[MEM_SIZE];
    lseek(fd, offset, SEEK_SET);
    read(fd, buffer, MEM_SIZE);
    close(fd);

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(100);

    WINDOW *win = newwin(LINES, COLS, 0, 0);

    size_t size = MEM_SIZE;
    size_t view_offset = 0;

    while (1) {
        werase(win);
        for (size_t i = view_offset; i < view_offset + PAGE_SIZE && i < size; i += 16) {
            mvwprintw(win, (i - view_offset) / 16, 0, "%08zx  ", i + offset);
            for (size_t j = 0; j < 16; ++j) {
                if (i + j < size)
                    mvwprintw(win, (i - view_offset) / 16, 10 + j * 3, "%02x", buffer[i + j]);
                else
                    mvwprintw(win, (i - view_offset) / 16, 10 + j * 3, "  ");
            }
            mvwprintw(win, (i - view_offset) / 16, 50, " |");
            for (size_t j = 0; j < 16; ++j) {
                if (i + j < size) {
                    char c = (buffer[i + j] >= 32 && buffer[i + j] <= 126) ? buffer[i + j] : '.';
                    mvwprintw(win, (i - view_offset) / 16, 52 + j, "%c", c);
                } else {
                    mvwprintw(win, (i - view_offset) / 16, 52 + j, " ");
                }
            }
            mvwprintw(win, (i - view_offset) / 16, 68, "|");
        }
        wrefresh(win);

        int ch = getch();
        switch (ch) {
            case KEY_UP:
                if (view_offset > 0) {
                    view_offset -= PAGE_SIZE;
                    if (view_offset < 0) view_offset = 0;
                }
                break;
            case KEY_DOWN:
                if (view_offset + PAGE_SIZE < size) {
                    view_offset += PAGE_SIZE;
                }
                break;
            case 'q':
                endwin();
                return;
        }
    }

    endwin();
}


void print_memory_old(const unsigned char *mem, size_t size) {
    for (size_t i = 0; i < size; i += 16) {
        printf("%08zx:%08lx  ", i, (unsigned long) &mem[i]);
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < size)
                printf("%02x ", mem[i + j]);
            else
                printf("   ");
        }
        printf(" |");
        for (size_t j = 0; j < 16; ++j) {
            if (i + j < size)
                printf("%c", (mem[i + j] >= 32 && mem[i + j] <= 126) ? mem[i + j] : '.');
            else
                printf(" ");
        }
        printf("|\n");
    }
}



void list_background_processes() {
    DIR *proc_dir = opendir("/proc");
    struct dirent *entry;

    if (proc_dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(proc_dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // We assume directories starting with digits are process IDs
            if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') {
                char path[256];
                snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
                FILE *stat_file = fopen(path, "r");
                if (stat_file) {
                    char comm[256];
                    int state;
                    fscanf(stat_file, "%*d %s %c", comm, &state);
                    printf("PID: %s, Command: %s, State: %c\n", entry->d_name, comm, state);
                    fclose(stat_file);
                }
            }
        }
    }
    closedir(proc_dir);
}

void check_root() {
    if (getuid() != 0) {
        fprintf(stderr, "This program requires root privileges to access process memory.\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
if (argc != 2) {
    printf("\nBackground Processes:\n");
    list_background_processes();
    fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
    check_root();
    return 1;
    } else {
    print_process_memory(argv[1], 0);
    check_root();
    }
    return 0;
}
