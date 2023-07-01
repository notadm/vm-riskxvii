#ifndef CTRLFLOW_H 
#define CTRLFLOW_H

// CONTROL FLOW

typedef unsigned int   		instruction;
typedef unsigned int   		op;

// RISK-XVII INSTRUCTION TYPES
#define M_OPCODE    		0x0000007F
#define M_R_TYPE    		0xFE00707F
#define M_I_TYPE    		0x0000707F
#define M_S_TYPE    		0x0000707F
#define M_SB_TYPE   		0x0000707F
#define M_U_TYPE    		0x0000007F
#define M_UJ_TYPE   		0x0000007F

#define R_TYPE      		0x00000033
#define I_TYPE      		0x00000013
#define S_TYPE      		0x00000023
#define SB_TYPE     		0x00000063
#define U_TYPE      		0x00000037
#define UJ_TYPE     		0x0000006F
#define MEM_TYPE            0x00000003
#define RET_TYPE 			0x00000067

// R TYPE
#define M_ADD       		0x00000033 
#define M_SUB       		0x40000033 
#define M_XOR       		0x00004033
#define M_OR        		0x00006033
#define M_AND       		0x00007033
#define M_ANDI      		0x00007013
#define M_SLL       		0x00001033
#define M_SRL       		0x00005033
#define M_SRA       		0x40005033
#define M_SLT       		0x00002033
#define M_SLTI      		0x00002013
#define M_SLTU      		0x00003033
#define M_SLTIU     		0x00003013

// I TYPE
#define M_ADDI      		0x00000013
#define M_XORI      		0x00004013
#define M_ORI       		0x00006013
#define M_LB        		0x00000003
#define M_LH        		0x00001003
#define M_LW        		0x00002003
#define M_LBU       		0x00004003
#define M_LHU       		0x00005003
#define M_SB        		0x00000023
#define M_SH        		0x00001023
#define M_SW        		0x00002023
#define M_JALR      		0x00000067

// SB TYPE
#define M_BEQ       		0x00000063
#define M_BNE       		0x00001063
#define M_BLT       		0x00004063
#define M_BLTU      		0x00006063
#define M_BGE       		0x00005063
#define M_BGEU      		0x00007063

// U TYPE
#define M_LUI       		0x00000037

// UJ TYPE
#define M_JAL       		0x0000006F

op typemask(instruction x)                       
{                                              
    switch ( M_OPCODE & x )                    
    {                                          
		case R_TYPE:   return R_TYPE; 
		case S_TYPE:   return S_TYPE;  
		case I_TYPE:   return I_TYPE;  
		case U_TYPE:   return U_TYPE;
		case SB_TYPE:  return SB_TYPE;
		case UJ_TYPE:  return UJ_TYPE;
		case MEM_TYPE: return I_TYPE;
		case RET_TYPE: return I_TYPE;
    }
    return 0;
}


op rmask(instruction x)                       
{                                                
    switch ( M_R_TYPE & x )                      
    {                                            
		case M_ADD:   return M_ADD; 
		case M_SUB:   return M_SUB;
		case M_XOR:   return M_XOR;
		case M_OR:    return M_OR;
		case M_AND:   return M_AND;
		case M_SLL:   return M_SLL;
		case M_SRL:   return M_SRL;
		case M_SRA:   return M_SRA;	
		case M_SLT:   return M_SLT;
		case M_SLTU:  return M_SLTU;
    } 
    return 0;
}

op imask(instruction x)                       
{                                            
    switch ( M_I_TYPE & x )                      
    {                                            
		case M_ADDI:  return M_ADDI;
		case M_XORI:  return M_XORI;
		case M_ORI:   return M_ORI;
		case M_ANDI:  return M_ANDI;
		case M_LB:    return M_LB;
		case M_LH:    return M_LH;
		case M_LW:    return M_LW;
		case M_LBU:   return M_LBU;
		case M_LHU:   return M_LHU;
		case M_SLTI:  return M_SLTI;
		case M_SLTIU: return M_SLTIU;
		case M_JALR:  return M_JALR;
    }                                          
    return 0;
}

op smask(instruction x)                       
{                                                
    switch ( M_S_TYPE & x )                      
    {                                            
		case M_LUI:   return M_LUI;
		case M_SB:    return M_SB;
		case M_SH:    return M_SH;
		case M_SW:    return M_SW;
    }                                           
    return 0;
}

op umask(instruction x)                       
{                                                
    switch ( M_U_TYPE & x )                      
    {                                            
		case M_LUI:   return M_LUI;
    }                                        
    return 0;
}

op sbmask(instruction x)                       
{                                                
    switch ( M_SB_TYPE & x )                     
    {                                            
		case M_BEQ:   return M_BEQ;	
		case M_BNE:   return M_BNE; 
		case M_BLT:   return M_BLT;	
		case M_BLTU:  return M_BLTU;
		case M_BGE:   return M_BGE;
		case M_BGEU:  return M_BGEU;
    }                                           
    return 0;
}

op ujmask(instruction x)                       
{                                                
    switch ( M_UJ_TYPE & x )                     
    {                                            
		case M_JAL:   return M_JAL;	
    }                                            
    return 0;
}

op opmask(instruction x)                       
{                                              
    switch ( typemask(x) )                    
    {                                          
		case R_TYPE:  return rmask(x); 
		case S_TYPE:  return smask(x);  
		case I_TYPE:  return imask(x);  
		case U_TYPE:  return umask(x);
		case SB_TYPE: return sbmask(x);
		case UJ_TYPE: return ujmask(x);
    }
    return 0;
}

#endif
