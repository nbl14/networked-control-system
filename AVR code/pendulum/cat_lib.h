void setupADC_8(void);
uint8_t getADCdata_8(uint8_t channel);
void setupADC_10(void);
void setupADC_10_aref(void);
uint16_t getADCdata_10(uint8_t channel);

void setupCounterT0(void);
void setupCounterT0_nonstop(void);
uint8_t count_for_100ms_T0_nonstop(void);
uint8_t count_for_100ms_T0(void);
uint8_t count_for_95ms_T0_nonstop(void);
uint8_t count_for_50ms_T0_nonstop(void);
uint8_t count_for_50ms_T0(void);
void setupCounterT0_asynch(void);

void setupPWM_T1(void);
void startPWM_T1A_LOW1_Q10(void);
void stopPWM_T1A_LOW1_Q10(void);
void startPWM_T1B_LOW2_Q11(void);
void stopPWM_T1B_LOW2_Q11(void);
void setupPWM_T2(void);

#define pwm_duty_cycle_t1a_low1_q10(x)     OCR1A = x
#define pwm_duty_cycle_t1b_low2_q11(x)     OCR1B = x
#define pwm_duty_cycle_t2(x)     OCR2 = x