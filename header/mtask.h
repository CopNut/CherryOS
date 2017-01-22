#ifndef MEMORY_H
#define MEMORY_H 1
#include <memory.h>
#endif


#define MAX_TASKS		1000	/* Å‘åƒ^ƒXƒN” */
#define TASK_GDT0		3		/* TSS‚ðGDT‚Ì‰½”Ô‚©‚çŠ„‚è“–‚Ä‚é‚Ì‚© */
typedef struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
}TSS, *TSSPtr;
typedef struct TASK {
	int sel, flags; /* sel‚ÍGDT‚Ì”Ô†‚Ì‚±‚Æ */
	struct TSS32 tss;
}Task, *TaskPtr;
typedef struct TASKCTL {
	int running; /* “®ì‚µ‚Ä‚¢‚éƒ^ƒXƒN‚Ì” */
	int now; /* Œ»Ý“®ì‚µ‚Ä‚¢‚éƒ^ƒXƒN‚ª‚Ç‚ê‚¾‚©•ª‚©‚é‚æ‚¤‚É‚·‚é‚½‚ß‚Ì•Ï” */
	struct TASK *tasks[MAX_TASKS];
	struct TASK tasks0[MAX_TASKS];
}TaskCtl, *TaskCtlPtr;

extern TimerPtr timerPtr_mt;
void MT_init(void);
void MT_taskswitch(void);