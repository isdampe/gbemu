#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"
#include "cpu.h"

using namespace std;

memory mmu_create()
{
	memory mmu;
	memset(&mmu.memory, 0, 0xFFFF);

	return mmu;
}

void mmu_absolute_write(memory &mmu, const uint16_t addr, const uint8_t value)
{
	if (addr > 0xFFFF)
	{
		cerr << "FATAL: mmu_absolute_write called with address " << hex << addr  \
			<< " which is out of bounds." << endl;
		exit(1);
	}

	mmu.memory[addr] = value;
}

uint8_t mmu_absolute_read(memory &mmu, const uint16_t addr)
{
	if (addr > 0xFFFF)
	{
		cerr << "FATAL: mmu_absolute_read called with address " << hex << addr  \
			<< " which is out of bounds." << endl;
		exit(1);
	}

	return mmu.memory[addr];
}

uint16_t mmu_absolute_read_u16(memory &mmu, const uint16_t addr)
{
	uint8_t b1, b2;
	b1 = mmu_absolute_read(mmu, addr);
	b2 = mmu_absolute_read(mmu, addr + 0x1);
	uint16_t res = b1 | (b2 << 8);
	return res;
}
