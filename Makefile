TARGET = P2

# Compilador y opciones de compilación
CC = gcc
CFLAGS = -Wall -g	

# Lista de archivos fuente y objetos
SRCS = P2.c lista.c comandos.c
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
