#ifndef MAIN_C_H_
#define MAIN_C_H_

#ifdef __cplusplus
extern "C" {
#endif

/* irq hang for emu cpu, virutal enabled in driver */
void irq_forward();

/* external code */
int main_c();

#ifdef __cplusplus
}
#endif

#endif
