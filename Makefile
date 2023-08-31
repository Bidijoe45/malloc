CC = gcc
CFLAGS = -g
NAME = malloc
LIBRARY_NAME = lib$(NAME).so
LIBRARY = $(BUILD_DIR)/$(LIBRARY_NAME)

#===============
#=== MALLOC ====
#===============

BUILD_DIR = ./build
OBJS_BUILD_DIR = ./build/objs
SRCS_DIR = ./src
SRCS = $(shell find $(SRCS_DIR) -type f -name '*.c')
OBJS = $(patsubst $(SRCS_DIR)/%.c, $(OBJS_BUILD_DIR)/%.o, $(SRCS))

$(OBJS_BUILD_DIR)/%.o:$(SRCS_DIR)/%.c
	$(CC) -c $< -o $@

all: $(LIBRARY)
$(NAME): $(LIBRARY)
$(LIBRARY): $(OBJS)
	@mkdir -p $(OBJS_BUILD_DIR)
	$(CC) -fPIC -shared -o $(LIBRARY) -lc $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(LIBRARY)

re: fclean
	$(LIBRARY)

#===============
#==== TESTS ====
#===============

TEST_BULID_DIR = ./test/build
TEST_OBJS_BUILD_DIR = ./test/build/objs
TEST_SRCS_DIR = ./test/src
TEST_SRCS = $(shell find $(TEST_SRCS_DIR) -type f -name '*.c')
TEST_OBJS = $(patsubst $(TEST_SRCS_DIR)/%.c, $(TEST_OBJS_BUILD_DIR)/%.o, $(TEST_SRCS))

TEST_NAME = test
TEST = $(TEST_BULID_DIR)/$(TEST_NAME)

$(TEST_OBJS_BUILD_DIR)/%.o:$(TEST_SRCS_DIR)/%.c
	$(CC) -I$(SRCS_DIR) -c $< -o $@

test: $(TEST)
$(TEST): $(OBJS) $(TEST_OBJS)
	mkdir -p $(TEST_OBJS_BUILD_DIR)
	$(CC) $(TEST_OBJS) -I$(SRCS_DIR) -L$(BUILD_DIR) -l$(NAME) -o $(TEST)
	
test_clean:
	rm -f $(TEST_OBJS)

test_fclean: test_clean
	rm -rf $(TEST)

test_re: test_fclean 
