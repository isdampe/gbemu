#pragma once
#include <stdint.h>

using namespace std;

struct memory
{
	uint8_t memory[0xFFFF];
};

memory mmu_create();
void mmu_absolute_write(memory &mmu, const uint16_t addr, const uint8_t value);
uint8_t mmu_absolute_read(memory &mmu, const uint16_t addr);
