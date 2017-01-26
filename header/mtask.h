#define MAX_TASKS		1000	
#define TASK_GDT0		3		
#define MAX_TASKS_LV	100
#define MAX_TASKLEVELS	10

typedef struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
}TSS, *TSSPtr;

typedef struct TASK {
	int sel, flags;
	int level, priority;
	struct TSS32 tss;
}Task, *TaskPtr;

typedef struct TASKLEVEL {
	int running;
	int now;
	struct TASK *tasks[MAX_TASKS_LV];
}TaskLevel, *TaskLevelPtr;

typedef struct TASKCTL {
	int now_lv;
	char lv_change;
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
}TaskCtl, *TaskCtlPtr;

extern struct TIMER *timerPtr_task;
TaskPtr Task_init();
TaskPtr Task_alloc(void);
void Task_run(TaskPtr task, int level, int priority);
void Task_switch(void);
void Task_sleep(TaskPtr task);