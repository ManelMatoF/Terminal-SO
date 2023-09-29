# Nombre del programa final
TARGET = mi_shell

# Compilador y opciones de compilación
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Lista de archivos fuente y objetos
SRCS = main.c lista.c comandos.c
OBJS = $(SRCS:.c=.o)

# Reglas de compilación
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
