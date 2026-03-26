CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = 
TARGET = webserver
SRC_DIR = .
OBJ_DIR = obj
INC_DIR = ../include

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

.PHONY: clean all

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

all: $(TARGET)
