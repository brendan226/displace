export VK_LAYER_PATH = $(CURDIR)/vulkan/Bin

CC = gcc
CFLAGS = -Ivulkan/Include
LDFLAGS = -Lvulkan/Lib -lgdi32 -luser32 -ldwmapi -lpsapi
SRC = *.c
BINARY = Displace.exe

.PHONY: all clean

all:
	$(CC) $(CFLAGS) -o Displace.exe $(SRC) $(LDFLAGS) && $(BINARY)
 
clean:
	del /Q  *.o $(BINARY)
