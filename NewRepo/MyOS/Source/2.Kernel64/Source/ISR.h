#ifndef ISR_H
#define ISR_H

void ISRDivideError();
void ISRDebug();
void ISRNMI();
void ISRBreakPoint();
void ISROverflow();
void ISRBoundRangeExceeded();
void ISRInvalidOpcode();
void ISRDeviceNotAvailable();
void ISRDoubleFault();
void ISRCoprocessorSegmentOverrun();
void ISRInvalidTSS();
void ISRSegmentNotPresent();
void ISRStackSegmentFault();
void ISRGeneralProtection();
void ISRPageFault();
void ISR15();
void ISRFPUError();
void ISRAlignmentCheck();
void ISRMachineCheck();
void ISRSIMDError();
void ISRETCException();

void ISRTimer();
void ISRKeyboard();
void ISRSlavePIC();
void ISRSerial2();
void ISRSerial1();
void ISRParallel2();
void ISRFloppy();
void ISRParallel1();
void ISRRTC();
void ISRReserved();
void ISRNotUsed1();
void ISRNotUsed2();
void ISRMouse();
void ISRCoprocessor();
void ISRHDD1();
void ISRHDD2();
void ISRETCInterrupt();

#endif