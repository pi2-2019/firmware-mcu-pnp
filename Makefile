MODULE := pnp_control
EXE := $(MODULE).elf

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC = /opt/msp430/msp430-gcc-7.3.2.154_linux64/bin/msp430-elf-gcc

CPPFLAGS = -I/opt/msp430/msp430-gcc-support-files/include -Iinclude

DEBUG :=
OPTIMIZATION := -Os -fdata-sections -ffunction-sections -fno-math-errno \
-Wl,--gc-sections -Wl,--print-gc-sections -Wl,--cref -Wl,-Map=output.map
WARNINGS := -Wall -Wextra -pedantic -D_FORTIFY_SOURCE=1 -Wformat-overflow=2 \
-Wformat-security -Wformat-truncation=2
MSPFLAGS := -mmcu=msp430g2553 -mhwmult=auto -minrt
OTHER := $(OPTIMIZATION) $(WARNINGS)
CFLAGS := $(DEBUG) $(OTHER) $(MSPFLAGS)

LDFLAGS = -L/opt/msp430/msp430-gcc-support-files/include
LDLIBS =

$(EXE): $(OBJ)
	@echo "Building target: $@"
	@echo "Invoking: GCC C Linker"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)
	@echo "Finished building target: $@"
	@echo " "

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo "Building file: $<"
	@echo "Invoking GCC C Compiler"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -o"$@" "$<"
	@echo "Finished building> $<"
	@echo " "

clean:
	$(RM) $(EXE) $(OBJ)
	
devclean:
	make clean
	sleep 1
	clear
	
devredo:
	make devclean
	make
