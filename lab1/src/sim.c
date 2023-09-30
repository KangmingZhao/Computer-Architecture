#include<stdio.h>
#include"shell.h"
enum{
 OP_J = 2,
 OP_JAL = 3,
 OP_BEQ = 4,
 OP_BNE = 5,
 OP_BLEZ = 6,
 OP_BGTZ = 7,
 OP_ADDI = 8,
 OP_ADDIU = 9,
 OP_SLTI = 10,
 OP_SLTIU = 11,
 OP_ANDI = 12,
 OP_ORI = 13,
 OP_XORI = 14,
 OP_LUI = 15,
 OP_LB = 32,
 OP_LH = 33,
 OP_LW = 35,
 OP_LBU = 36,
 OP_LHU = 37,
 OP_SB = 40,
 OP_SH = 41,
 OP_SW = 43,
 OP_REGIMM = 1,
 OP_BLTZ = 0,
 OP_BGEZ = 1,
 OP_BLTZAL = 16,
 OP_BGEZAL = 17,
 OP_SPECIAL = 0,
 OP_SLL = 0,
 OP_SRL = 2,
 OP_SRA = 3,
 OP_SLLV = 4,
 OP_SRLV = 6,
 OP_SRAV = 7,
 OP_JR = 8,
 OP_JALR = 9,
 OP_ADD = 32,
 OP_ADDU = 33,
 OP_SUB = 34,
 OP_SUBU = 35,
 OP_AND = 36,
 OP_OR = 37,
 OP_XOR = 38,
 OP_NOR = 39,
 OP_SLT = 42,
 OP_SLTU = 43,
 OP_MULT = 24,
 OP_MFHI = 16,
 OP_MFLO = 18,
 OP_MTHI = 17,
 OP_MTLO = 19,
 OP_MULTU = 25,
 OP_DIV = 26,
 OP_DIVU = 27,
 OP_SYSCALL = 12,
	
}opcode;
//一堆变量的声明
uint32_t machine_inst;/*指令的机器码*/
uint32_t op;
//R型指令
uint32_t rs,rt,rd,shamt,funct;
//J型指令
 uint32_t jump_target, branch_target, virt_addr;
//I型指令
uint32_t imm;
uint32_t offset;

int32_t temp32;
int64_t temp64;
uint32_t tempu32;
uint64_t tempu64;
uint32_t base;
uint32_t sa;
uint32_t special_type;
uint32_t instr_regimm;
uint32_t s;
uint32_t flag;
//取指
void IF()
{
    machine_inst=mem_read_32(CURRENT_STATE.PC);
}
//译码
void ID()
{
    //获取各个位并且将最高位清零
    op=(machine_inst>>26) & 0x3F;
    rs=(machine_inst>>21) & 0x1F;
    rt=(machine_inst>>16) & 0x1F;
    rd=(machine_inst>>11) & 0x1F;
    shamt=(machine_inst>>6) & 0x1F;
    funct=(machine_inst>>0) & 0x3F;

    jump_target=machine_inst&0x03FFFFFF;
    imm = (machine_inst >> 0) & 0x0000FFFF;
    offset =(machine_inst >> 0) & 0xFFFF;
    base=(machine_inst&0x03E00000)>>21;
}
void execute()
{
    switch (op)
    {
    //1
    case OP_J:
      
        NEXT_STATE.PC=(CURRENT_STATE.PC&0xF0000000)|(jump_target<<2);
        break;
    //2
    case OP_JAL:
        NEXT_STATE.PC=(CURRENT_STATE.PC&0xF0000000)|(jump_target<<2);
        NEXT_STATE.REGS[31]=CURRENT_STATE.PC+8;
        break;
    //3
    case OP_BEQ:
        branch_target=((offset<<16)>>14);
        if(CURRENT_STATE.REGS[rs]==CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
        }
        else{
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
        }
        break;
    //4 
    case OP_BNE:

    branch_target=((offset<<16)>>14);
    if(CURRENT_STATE.REGS[rs]!=CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
        }
        else{
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
        }
        break;
    //5
     case OP_BLEZ:
	     branch_target=((offset<<16)>>14);
		 if((CURRENT_STATE.REGS[rs]&0x80000000)==0 && CURRENT_STATE.REGS[rs]!=0)
		 {
			 NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
		 }
		 else
		 {
			 NEXT_STATE.PC=CURRENT_STATE.PC+4;
		 }
         break;
    //6
     case OP_BGTZ:
          branch_target=((offset<<16)>>14);
         if((CURRENT_STATE.REGS[rs]&0x80000000)==0 && CURRENT_STATE.REGS[rs]!=0)
		 {
			 NEXT_STATE.PC=CURRENT_STATE.PC+branch_target;
		 }
		 else
		 {
			 NEXT_STATE.PC=CURRENT_STATE.PC+4;
		 }
		 break;
    //7
    case OP_ADDI:
    imm=((imm<<16)>>16);
    NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]+imm;
    NEXT_STATE.PC=NEXT_STATE.PC+4;
    break;
    //8
    case OP_ADDIU:
    imm=((imm<<16)>>16);
    NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]+imm;
    NEXT_STATE.PC=NEXT_STATE.PC+4;
    break;
    //9
    case OP_SLTI:
    imm=((imm<<16)>>16);
    if(((int32_t)CURRENT_STATE.REGS[rs]<(int32_t)imm))
    {
        NEXT_STATE.REGS[rt]=1;
    }
    else{
         NEXT_STATE.REGS[rt]=0;
    }
    NEXT_STATE.PC=CURRENT_STATE.PC+4;
    break;
    //10
    case OP_SLTIU:
    imm=(imm<<16)>>16;
    if(CURRENT_STATE.REGS[rs]<(int32_t)imm)
    {
        NEXT_STATE.REGS[rt]=1;
    
    }
     else{
         NEXT_STATE.REGS[rt]=0;
    }
    NEXT_STATE.PC=CURRENT_STATE.PC+4;
    break;
    //11
    case OP_ANDI:
    NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]&(imm&0x0000FFFF);
    NEXT_STATE.PC=CURRENT_STATE.PC+4;
    //12
    case OP_ORI:
    NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]|(imm&0x0000FFFF);
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
    break;
    //13
    case OP_XORI:
	NEXT_STATE.REGS[rt]=(CURRENT_STATE.REGS[rs])^(imm&0x0000FFFF);
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
	break;
    //14
    case OP_LUI:
    NEXT_STATE.REGS[rt]=(imm<<16);
    NEXT_STATE.PC=CURRENT_STATE.PC+4;
    //15
    case OP_LB:
    virt_addr=CURRENT_STATE.REGS[base]+((offset<<16)>>16);
	NEXT_STATE.REGS[rt]=(((int8_t)mem_read_32(virt_addr))<<24)>>24;
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
	break;
    //16
    case OP_LH:
	virt_addr=CURRENT_STATE.REGS[base]+((offset<<16)>>16);
    NEXT_STATE.REGS[rt]=(((int16_t)mem_read_32(virt_addr))<<16)>>16;
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
	break;
    //17 
	case OP_LW:
	virt_addr=CURRENT_STATE.REGS[base]+((offset<<16)>>16);
	NEXT_STATE.REGS[rt]=mem_read_32(virt_addr);
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
	break;
    //18
    case OP_LBU:
    virt_addr=CURRENT_STATE.REGS[base]+((offset<<16)>>16);
	NEXT_STATE.REGS[rt]=(mem_read_32(virt_addr)&0x000000FF);
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
    break;
    //19
    case OP_LHU:
    virt_addr=CURRENT_STATE.REGS[base]+((offset<<16)>>16);
	NEXT_STATE.REGS[rt]=((mem_read_32(virt_addr)&0x0000FFFF));
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
	break;
    //20
    case OP_SB:
    virt_addr=CURRENT_STATE.REGS[base]+((offset<<16)>>16);
	mem_write_32(virt_addr,(uint8_t)CURRENT_STATE.REGS[rt]);
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
    break;
    //21
    case OP_SH:
    virt_addr=CURRENT_STATE.REGS[base]+((offset<<16)>>16);
	mem_write_32(virt_addr,(uint16_t)CURRENT_STATE.REGS[rt]);
	NEXT_STATE.PC=CURRENT_STATE.PC+4;   
    break;
    //22
    case OP_SW:
    virt_addr=CURRENT_STATE.REGS[base]+((offset<<16)>>16);
	mem_write_32(virt_addr, CURRENT_STATE.REGS[rt]);
	NEXT_STATE.PC=CURRENT_STATE.PC+4;
    break;
    //23
    case OP_REGIMM:
    instr_regimm=(machine_inst&0x001F0000)>>16;
    branch_target=CURRENT_STATE.PC+((offset<<16)>>14);
    switch (instr_regimm)
    {
        //24
        case OP_BLTZ:
			NEXT_STATE.PC=((CURRENT_STATE.REGS[rs]&0x80000000)==0x80000000)?branch_target:CURRENT_STATE.PC+4;
			break;
		 //25
		 case OP_BGEZ:
			NEXT_STATE.PC=((CURRENT_STATE.REGS[rs]&0x80000000)==0x00000000)?branch_target:CURRENT_STATE.PC+4;
			break;
		//26	
		 case OP_BLTZAL:
			NEXT_STATE.PC=((CURRENT_STATE.REGS[rs]&0x80000000)==0x80000000)?branch_target:CURRENT_STATE.PC+4;
			NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;
			break;
		//27	
		 case OP_BGEZAL:
			NEXT_STATE.PC=((CURRENT_STATE.REGS[rs]&0x80000000)==0x00000000)?branch_target:CURRENT_STATE.PC+4;
			NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;
			break;
    
    default:
        break;
    }
    //special类型
    case  OP_SPECIAL:
    special_type=machine_inst&0x0000003F;
    switch (special_type)
    {
    case OP_SLL:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]<<sa;//左移
        break;
    case OP_SRL:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>sa;
    case OP_SRA:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>sa;
    case OP_SLLV:
    s=CURRENT_STATE.REGS[rs]& 0x0000001F;
    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]<<s;
    case OP_SRLV:
    s=CURRENT_STATE.REGS[rs]& 0x0000001F;
    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>s;
    break;
    
    case OP_SRAV:
    s=CURRENT_STATE.REGS[rs]& 0x0000001F;
    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>s;
    break;

    case OP_JR:
        NEXT_STATE.PC=CURRENT_STATE.REGS[rs];
        break;
    case OP_JALR:
        NEXT_STATE.PC=CURRENT_STATE.REGS[rs];
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]+4;
        break;
    case OP_ADD:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]+CURRENT_STATE.REGS[rs];
        break;

    case OP_ADDU:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]+CURRENT_STATE.REGS[rs];
    	break;
	case OP_SUB:
		NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
		break;
	case OP_SUBU:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
		break;
    case OP_AND:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]&CURRENT_STATE.REGS[rt];
		break;
    case OP_OR:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt];
        break;
    case OP_XOR:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]^CURRENT_STATE.REGS[rt];
        break;
    case OP_NOR:
        NEXT_STATE.REGS[rd]=~(CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt]);
    case OP_SLT:
        temp32=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
        if(temp32<0)
        {
            NEXT_STATE.REGS[rd]=1;
        }
        else{
            NEXT_STATE.REGS[rd]=0;
        }
        break;
    case OP_SLTU:
     tempu32=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
        if(tempu32<0)
        {
            NEXT_STATE.REGS[rd]=1;
        }
        else{
            NEXT_STATE.REGS[rd]=0;
        }
        break;
    case OP_MULT:
    temp64=(int64_t)CURRENT_STATE.REGS[rs]*(int64_t)CURRENT_STATE.REGS[rt];
    NEXT_STATE.HI=temp64&0xFFFFFFFF00000000;
    NEXT_STATE.LO=temp64&0x00000000FFFFFFFF;
    break;
    case OP_MFHI:
			NEXT_STATE.REGS[rd]=CURRENT_STATE.HI;
			break;
			 
	case OP_MFLO:
			NEXT_STATE.REGS[rd]=CURRENT_STATE.LO;
			break;
			
	case OP_MTHI:
		    NEXT_STATE.HI=CURRENT_STATE.REGS[rs];
			break;
			
	case OP_MTLO:
		    NEXT_STATE.LO=CURRENT_STATE.REGS[rs];
			break;
    case OP_MULTU:
    tempu64=(uint64_t)CURRENT_STATE.REGS[rs]*(uint64_t)CURRENT_STATE.REGS[rt];
    NEXT_STATE.HI=tempu64&0xFFFFFFFF00000000;
    NEXT_STATE.LO=tempu64&0x00000000FFFFFFFF;
    break;
    case OP_DIV:
    NEXT_STATE.HI=((int32_t)CURRENT_STATE.REGS[rs]%(int32_t)CURRENT_STATE.REGS[rt]);
	NEXT_STATE.LO=((int32_t)CURRENT_STATE.REGS[rs]/(int32_t)CURRENT_STATE.REGS[rt]);
	break;
	case OP_DIVU:
    NEXT_STATE.HI=(CURRENT_STATE.REGS[rs]%CURRENT_STATE.REGS[rt]);
	NEXT_STATE.LO=(CURRENT_STATE.REGS[rs]/CURRENT_STATE.REGS[rt]);
	break;
	case OP_SYSCALL:
    if(CURRENT_STATE.REGS[2]==0x0A)
    {
        RUN_BIT=FALSE;
    }	
    flag=1;
    
    break;

    default:
        break;
    }
    if(flag==0)
    {
         NEXT_STATE.PC=CURRENT_STATE.PC+4;
    }
    
    default:
        break;
    }
}
void process_instruction()
{
    IF();
    ID();
    execute();
}