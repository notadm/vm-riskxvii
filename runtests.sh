PROGRAM="vm_riskxvii"
run() 
{
	gcc -Wall -Wvla -Werror -O0 -g -std=c11 $PROGRAM.c -o $PROGRAM

	touch $PROGRAM.out
	./$PROGRAM $1 > $PROGRAM.out
	cmp $2 $PROGRAM.out && echo -e "\033[0;32m[PASSED] \033[0m $1 $2" || echo -e "\033[0;31m[FAILED] \033[0m $1 $2"
	rm $PROGRAM.out

	rm $PROGRAM
}

run "tests/printa.mi" "tests/printa.out"
run "tests/dump_registers.mi" "tests/dump_registers.out"
run "tests/add.mi" "tests/add.out"
run "tests/sub.mi" "tests/sub.out"
run "tests/xor.mi" "tests/xor.out"
run "tests/or.mi"  "tests/or.out"
run "tests/and.mi" "tests/and.out"
run "tests/sll.mi" "tests/sll.out"
run "tests/srl.mi" "tests/srl.out"
run "tests/sra.mi" "tests/sra.out"
run "tests/andi.mi" "tests/andi.out"
run "tests/addi.mi" "tests/addi.out"
run "tests/ori.mi" "tests/ori.out"
run "tests/xori.mi" "tests/xori.out"
run "tests/lui.mi" "tests/lui.out"
run "tests/lb.mi" "tests/lb.out"
run "tests/lh.mi" "tests/lh.out"
run "tests/lw.mi" "tests/lw.out"
run "tests/lbu.mi" "tests/lbu.out"
run "tests/lhu.mi" "tests/lhu.out"
run "tests/slt.mi" "tests/slt.out"
run "tests/slti.mi" "tests/slti.out"
run "tests/sltu.mi" "tests/sltu.out"
run "tests/sltiu.mi" "tests/sltiu.out"
run "tests/malloc_1.mi" "tests/malloc_1.out"
run "tests/malloc_2.mi" "tests/malloc_2.out"
run "tests/malloc_3.mi" "tests/malloc_3.out"
run "tests/malloc_4.mi" "tests/malloc_4.out"
run "tests/malloc_5.mi" "tests/malloc_5.out"
