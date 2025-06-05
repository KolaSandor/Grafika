SRC_DIR = src
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SOURCES))
TARGET  = $(BIN_DIR)/feladat.exe

CC      = gcc
CFLAGS  = -Wall -g -I$(SRC_DIR)
LDFLAGS = -lSDL2 -lSDL2_image -lopengl32 -lglu32

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	if not exist $(BIN_DIR) mkdir $(BIN_DIR)

clean:
	rm -f $(BIN_DIR)/*.o $(TARGET)
