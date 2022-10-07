#ifndef CONSTANT_H
#define CONSTANT_H

typedef unsigned char byte;
typedef long word;

#define IHALT 0
#define INOP 1
#define IRRMOVQ 2
#define IIRMOVQ 3
#define IRMMOVQ 4
#define IMRMOVQ 5
#define IOPQ 6
#define IJXX 7
#define ICALL 8
#define IRET 9
#define IPUSHQ 0xA
#define IPOPQ 0xB

#define ALUADD 0
#define ALUSUB 1
#define ALUAND 2
#define ALUXOR 3

#define ALUUNCON 0
#define ALULE 1
#define ALUL 2
#define ALUE 3
#define ALUNE 4
#define ALUGE 5
#define ALUG 6

#define AOK 1
#define HLT 2
#define ADR 3
#define INS 4

#define RRSP 4

#define ZF 0
#define SF 1
#define OF 2

#define MEMORY_SIZE 2048

#endif // CONSTANT_H
