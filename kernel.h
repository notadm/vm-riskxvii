#ifndef KERNEL_H
#define KERNEL_H

// KERNELS

#include "vm.h"
#include "alloc.h"

i32 dumpKernel(u32 op, u32 v)
{ 
    switch (op)                                                                 
    {                                                                          
	case VR_WR_CHAR: 	printf("%c", *_U08(&VM.M[v]));clr(v); 	  break;
	case VR_WR_INT: 	printf("%d", *_I32(&VM.M[v]));clr(v); 	  break;
	case VR_WR_UINT: 	printf("%x", *_U32(&VM.M[v]));clr(v); 	  break;
	case VR_DUMP_WORD: 	printf("%d", *_U32(&VM.M[v]));            break;
    }
    return 0;
}

i32 outKernel(u32 op, u32 v) 
{ 
    switch (op)                                                                 
    {                                                                          
	case VR_WR_CHAR: 	dumpKernel(VR_WR_CHAR,   v);              break;
	case VR_WR_INT: 	dumpKernel(VR_WR_INT,    v);              break;
	case VR_WR_UINT: 	dumpKernel(VR_WR_UINT,   v);              break;
	case VR_DUMP_WORD: 	dumpKernel(VR_DUMP_WORD, v);              break;
	case VR_DUMP_PC: 	dumpPC();                                 break;
	case VR_DUMP_REG: 	dumpRegisterBanks();                      break;
	case VR_HALT: 		halt();                                   break;
    }
    return 0;
}

i32 inKernel(u32 op, u32 v) 
{ 
    clr(v); 
    switch (op)                                                                 
    {                                                                          
	case VR_RD_CHAR: 	scanf("%c", _I08(&VM.M[v]));              break;
	case VR_RD_INT: 	scanf("%d", _I32(&VM.M[v]));              break;
    }
    
    return 0;
}

i32 heapKernel(u32 op, u32 v) 
{ 
    switch (op)                                                                 
    {                                                                          
	case VR_MALLOC: 	$rmalloc = vmalloc(*_U32(&VM.M[v]));      break;
	case VR_FREE: 		vmfree(*_U32(&VM.M[v]));                  break;
    }
    clr(v); 
    return 0;
}

i32 VRkernel(u32 v) 
{                                              
    switch (v)                                                                 
    {                                                                          
	case VR_WR_CHAR: 	outKernel	(VR_WR_CHAR,   v);	  break;
	case VR_WR_INT: 	outKernel	(VR_WR_INT,    v); 	  break;
	case VR_WR_UINT: 	outKernel	(VR_WR_UINT,   v); 	  break;
	case VR_DUMP_PC: 	outKernel	(VR_DUMP_PC,   v); 	  break;
	case VR_DUMP_REG: 	outKernel	(VR_DUMP_REG,  v); 	  break;
	case VR_DUMP_WORD: 	outKernel	(VR_DUMP_WORD, v); 	  break;
	case VR_RD_CHAR: 	inKernel	(VR_RD_CHAR,   v);	  break;
	case VR_RD_INT: 	inKernel	(VR_RD_INT,    v);	  break;
	case VR_MALLOC: 	heapKernel	(VR_MALLOC,    v);	  break; 
	case VR_FREE: 		heapKernel	(VR_FREE,      v);	  break; 
	case VR_HALT: 		outKernel	(VR_HALT,      v);	  break; 
    }
    return 0;
}

i32 ALUKernel(u32 op, u32 x) 
{                                                                              
    switch (op)                                                                
    {                                                                          
	case M_ADD:   	$rd = I32($rs1) +  I32($rs2);                     break;
	case M_SUB:   	$rd = I32($rs1) -  I32($rs2);                     break;
	case M_XOR:   	$rd = I32($rs1) ^  I32($rs2);                     break;
	case M_OR:   	$rd = I32($rs1) |  I32($rs2);                     break;
	case M_AND:   	$rd = I32($rs1) &  I32($rs2);                     break;
	case M_ADDI:  	$rd = I32($rs1) +  I32(decode_imm(x));            break;
	case M_XORI:  	$rd = I32($rs1) ^  I32(decode_imm(x));            break;
	case M_ORI:  	$rd = I32($rs1) |  I32(decode_imm(x));            break;
	case M_ANDI:  	$rd = I32($rs1) &  I32(decode_imm(x));            break;
	case M_SLL:   	$rd = U32($rs1) << U32($rs2);                     break;
	case M_SRL:   	$rd = U32($rs1) >> U32($rs2);                     break;
	case M_SRA:   	$rd = rotr(I32($rs1),I32($rs2));                  break;
	case M_LUI:   	$rd = decode_imm(x);                              break;
    } 									       
    NEXT_INSTRUCTION;
    return 0;
}

i32 LogicKernel(u32 op, u32 x) 
{                                                                              
    switch (op)                                                         
    {                                                                          
	case M_SLT:   	$rd = (I32($rs1) < I32($rs2))          ? 1 : 0;	  break;
	case M_SLTU:  	$rd = (U32($rs1) < U32(decode_imm(x))) ? 1 : 0;   break;
	case M_SLTI:   	$rd = (I32($rs1) < I32($rs2)) 	       ? 1 : 0;   break;
	case M_SLTIU:   $rd = (U32($rs1) < U32($rs2)) 	       ? 1 : 0;   break;
    } 									       
    NEXT_INSTRUCTION;
    return 0;
}


i32 MemoryKernel(u32 op, u32 x) 
{                                                                              
    u32 	v = 	offset(x); 
    void* 	m = 	(void*)&VM.M[v];

    if (v >= HEAP_OFFSET && v <= MEMORY_LIMIT && !is_allocated(v)) illegal_operation();

    switch (op)                                                                
    {                                                                          
	case M_LB:  	VRkernel(v); $rd = signext(*_I08(m), 7);   	  break;
	case M_LH:  	VRkernel(v); $rd = signext(*_I16(m), 15);  	  break;
	case M_LW:  	VRkernel(v); $rd = *_I32(m);			  break;
	case M_SB:   	memcpy(m, &$rs2, sizeof(u08)); VRkernel(v);       break;
	case M_SH:   	memcpy(m, &$rs2, sizeof(u16)); VRkernel(v);	  break;
	case M_SW:   	memcpy(m, &$rs2, sizeof(u32)); VRkernel(v);	  break;
	case M_LBU:  	$rd = I08(VM.M[v]);				  break;
	case M_LHU:  	$rd = I16(VM.M[v]);				  break;
    }
    NEXT_INSTRUCTION;
    return 0;
}

i32 BranchKernel(u32 op, u32 x) 
{                                                                              
    u32 prev = VM.PC;
    switch (op)                                                                
    {                                                                          
	case M_BEQ:   	if (I32($rs1) == I32($rs2)) VM.PC+=decode_imm(x); break;
	case M_BNE:   	if (I32($rs1) != I32($rs2)) VM.PC+=decode_imm(x); break;
	case M_BLT:   	if (I32($rs1) <  I32($rs2)) VM.PC+=decode_imm(x); break;
	case M_BLTU:   	if (U32($rs1) <  U32($rs2)) VM.PC+=decode_imm(x); break;
	case M_BGE:   	if (I32($rs1) >= I32($rs2)) VM.PC+=decode_imm(x); break;
	case M_BGEU:   	if (U32($rs1) >= U32($rs2)) VM.PC+=decode_imm(x); break;
    } 									       
    if (prev == VM.PC) NEXT_INSTRUCTION;
    return 0;
}

i32 CtrlFlowKernel(u32 op, u32 x) 
{                                                                              
    switch (op)                                                                
    {                                                                          
	case M_BEQ:   	BranchKernel(M_BEQ,  x);                          break;
	case M_BNE:   	BranchKernel(M_BNE,  x);                          break;
	case M_BLT:   	BranchKernel(M_BLT,  x);                          break;
	case M_BLTU:   	BranchKernel(M_BLTU, x);                          break;
	case M_BGE:   	BranchKernel(M_BGE,  x);                          break;
	case M_BGEU:   	BranchKernel(M_BGEU, x);                          break;
	case M_JAL:   	$rd = VM.PC + 4; VM.PC += decode_imm(x);	  break;
	case M_JALR:   	$rd = VM.PC + 4; VM.PC = $rs1 + decode_imm(x); 	  break;
    } 									       
    return 0;
}

i32 execute(u32 x) 
{                                                                              
    switch (opmask(x))                                                         
    {                                                                          
	case M_ADD:   	ALUKernel	(M_ADD,   	x);		  break;
	case M_SUB:   	ALUKernel	(M_SUB,   	x);		  break;
	case M_XOR:   	ALUKernel	(M_XOR,   	x);		  break;
	case M_OR:   	ALUKernel	(M_OR,    	x);		  break;
	case M_AND:   	ALUKernel	(M_AND,   	x);		  break;
	case M_ADDI:   	ALUKernel	(M_ADDI,  	x);		  break;
	case M_XORI:   	ALUKernel	(M_XORI,  	x);		  break;
	case M_ORI:   	ALUKernel	(M_ORI,   	x);		  break;
	case M_ANDI:   	ALUKernel	(M_ANDI,  	x);		  break;
	case M_SLL:   	ALUKernel	(M_SLL,   	x);		  break;
	case M_SRL:   	ALUKernel	(M_SRL,   	x);		  break;
	case M_SRA:   	ALUKernel	(M_SRA,   	x);		  break;
	case M_LUI:   	ALUKernel	(M_LUI,   	x);		  break;
	case M_SLT:   	LogicKernel	(M_SLT,   	x);		  break;
	case M_SLTU:   	LogicKernel	(M_SLTU,  	x);		  break;
	case M_SLTI:   	LogicKernel	(M_SLTI,  	x);		  break;
	case M_SLTIU:   LogicKernel	(M_SLTIU, 	x);		  break;
	case M_SB:   	MemoryKernel	(M_SB,    	x);		  break;
	case M_SH:   	MemoryKernel	(M_SH,    	x);		  break;
	case M_SW:   	MemoryKernel	(M_SW,    	x);		  break;
	case M_LB:   	MemoryKernel	(M_LB,    	x);		  break;
	case M_LH:   	MemoryKernel	(M_LH,    	x);		  break;
	case M_LW:   	MemoryKernel	(M_LW,    	x); 		  break;
	case M_LBU:   	MemoryKernel	(M_LBU,   	x); 		  break;
	case M_LHU:   	MemoryKernel	(M_LHU,   	x); 		  break;
	case M_BEQ:   	CtrlFlowKernel	(M_BEQ,   	x); 		  break;
	case M_BNE:   	CtrlFlowKernel	(M_BNE,   	x); 		  break;
	case M_BLT:   	CtrlFlowKernel	(M_BLT,   	x); 		  break;
	case M_BLTU:   	CtrlFlowKernel	(M_BLTU,  	x); 		  break;
	case M_BGE:   	CtrlFlowKernel	(M_BGE,   	x); 		  break;
	case M_BGEU:   	CtrlFlowKernel	(M_BGEU,  	x); 		  break;
	case M_JAL:   	CtrlFlowKernel	(M_JAL,   	x); 		  break;
	case M_JALR:   	CtrlFlowKernel	(M_JALR,  	x); 		  break;
	default:        invalid		(               x);  		  break;
    }                                                      		       
    return 0;
}


#endif
