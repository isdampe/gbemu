#include <iostream>
#include "cpu.h"

using namespace std;

processor cpu_create(string rom_fp)
{
	processor cpu;

	//Setup registers.
	cpu.r.A = 0x0;
	cpu.r.F = 0x0;
	cpu.r.B = 0x0;
	cpu.r.C = 0x0;
	cpu.r.D = 0x0;
	cpu.r.E = 0x0;
	cpu.r.H = 0x0;
	cpu.r.L = 0x0;

	cpu.r.SP = 0x0;
	cpu.r.PC = 0x0;

	//Setup memory.
	cpu.mmu = mmu_create();

	//Write the ROM to the memory.
	for (int i=0x0; i<0x100; i++)
		mmu_absolute_write(cpu.mmu, (uint16_t)i, cpu.bios_rom[i]);

	return cpu;
}

void cpu_main(processor &cpu)
{
	//Main cpu loop.
	instruction last_inst;

	while (1)
	{
		//Whatever is in the program counter.
		cpu_execute_inst(
			cpu,
			mmu_absolute_read(cpu.mmu, cpu.r.PC)
		);
	}

	cout << cpu.r.PC;

}

instruction cpu_execute_inst(processor &cpu, const uint8_t &i)
{
	instruction inst;
	inst.inst = i;

	cout << "0x" << hex << (int)cpu.r.PC << ": ";

	switch (i)
	{
		case 0x31:
			inst.disassembly = "LD SP,d16";
			cpu_ld_sp_d16(cpu);
			break;
		case 0x32:
			inst.disassembly = "LD (HL-),A";
			cpu_ld_hldec_a(cpu);
			break;
		case 0xAF:
			inst.disassembly = "XOR A";
			cpu_xor_a(cpu);
			break;
		case 0x21:
			inst.disassembly = "LD HL,d16";
			cpu_ld_hl_d16(cpu);
			break;
		case 0xCB:
			inst.disassembly = "PREFIX CB";
			cpu_prefix_cb(cpu);
			break;
		case 0x20:
			inst.disassembly = "JPNZ,r8";
			cpu_jpnz_r8(cpu);
			break;
		break;
		case 0x0E:
			inst.disassembly = "LD C,d8";
			cpu_ld_c_d8(cpu);
			break;
		break;
		case 0x3E:
			inst.disassembly = "LD A,d8";
			cpu_ld_a_d8(cpu);
			break;
		break;
		case 0xE2:
			inst.disassembly = "LD C,A";
			cpu_ld_c_a(cpu);
			break;
		case 0xC:
			inst.disassembly = "INC C";
			cpu_inc_c(cpu);
			break;
		case 0x77:
			inst.disassembly = "LD (HL),A";
			cpu_ld_hl_a(cpu);
			break;
		case 0xE0:
			inst.disassembly = "LDH (a8),A";
			cpu_ldh_a8_a(cpu);
			break;
		break;
		case 0x11:
			inst.disassembly = "LD DE,d16";
			cpu_ld_de_d16(cpu);
			break;
		break;
		case 0x1A:
			inst.disassembly = "LD A,(DE)";
			cpu_ld_a_de(cpu);
			break;
		break;
		case 0xCD:
			inst.disassembly = "CALL NZ,a16";
			cpu_call_nz_a16(cpu);
			break;
		break;
		case 0x4F:
			inst.disassembly = "LD C,A";
			cpu_ld_c_ra(cpu);
			break;
		case 0x06:
			inst.disassembly = "LD B,d8";
			cpu_ld_b_d8(cpu);
			break;
		break;
		case 0xC5:
			inst.disassembly = "PUSH BC";
			cpu_push_bc(cpu);
			break;
		break;
		default:
			inst.disassembly = "UNKNOWN";
			cout << "Hit unknown opcode 0x" << hex << (int)i;
			cout << ", program counter: 0x" << cpu.r.PC << endl;
			cpu_dump(cpu);
			exit(1);
	}

	cout << "0x" << hex << (int)i << ": " << inst.disassembly << endl;

	return inst;
}

void cpu_dump(const processor &cpu)
{
	cout << "\n---- BEGIN CPU STACK TRACE ----\n\n";
	cout << "Register A: 0x" << hex << (int)cpu.r.A << "\n";
	cout << "Register F: 0x" << hex << (int)cpu.r.F << "\n";
	cout << "Register B: 0x" << hex << (int)cpu.r.B << "\n";
	cout << "Register C: 0x" << hex << (int)cpu.r.C << "\n";
	cout << "Register D: 0x" << hex << (int)cpu.r.D << "\n";
	cout << "Register E: 0x" << hex << (int)cpu.r.E << "\n";
	cout << "Register H: 0x" << hex << (int)cpu.r.H << "\n";
	cout << "Register L: 0x" << hex << (int)cpu.r.L << "\n";

	cout << "Register AF: 0x" << hex << (int)cpu_reg_read_16b(cpu.r.A, cpu.r.F) << "\n";
	cout << "Register BC: 0x" << hex << (int)cpu_reg_read_16b(cpu.r.B, cpu.r.C) << "\n";
	cout << "Register DE: 0x" << hex << (int)cpu_reg_read_16b(cpu.r.D, cpu.r.E) << "\n";
	cout << "Register HL: 0x" << hex << (int)cpu_reg_read_16b(cpu.r.H, cpu.r.L) << "\n";

	cout << "Register SP: 0x" << hex << (int)cpu.r.SP << "\n";
	cout << "Register PC: 0x" << hex << (int)cpu.r.PC << "\n";

	cout << "\n---- END CPU STACK TRACE ----\n\n";
}

void cpu_stack_push(processor &cpu, const uint16_t value)
{
	cpu.r.SP -= 0x2;
	mmu_absolute_write(cpu.mmu, cpu.r.SP, value);
}

void cpu_reg_write_16b(uint8_t &reg_A, uint8_t &reg_B, const uint16_t value)
{
	uint8_t high = (value >> 8);
	uint8_t low = (value & 0xFF);
	reg_A = high; reg_B = low;
}

uint16_t cpu_reg_read_16b(const uint8_t &reg_A, const uint8_t &reg_B)
{
	return reg_B | (reg_A << 8);
}

void cpu_set_flag(processor &cpu, const uint8_t flag, const uint8_t on_off)
{
	if (on_off == 1)
		cpu.r.F |= flag;
	else
		cpu.r.F &= ~(flag);
}

uint8_t cpu_get_flag(processor &cpu, const uint8_t flag)
{
	return (cpu.r.F & flag) ? 1 : 0;
}

void cpu_ld_sp_d16(processor &cpu)
{
	uint16_t res = mmu_absolute_read_u16(cpu.mmu, cpu.r.PC + 0x1);
	cpu.r.SP = res;
	cpu.r.PC += 0x3;
}

void cpu_xor_a(processor &cpu)
{
	//Really, this is just setting r.A to zero...
	cpu.r.A = (cpu.r.A ^ cpu.r.A);
	cpu.r.PC += 0x1;
}

void cpu_ld_hl_d16(processor &cpu)
{
	uint16_t res = mmu_absolute_read_u16(cpu.mmu, cpu.r.PC + 0x1);
	cpu_reg_write_16b(cpu.r.H, cpu.r.L, res);
	cpu.r.PC += 0x3;
}

void cpu_ld_hldec_a(processor &cpu)
{
	uint16_t addr = cpu_reg_read_16b(cpu.r.H, cpu.r.L);
	mmu_absolute_write(cpu.mmu, addr, cpu.r.A);
	cpu_reg_write_16b(cpu.r.H, cpu.r.L, (addr -1));
	cpu.r.PC += 0x1;
}

void cpu_prefix_cb(processor &cpu)
{
	uint8_t cb = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	uint8_t hb, msb, fc;
	switch(cb)
	{
		case 0x7C:
			//Check most significant bit.
			hb = cpu_reg_read_16b(cpu.r.H, cpu.r.L) >> 8;
			msb = 1 << (8 -1);
			if (hb & msb)
				cpu.r.F = 0x1; //Set F register cleared.
			else
				cpu.r.F = 0x0; //Set F register active.
		break;
		case 0x11:
			fc = cpu.r.C > 0x7F;
			if (((cpu.r.C << 1) & 0xFF) | cpu_get_flag(cpu, FLAG_CARRY))
				cpu.r.C = 1;
			else
				cpu.r.C = 0;
			cpu_set_flag(cpu, FLAG_CARRY, fc);
			cpu_set_flag(cpu, FLAG_HALF_CARRY, 0);
			cpu_set_flag(cpu, FLAG_ZERO, (cpu.r.C == 0 ? 0 : 1));

		/**var newFCarry = (parentObj.registerC > 0x7F);
		parentObj.registerC = ((parentObj.registerC << 1) & 0xFF) | ((parentObj.FCarry) ? 1 : 0);
		parentObj.FCarry = newFCarry;
		parentObj.FHalfCarry = parentObj.FSubtract = false;
		parentObj.FZero = (parentObj.registerC == 0);*/
		break;
		default:
			cerr << "Hit unknown Prefix CB in cpu_prefix_cb, 0x" << hex << (int)cb << "\n";
			cpu_dump(cpu);
			exit(1);
		break;
	}

	cpu.r.PC += 0x2;
}

void cpu_jpnz_r8(processor &cpu)
{
	int8_t ra = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	if (cpu.r.F == 0x1)
		cpu.r.PC = (cpu.r.PC + 0x2) + ra;
	else
		cpu.r.PC += 0x2;
}

void cpu_ld_a_d8(processor &cpu)
{
	uint8_t val = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	cpu.r.A = val;
	cpu.r.PC += 0x2;
}

void cpu_ld_b_d8(processor &cpu)
{
	cpu.r.B = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	cpu.r.PC += 0x2;
}

void cpu_ld_c_d8(processor &cpu)
{
	cpu.r.C = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	cpu.r.PC += 0x2;
}

void cpu_ld_c_a(processor &cpu)
{
	mmu_absolute_write(cpu.mmu, (0xff00 + cpu.r.C), cpu.r.A);
	cpu.r.PC += 0x1;
}

void cpu_inc_c(processor &cpu)
{
	cpu.r.C += 0x1;
	cpu.r.PC += 0x1;
}

void cpu_ld_hl_a(processor &cpu)
{
	uint16_t addr = cpu_reg_read_16b(cpu.r.H, cpu.r.L);
	mmu_absolute_write(cpu.mmu, addr, cpu.r.A);
	cpu.r.PC += 0x1;
}

void cpu_ldh_a8_a(processor &cpu)
{
	uint8_t val = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	mmu_absolute_write(cpu.mmu, (0xFF00 + val), cpu.r.A);
	cpu.r.PC += 0x2;
}

void cpu_ld_de_d16(processor &cpu)
{
	uint16_t res = mmu_absolute_read_u16(cpu.mmu, cpu.r.PC + 0x1);
	cpu_reg_write_16b(cpu.r.D, cpu.r.E, res);
	cpu.r.PC += 0x3;
}

void cpu_ld_a_de(processor &cpu)
{
	uint16_t addr = cpu_reg_read_16b(cpu.r.D, cpu.r.E);
	cpu.r.A = mmu_absolute_read(cpu.mmu, addr);
	cpu.r.PC += 0x1;
}

void cpu_call_nz_a16(processor &cpu)
{
	uint16_t res = mmu_absolute_read_u16(cpu.mmu, cpu.r.PC + 0x1);
	cpu_stack_push(cpu, cpu.r.PC + 0x1);
	cpu.r.PC = res;
}

void cpu_ld_c_ra(processor &cpu)
{
	cpu.r.C = cpu.r.A;
	cpu.r.PC += 0x1;
}

void cpu_push_bc(processor &cpu)
{
	uint16_t res = cpu_reg_read_16b(cpu.r.B, cpu.r.C);
	cpu_stack_push(cpu, res);
	cpu.r.PC += 0x1;
}
