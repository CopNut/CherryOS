#include <stdio.h>
#include <CherryOS.h>

TaskCtlPtr taskCtl;
TimerPtr task_timer;

TaskPtr task_init(MemoryPtr memory)
{
	int i;
	TaskPtr task;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	taskCtl = (TaskCtlPtr) memman_alloc_4k(memory, sizeof (TaskCtl));
	for (i = 0; i < MAX_TASKS; i++) {
		taskCtl->tasks0[i].flags = 0;
		taskCtl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskCtl->tasks0[i].tss, AR_TSS32);
	}
	task = task_alloc();
	task->flags = 2; /* 動作中マーク */
	taskCtl->running = 1;
	taskCtl->now = 0;
	taskCtl->tasks[0] = task;
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, 2);
	return task;
}

TaskPtr task_alloc(void)
{
	int i;
	TaskPtr task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskCtl->tasks0[i].flags == 0) {
			task = &taskCtl->tasks0[i];
			task->flags = 1; /* 使用中マーク */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			task->tss.eax = 0; /* とりあえず0にしておくことにする */
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0; /* もう全部使用中 */
}

void task_run(TaskPtr task)
{
	task->flags = 2; /* 動作中マーク */
	taskCtl->tasks[taskCtl->running] = task;
	taskCtl->running++;
	return;
}

void task_switch(void)
{
	timer_settime(task_timer, 2);
	if (taskCtl->running >= 2) {
		taskCtl->now++;
		if (taskCtl->now == taskCtl->running) {
			taskCtl->now = 0;
		}
		farjmp(0, taskCtl->tasks[taskCtl->now]->sel);
	}
	return;
}
