#include <stdio.h>

#define WIDTH 80
#define HEIGHT 24
#define MAX_ITER 1000

void mandelbrot(double zoom, double offsetX, double offsetY) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double zx, zy, cx, cy;
            cx = (x - WIDTH / 2.0) * 4.0 / (zoom * WIDTH) + offsetX;
            cy = (y - HEIGHT / 2.0) * 2.0 / (zoom * HEIGHT) + offsetY;
            zx = 0;
            zy = 0;
            int iteration = 0;
            while (zx * zx + zy * zy < 4 && iteration < MAX_ITER) {
                double temp = zx * zx - zy * zy + cx;
                zy = 2.0 * zx * zy + cy;
                zx = temp;
                iteration++;
            }

            if (iteration == MAX_ITER) {
                printf("\033[48;5;0m "); // Black background for points in the Mandelbrot set
            } else {
                int color = 232 + (iteration % 24); // Create a grayscale effect
                printf("\033[48;5;%dm ", color); // Set background color
            }
        }
        printf("\033[0m\n"); // Reset attributes and move to next line
    }
}

int main() {
    double zoom = 1.0;
    double offsetX = -0.5;
    double offsetY = 0.0;

    printf("Mandelbrot Set Visualization with Zoom, Shading, and Colors\n");
    printf("Use WASD keys to navigate, Z/X to zoom in/out, and Q to quit.\n");

    char command;
    while (1) {
        mandelbrot(zoom, offsetX, offsetY);

        command = getchar();
        switch (command) {
            case 'w': offsetY -= 0.1 / zoom; break;
            case 's': offsetY += 0.1 / zoom; break;
            case 'a': offsetX -= 0.1 / zoom; break;
            case 'd': offsetX += 0.1 / zoom; break;
            case 'z': zoom *= 1.1; break;
            case 'x': zoom /= 1.1; break;
            case 'q': return 0;
        }

        // Clear the screen (works on most Unix-like systems)
        printf("\e[1;1H\e[2J");
    }

    return 0;
}
