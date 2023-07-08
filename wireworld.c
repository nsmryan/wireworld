#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <time.h>

#include "tigr.h"

#define WIDTH 64
#define HEIGHT 64

Tigr *screen;
Tigr *backbuffer;

TPixel electronHead = { 0, 151, 151, 0 };
TPixel electronTail = { 255, 255, 255, 0 };
TPixel copper = { 255, 151, 5, 0 };
TPixel empty = { 0, 0, 0, 0 };

int main(int argc, char *argv)
{
    screen = tigrWindow(WIDTH, HEIGHT, "Wire World", TIGR_2X);
    backbuffer = tigrBitmap(WIDTH, HEIGHT);
    tigrClear(screen, empty);
    tigrClear(backbuffer, empty);

    int running = -1;

    int prevMouseButtons = 0;
    int prevMouseX = 0;
    int prevMouseY = 0;
    while (!tigrClosed(screen) && !tigrKeyDown(screen, TK_ESCAPE))
    {
        int mouseX;
        int mouseY;
        int mouseButtons;
        tigrMouse(screen, &mouseX, &mouseY, &mouseButtons);

        if (((prevMouseButtons & 1) == 0) && ((mouseButtons & 1) == 1))
        {
            if (memcmp(&backbuffer->pix[mouseX + mouseY * WIDTH], &electronHead, sizeof(electronHead)) == 0)
            {
                backbuffer->pix[mouseX + mouseY * WIDTH] = electronTail;
            }
            else if (memcmp(&backbuffer->pix[mouseX + mouseY * WIDTH], &electronTail, sizeof(electronTail)) == 0)
            {
                backbuffer->pix[mouseX + mouseY * WIDTH] = empty;
            }
            else
            {
                backbuffer->pix[mouseX + mouseY * WIDTH] = electronHead;
            }
        }

        if (((prevMouseButtons & 2) == 0) && ((mouseButtons & 2) == 2))
        {
            backbuffer->pix[mouseX + mouseY * WIDTH] = empty;
        }

        if (((prevMouseButtons & 4) == 0) && ((mouseButtons & 4) == 4))
        {
            backbuffer->pix[mouseX + mouseY * WIDTH] = copper;
        }
        prevMouseButtons = mouseButtons;
        prevMouseX = mouseX;
        prevMouseY = mouseY;

        if (tigrKeyDown(screen, TK_SPACE))
        {
            if (running < 0)
            {
                running = 0;
            }
            else
            {
                running = -1;
            }
        }

        int key = tigrReadChar(screen);
        while (key != 0)
        {
            if (key == 's')
            {
                running = 1;
            }
            key = tigrReadChar(screen);
        }

        if (running != 0)
        {
            if (running > 0)
            {
                running--;
            }

            for (int y = 0; y < HEIGHT; y++)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    int index = x + y * WIDTH;
                    if (memcmp(&backbuffer->pix[index], &copper, sizeof(copper)) == 0)
                    {
                        int sumHeads = 0;
                        for (int dx = -1; dx <= 1; dx++)
                        {
                            for (int dy = -1; dy <= 1; dy++)
                            {
                                if (dx == 0 && dy == 0)
                                {
                                    continue;
                                }

                                int localX = x + dx;
                                int localY = y + dy;
                                int localIndex = localX + localY * WIDTH;
                                if (memcmp(&backbuffer->pix[localIndex], &electronHead, sizeof(electronHead)) == 0)
                                {
                                    sumHeads++;
                                }
                            }
                        }

                        if (sumHeads == 1 || sumHeads == 2)
                        {
                            screen->pix[index] = electronHead;
                        }
                        else
                        {
                            screen->pix[index] = copper;
                        }
                    }
                    else if (memcmp(&backbuffer->pix[index], &electronHead, sizeof(electronHead)) == 0)
                    {
                        screen->pix[index] = electronTail;
                    }
                    else if (memcmp(&backbuffer->pix[index], &electronTail, sizeof(electronTail)) == 0)
                    {
                        screen->pix[index] = copper;
                    }
                    else
                    {
                        screen->pix[index] = empty;
                    }
                }
            }
        }
        else
        {
            tigrBlit(screen, backbuffer, 0, 0, 0, 0, WIDTH, HEIGHT);
        }
        tigrUpdate(screen);
        tigrBlit(backbuffer, screen, 0, 0, 0, 0, WIDTH, HEIGHT);

        struct timespec wait;
        wait.tv_sec = 0;
        wait.tv_nsec = 10000000;
        nanosleep(&wait, NULL);
        //printf("took %.3f\n", tigrTime());
    }

    tigrFree(screen);
    tigrFree(backbuffer);
}

