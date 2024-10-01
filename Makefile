# Definição das variáveis
CC = gcc
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
TARGET = $(BIN_DIR)/macacompiler
CFLAGS = -Wall -pedantic -std=c99

# Lista de arquivos fonte
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Gera os arquivos objeto correspondentes
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Regra padrão para compilar o projeto
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

# Regra para compilar cada arquivo objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

# Limpa os binários e objetos
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
