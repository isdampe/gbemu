#include <iostream>
#include "cpu.h"

using namespace std;

int main(int argc, char *argv[])
{
	processor cpu = cpu_create("/home/dampe");
	cpu_main(cpu);
 	return 0;
}
