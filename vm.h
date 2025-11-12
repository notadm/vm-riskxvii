#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "ctrlflow.h"

// TYPES
typedef char*          	        str;
typedef char           	        i08;
typedef short          	        i16;
typedef int            	        i32;
typedef long           	        i64;
typedef unsigned char  	        u08;
typedef unsigned short 	        u16;
typedef unsigned int   	        u32;
typedef unsigned long  	        u64;
typedef __int128_t    	        i128;
typedef __uint128_t    	        u128;

#define I08(x) 		        	( (i08) x )
#define U08(x) 		        	( (u08) x )
#define I16(x) 		        	( (i16) x )
#define U16(x) 		        	( (u16) x )
#define I32(x) 		        	( (i32) x )
#define U32(x) 		        	( (u32) x )

#define _I08(x) 	        	( (i08*) x )
#define _U08(x) 	        	( (u08*) x )
#define _I16(x) 	        	( (i16*) x )
#define _U16(x) 	        	( (u16*) x )
#define _I32(x) 	        	( (i32*) x )
#define _U32(x) 	        	( (u32*) x )

#define PROGRAM_ARGS            2
#define HEAP_BITMAP_DEPTH       4
#define TOTAL_VIRTUAL_ROUTINES  11
#define REGISTER_LIMIT        	32
#define TOTAL_INSTRUCTIONS      1024
#define HEAP_OFFSET             0xB700
#define MEMORY_LIMIT            0xD700
#define VIRTUAL_ROUTINE_OFFSET  0x0800
#define BINARY_IMAGE            argv[1]
#define TERMINATE               exit(0)
#define INSTRUCTION             (*_U32(&VM.M[VM.PC]))
#define NEXT_INSTRUCTION        VM.PC += 4

#define ZERO                  	0
#define RA                  	1
#define SP                  	2
#define FP                  	3
#define X0                  	0
#define X1                  	1
#define X2                    	2
#define X3                   	3
#define X4                   	4
#define X5                   	5
#define A0                  	10
#define A1                   	11
#define A2                   	12
#define A3                  	13
#define A4                  	14
#define A5                  	15

#define $zero             		VM.R[ZERO]
#define $ra             		VM.R[RA]
#define $sp             		VM.R[SP]
#define $fp             		VM.R[FP]
#define $x0           			VM.R[X0]
#define $x1             		VM.R[X1]
#define $x2             		VM.R[X2]
#define $x3             		VM.R[X3]
#define $x4             		VM.R[X4]
#define $x5             		VM.R[X5]
#define $a0           			VM.R[A0]
#define $a1             		VM.R[A1]
#define $a2             		VM.R[A2]
#define $a3             		VM.R[A3]
#define $a4             		VM.R[A4]
#define $a5             		VM.R[A5]

#define $rd               		VM.R[rd(x) ]
#define $rs1               		VM.R[rs1(x)]
#define $rs2					VM.R[rs2(x)]
#define $rmalloc				VM.R[28]

#define RD_MASK				0x00000F80 
#define RS1_MASK			0x000F8000 
#define RS2_MASK			0x01FF8000 

// BIT ALIGNMENTS / WIDTHS

#define _21_20_WIDTH    	1
#define _08_07_WIDTH    	1
#define _31_30_WIDTH    	1
#define _14_12_WIDTH    	3
#define _11_08_WIDTH    	4
#define _24_20_WIDTH    	5
#define _11_07_WIDTH    	5
#define _19_15_WIDTH    	5
#define _30_25_WIDTH    	6
#define _06_00_WIDTH    	7
#define _31_25_WIDTH    	7
#define _31_25_WIDTH    	7
#define _19_12_WIDTH    	8
#define _30_21_WIDTH    	10
#define _31_20_WIDTH    	12
#define _31_12_WIDTH    	20

#define _06_00_OFFSET   	0
#define _08_07_OFFSET   	7 
#define _11_07_OFFSET   	7
#define _11_08_OFFSET   	8
#define _31_12_OFFSET   	12
#define _19_12_OFFSET   	12
#define _14_12_OFFSET   	12
#define _19_15_OFFSET   	15
#define _24_20_OFFSET   	20
#define _21_20_OFFSET   	20
#define _31_20_OFFSET   	20
#define _30_21_OFFSET   	21
#define _31_25_OFFSET   	25
#define _30_25_OFFSET   	25
#define _31_30_OFFSET   	31

#define VR_WR_CHAR	     	0x0800
#define VR_WR_INT	      	0x0804
#define VR_WR_UINT     		0x0808
#define VR_HALT     		0x080C
#define VR_RD_CHAR     		0x0812
#define VR_RD_INT     		0x0816
#define VR_DUMP_PC    		0x0820
#define VR_DUMP_REG   		0x0824
#define VR_DUMP_WORD  		0x0828
#define VR_MALLOC     		0x0830
#define VR_FREE       		0x0834

// IMMEDIATE NUMBER DECODER OPS
#define decode_imm_i(x)    mpbits( 0,                                          \
			   x, 0, _31_20_OFFSET,  _31_20_WIDTH )
#define decode_imm_u(x)    mpbits( 0,                                          \
			   x, _31_12_OFFSET, _31_12_OFFSET,  _31_12_WIDTH )
#define decode_imm_s(x)    mpbits( mpbits( 0,                                  \
			   x, 0, _11_07_OFFSET, _11_07_WIDTH ),                \
	    	   x, 5, _31_25_OFFSET, _31_25_WIDTH )            
#define decode_imm_sb(x)   mpbits( mpbits( mpbits( mpbits(0,                   \
			   x, 11, _08_07_OFFSET, _08_07_WIDTH ),               \
			   x, 1,  _11_08_OFFSET,  _11_08_WIDTH ),              \
			   x, 12, _31_30_OFFSET, _31_30_WIDTH ),               \
			   x, 5,  _30_25_OFFSET,  _30_25_WIDTH )
#define decode_imm_uj(x)   mpbits( mpbits( mpbits( mpbits(0,                   \
			   x, 12, _19_12_OFFSET, _19_12_WIDTH ),               \
			   x, 11, _21_20_OFFSET, _21_20_WIDTH ),               \
			   x, 1,  _30_21_OFFSET, _30_21_WIDTH ),               \
			   x, 20, _31_30_OFFSET, _31_30_WIDTH )


// BIT OPS
u32 signext(u32 x,u32 n)                         { return x|((x&(1<<n))?~((1<<n)-1):0);}
u32 nbitmask(u32 i,u32 n)                        { return (0xFFFFFFFF>>(32-n)<<i);     }
u32 rdbits(u32 x,u32 i,u32 n)                    { return (x>>i)&(0xFFFFFFFF>>(32-n)); }
u32 rmbits(u32 x,u32 i,u32 n)                    { return x&(0xFFFFFFFF^nbitmask(i,n));}
u32 wrbits(u32 x,u32 y,u32 i,u32 n)              { return rmbits(x,i,n)|(y<<i);        }
u32 mpbits(u32 x,u32 y,u32 i,u32 j,u32 n)        { return wrbits(x,rdbits(y,j,n),i,n); }
u32 rotr(u32 x,u32 n)                            { return (x>>n%32)|(x<<(32-n)%32);    }

u32 decode_imm(u32 x)
{ 									       
    switch(typemask(x))   	                                               
    {              							       
        case U_TYPE: 	return decode_imm_u(x);	
        case I_TYPE: 	return signext( decode_imm_i(x) , 11);
        case S_TYPE: 	return signext( decode_imm_s(x) , 11);
        case SB_TYPE: 	return signext( decode_imm_sb(x), 12);		
        case UJ_TYPE: 	return signext( decode_imm_uj(x), 20); 
    }		                                               
    return 0;
}

typedef struct bitmap 
{
    u64 	upper;
    u64 	lower;
}
bitmap;

typedef struct State 
{
    u32         PC;  		
    u32         R[REGISTER_LIMIT];
    i08         M[MEMORY_LIMIT];
    bitmap      allocation;
    bitmap      allocsize[HEAP_BITMAP_DEPTH];
}
State;

State VM;
		
u32 rd(u32 x)           { return ( (x & RD_MASK  ) >> _11_07_OFFSET );                  }
u32 rs1(u32 x)          { return ( (x & RS1_MASK ) >> _19_15_OFFSET );                  }
u32 rs2(u32 x)          { return ( (x & RS2_MASK ) >> _24_20_OFFSET );                  }
i32 clr(u32 x)          { *_I32(&VM.M[x]) = 0; return 0;                                }
i32 offset(u32 x)       { return (I32($rs1)+decode_imm(x));                             }
i32 dumpPC()            { printf("PC = 0x%08x;\n", VM.PC); return 0;                    }
i32 dumpRegister(u32 i) { printf("R[%d] = 0x%08x;\n",i, VM.R[i]); return 0;             }

i32 dumpRegisterBanks()
{ 
    dumpPC(); 
    for (int i = 0; i < REGISTER_LIMIT; i++) { dumpRegister(i); }
    return 0;
}

i32 halt()
{ 
    printf("CPU Halt Requested\n");
    TERMINATE;
    return 0;
}

i32 invalid(u32 x)
{
    printf("Instruction Not Implemented: 0x%x\n", x); 
    dumpRegisterBanks(); 
    TERMINATE;                                	
    return 0;
}

i32 illegal_operation()
{ 
    printf("Illegal Operation: 0x%08x\n", INSTRUCTION); 
    dumpRegisterBanks();
    TERMINATE;
    return 0;
}


#endif
