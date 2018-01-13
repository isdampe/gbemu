#include <iostream>
#include "cpu.h"

using namespace std;

processor cpu_create(string rom_fp)
{
	processor cpu;

	//Setup registers.
	cpu.r.A = 0x0;
	cpu.r.F = 0x0;
	cpu.r.SP = 0x0;
	cpu.r.PC = 0x0;
	cpu.r.HL = 0x0;

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
		default:
			inst.disassembly = "UNKNOWN";
			cout << "Hit unknown opcode 0x" << hex << (int)i;
			cout << ", program counter: 0x" << cpu.r.PC << endl;
			cpu_dump(cpu);
			exit(1);
	}

	cout << inst.disassembly << endl;

	return inst;
}

void cpu_dump(const processor &cpu)
{
	cout << "\n---- BEGIN CPU STACK TRACE ----\n\n";
	cout << "Register A: " << hex << (int)cpu.r.A << "\n";

	cout << "Register SP: " << hex << (int)cpu.r.SP << "\n";
	cout << "Register PC: " << hex << (int)cpu.r.PC << "\n";
	cout << "Register HL: " << hex << (int)cpu.r.HL << "\n";

	cout << "\n---- END CPU STACK TRACE ----\n\n";
}

void cpu_ld_sp_d16(processor &cpu)
{
	uint8_t b1, b2;
	b1 = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	b2 = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x2);

	uint16_t res;
	res = b1 | (b2 << 8);

	//Set stack pointer.
	cpu.r.SP = res;

	//Op size is 3
	cpu.r.PC += 0x3;

	//cout << hex << (int)res << "\n";

}

void cpu_xor_a(processor &cpu)
{
	//Really, this is just setting r.A to zero...
	cpu.r.A = (cpu.r.A ^ cpu.r.A);
	cpu.r.PC += 0x1;
}

void cpu_ld_hl_d16(processor &cpu)
{
	uint8_t b1, b2;
	b1 = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	b2 = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x2);

	uint16_t res;
	res = b1 | (b2 << 8);

	//Set HL register.
	cpu.r.HL = res;

	//Op size is 3
	cpu.r.PC += 0x3;

	cout << hex << (int)res << "\n";
}

void cpu_ld_hldec_a(processor &cpu)
{
	mmu_absolute_write(cpu.mmu, cpu.r.HL, cpu.r.A);
	cpu.r.HL--;
	cpu.r.PC += 0x1;
}

void cpu_prefix_cb(processor &cpu)
{
	uint8_t cb = mmu_absolute_read(cpu.mmu, cpu.r.PC + 0x1);
	uint8_t hb, msb;
	switch(cb)
	{
		case 0x7C:
			//Check most significant bit.
			hb = cpu.r.HL >> 8;
			msb = 1 << (8 -1);
			if (hb & msb)
				cpu.r.F = 0x1; //Set F register cleared.
			else
				cpu.r.F = 0x0; //Set F register active.
		break;
		default:
			cerr << "Hit unknown Prefix CB in cpu_prefix_cb, " << hex << cb << "\n";
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
