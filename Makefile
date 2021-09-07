SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := .

EXE := $(BIN_DIR)/gemcli
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CPPFLAGS := -Iinclude -MMD -MP -I/usr/local/opt/openssl/include
LDFLAGS := -L/usr/local/opt/openssl/lib -lcrypto -lssl
CFLAGS := -Wall -g

all: $(EXE)

.PHONY: all

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(OBJ_DIR)

.PHONY: clean

-include $(OBJ:.o=.d)