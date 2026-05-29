export VK_LAYER_PATH = $(CURDIR)/vulkan/Bin

CC = gcc
CFLAGS = -Ivulkan/Include
LDFLAGS = -Lvulkan/Lib -lgdi32 -luser32 -ldwmapi -lpsapi
SRC = *.c

.PHONY: all clean

all:
	$(CC) $(CFLAGS) -o main.exe $(SRC) $(LDFLAGS) && main.exe
 
clean:
	del /Q  *.o main.exe
