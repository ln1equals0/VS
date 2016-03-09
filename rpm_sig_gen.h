#ifndef RPM_SIG_GEN_H
#define RPM_SIG_GEN_H


extern uint32_t wheelfreq;
extern uint16_t logging;
uint8_t out_of_range;

void setupRPMdivider(void);
void setupUART_RPM_out(void);







#endif // RPM_SIG_GEN_H
