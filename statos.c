// File name: statos.c  
// Made for the Orange Pi Zero 3 and/or Orange pi pc plus computers.
//Rewrite for the cromebook Lean, mean, amazing machine.
// 
// Updated version: 09/17/2024 
// 1, I corrected buggy outout of statistics.  
// 2, enhancment of memory and cpu expandabillity options on other systems. 
// 3, A more human readable format such as bytes, KB, MB, GB, TB and so on. 
// 4, System WiFi stats working 
// 5, Cpu zone temperature sensor checking !
// (WIll show total CPUs checked:0  on older chromebooks)
// 
// header liberys for this project.
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_link.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <unistd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 128

const char* format_size(unsigned long size) {
    static char buffer[50];
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;

    double display_size = size;

    while (display_size >= 1024 && unit_index < 4) {
        display_size /= 1024;
        unit_index++;
    }

    snprintf(buffer, sizeof(buffer), "%.2f %s", display_size, units[unit_index]);
    return buffer;
}

void print_cpu_info() {
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo == NULL) {
        perror("Error opening /proc/cpuinfo");
        return;
    }

    char line[LINE_SIZE];
    char cpuType[64] = "";
    int coreCount = 0;

    while (fgets(line, sizeof(line), cpuinfo)) {
        if (strstr(line, "model name") != NULL) {
            sscanf(line, "model name : %[^\n]", cpuType);
        }
        if (strstr(line, "processor") != NULL) {
            coreCount++;
        }
    }

    fclose(cpuinfo);

    printf("║ CPU Type: %s\n", cpuType);
    printf("║ Number of Cores: %d\n", coreCount);
}

void get_memory_usage(double *total, double *free) {
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL) {
        perror("Error opening /proc/meminfo");
        return;
    }

    char line[LINE_SIZE];
    double value;
    while (fgets(line, sizeof(line), meminfo)) {
        if (sscanf(line, "MemTotal: %lf kB", &value) == 1) {
            *total = value;
        } else if (sscanf(line, "MemFree: %lf kB", &value) == 1) {
            *free = value;
        }
    }

    fclose(meminfo);
}

void print_disk_usage() {
    struct statvfs fs_info;
    if (statvfs("/", &fs_info) != 0) {
        perror("Error getting disk info");
        return;
    }

    unsigned long total = fs_info.f_blocks * fs_info.f_frsize / 1024;
    unsigned long free = fs_info.f_bfree * fs_info.f_frsize / 1024;
    unsigned long used = (fs_info.f_blocks - fs_info.f_bfree) * fs_info.f_frsize / 1024;

    printf("║ Disk Total: %s\n", format_size(total * 1024));
    printf("║ Disk Used: %s\n", format_size(used * 1024));
    printf("║ Disk Free: %s\n", format_size(free * 1024));
}

void print_network_info() {
    struct ifaddrs *ifaddr, *ifa;
    char ip_addr[INET_ADDRSTRLEN];
    char gateway[INET_ADDRSTRLEN] = "Not Available";

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    printf("║ Network Interfaces:\n");
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &sa->sin_addr, ip_addr, sizeof(ip_addr));

            if (strcmp(ifa->ifa_name, "eth0") == 0 || strcmp(ifa->ifa_name, "wlan0") == 0) {
                printf("║ %s: IP Address: %s\n", ifa->ifa_name, ip_addr);
                if (ifa->ifa_flags & IFF_UP) {
                    printf("║ %s is UP\n", ifa->ifa_name);
                } else {
                    printf("║ %s is DOWN\n", ifa->ifa_name);
                }
            }
        }
    }

    freeifaddrs(ifaddr);

    // Fetch gateway information
    FILE *fp = fopen("/proc/net/route", "r");
    if (fp) {
        char line[LINE_SIZE];
        while (fgets(line, sizeof(line), fp)) {
            char iface[IFNAMSIZ];
            char dest[32];
            char gateway_str[32];
            unsigned int dest_addr;
            unsigned int gateway_addr;

            if (sscanf(line, "%s %x %x", iface, &dest_addr, &gateway_addr) == 3) {
                if (dest_addr == 0) {  // Default route
                    snprintf(gateway_str, sizeof(gateway_str), "%d.%d.%d.%d",
                             (gateway_addr & 0xFF),
                             (gateway_addr >> 8) & 0xFF,
                             (gateway_addr >> 16) & 0xFF,
                             (gateway_addr >> 24) & 0xFF);
                    snprintf(gateway, sizeof(gateway), "%s", gateway_str);
                }
            }
        }
        fclose(fp);
    }

    printf("║ Default Gateway: %s\n", gateway);
}

void print_kernel_version() {
    struct utsname uname_data;
    if (uname(&uname_data) != 0) {
        perror("uname");
        return;
    }

    printf("║ Kernel Version: %s\n", uname_data.release);
}

void print_cpu_temperature() {
    int cpu_count = 0;
    FILE *file;
    char path[40];
    printf("║ Checking CPU temperature:\n");

    while (1) {
        snprintf(path, sizeof(path), "/sys/class/thermal/thermal_zone%d/temp", cpu_count);
        file = fopen(path, "r");
        if (file == NULL) {
            break;
        }

        int temperature;
        fscanf(file, "%d", &temperature);
        fclose(file);
        printf("║ CPU%d Temperature: %d°C\n", cpu_count, temperature / 1000);
        cpu_count++;
    }

    printf("║ Total CPUs checked: %d\n", cpu_count);
}

void print_system_info() {
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0) {
        perror("sysinfo");
        return;
    }

    printf("\033[4;40m            System Information                  \033[0;m\n");
    printf("╔═════════════════════════════════════════════════\n");

    int days = sys_info.uptime / 86400;
    int hours = (sys_info.uptime % 86400) / 3600;
    int mins = (sys_info.uptime % 3600) / 60;
    int secs = sys_info.uptime % 60;
    printf("║ Uptime: %ddays, %dhours, %dminutes, %dseconds\n", days, hours, mins, secs);

    printf("║ Load Averages: 1min(%ld) 5min(%ld) 15min(%ld)\n",
           sys_info.loads[0], sys_info.loads[1], sys_info.loads[2]);

    printf("║ Total RAM: %s \t Free: %s\n", format_size(sys_info.totalram * sys_info.mem_unit), format_size(sys_info.freeram * sys_info.mem_unit));
    printf("║ Shared RAM: %s \t Buffered RAM: %s\n", format_size(sys_info.sharedram * sys_info.mem_unit), format_size(sys_info.bufferram * sys_info.mem_unit));
    printf("║ Total Swap: %s \t Free Swap: %s\n", format_size(sys_info.totalswap * sys_info.mem_unit), format_size(sys_info.freeswap * sys_info.mem_unit));
    printf("║ Total High Memory: %s \t Free High Memory: %s\n",
           format_size(sys_info.totalhigh * sys_info.mem_unit), format_size(sys_info.freehigh * sys_info.mem_unit));
    printf("║ Total Number of Processes: %d\n", sys_info.procs);

    gid_t gid = getgid();
    struct group *grp = getgrgid(gid);
    if (grp == NULL) {
        perror("getgrgid");
        return;
    }

    printf("║ Group ID: %d (Group %s)\n", gid, grp->gr_name);
    printf("║ Users in your group: ");
    int user_count = 0;
    for (char **users = grp->gr_mem; *users != NULL; users++) {
        user_count++;
    }
    printf("%d\n", user_count);

    if (strcmp(grp->gr_passwd, "x") == 0) {
        printf("║ Password is protected by shadow file.\n");
    } else {
        printf("║ Password: %s\n", grp->gr_passwd);
    }
}

int main() {
    system("clear");
    print_system_info();
    print_disk_usage();
    print_network_info();
    print_kernel_version();
    print_cpu_temperature();

    double total_memory, free_memory;
    get_memory_usage(&total_memory, &free_memory);
    double used_memory = total_memory - free_memory;
    double used_percentage = (used_memory / total_memory) * 100;
    double free_percentage = (free_memory / total_memory) * 100;

    printf("║ Total Memory: %s\n", format_size(total_memory * 1024));
    printf("║ Used Memory: %s (%.2lf%%)\n", format_size(used_memory * 1024), used_percentage);
    printf("║ Free Memory: %s (%.2lf%%)\n", format_size(free_memory * 1024), free_percentage);

    print_cpu_info();

    return 0;
}
