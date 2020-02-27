#ifndef HAL_CORDIC_H_
#define HAL_CORDIC_H_

#include "emu_cpu.h"

/* implement functions which will be replaced by real firmware */
/* use define for different compile options if possible */
int cordic_rd_ctl(unsigned int adr) {
  unsigned int data;
  Emu_Cpu::get_instance()->read_bus(adr, &data);
  return data;
}

void cordic_wr_phi(unsigned int adr, unsigned int data) {
  Emu_Cpu::get_instance()->write_bus(adr, data);
}

int cordic_rd_xy(unsigned int adr) {
  unsigned int data;
  Emu_Cpu::get_instance()->read_bus(adr, &data);
  return data;
}

#endif
