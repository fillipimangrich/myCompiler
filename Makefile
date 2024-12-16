CC = gcc
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
OBJ_DIR = obj
TARGET = $(BIN_DIR)/macacompiler
CFLAGS = -I$(INC_DIR) -Wall -pedantic -std=c99

# Lista de arquivos fonte
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Gera os arquivos objeto correspondentes
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Alvo principal para compilar o projeto
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

# Compilação de cada arquivo objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

# Comando build para compilar o projeto
build: $(TARGET)

# Limpa os binários e objetos
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

# Executa o teste com o arquivo test.caco
test1: $(TARGET)
	$(TARGET) test1.caco

# Executa o teste com o arquivo test.caco
test2: $(TARGET)
	$(TARGET) test2.caco

# Executa o teste com o arquivo test.caco
test3: $(TARGET)
	$(TARGET) test3.caco


