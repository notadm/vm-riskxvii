TARGET 		= vm_riskxvii
CC 			= gcc
OPTIMISE   	= -Os -s -nostartfiles -ffunction-sections -fdata-sections -flto
CFLAGS     	= -c -Wall -Wvla -Werror -O0 -std=c11 
ASAN_FLAGS 	= -fsanitize=address
SRC        	= vm_riskxvii.c
OBJ        	= $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(OPTIMISE) $<

run_tests: 
	@echo "RUNNING TESTS"
	bash runtests.sh

vm:
	$(CC) $(TARGET).c -o $(TARGET)

add:
	$(CC) $(TARGET).c -o $(TARGET)
	./$(TARGET) tests/add.mi

clean:
	rm -f *.o *.obj $(TARGET)
	rm -rf vm_riskxvii.dSYM
