TARGET 		= vm_riskxvii
CC 			= gcc
OPTIMISE   	= -Os -s -nostartfiles -ffunction-sections -fdata-sections -flto
CFLAGS     	= -Wall -Wvla -Werror -O0 -std=c11
ASAN_FLAGS 	= -fsanitize=address
SRC        	= src/vm_riskxvii.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(OPTIMISE) $< -o $@

run_tests:
	@echo "RUNNING TESTS"
	bash runtests.sh

vm:
	$(CC) $(SRC) -o $(TARGET)

add:
	$(CC) $(SRC) -o $(TARGET)
	./$(TARGET) tests/add.mi

clean:
	rm -f *.o *.obj $(TARGET)
	rm -rf vm_riskxvii.dSYM
