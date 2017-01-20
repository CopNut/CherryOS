#ifndef MEMORY_H
#define MEMORY_H 1
#include <memory.h>
#endif


#define MAX_TASKS		1000	
#define TASK_GDT0		3		
typedef struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
}TSS, *TSSPtr;

typedef struct TASK {
	int sel, flags;
	struct TSS32 tss;
}Task, *TaskPtr;

typedef struct TASKCTL {
	int running;
	int now;
	TaskPtr tasks[MAX_TASKS];
	Task tasks0[MAX_TASKS];
}TaskCtl, *TaskCtlPtr;


void load_tr(int tr);
void Task_switch3(void);
void Task_switch4(void);
void farjmp(int eip, int cs);
TaskPtr task_init(MemoryPtr memory);
TaskPtr task_alloc(void);
void task_run(TaskPtr task);
