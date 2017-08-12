# Makefile

MEMCTL_INC_DIR = memctl/include

SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = .

ERRFLAGS = -Wall -Wpedantic -Wno-gnu -Werror
CFLAGS   = -O3 -I$(SRC_DIR) -I$(MEMCTL_INC_DIR) $(ERRFLAGS)
ARFLAGS  = r

CORE_LIB = $(LIB_DIR)/libmemctl_core.a

KEXT_CORE_SRCS = core.c

KEXT_CORE_HDRS =

KEXT_CORE_OBJS := $(KEXT_CORE_SRCS:%.c=$(OBJ_DIR)/%.o)

vpath % $(SRC_DIR)

all: $(CORE_LIB)

$(OBJ_DIR)/%.o: %.c $(KEXT_CORE_HDRS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(CORE_LIB): $(KEXT_CORE_OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -rf -- $(OBJ_DIR) $(CORE_LIB)
