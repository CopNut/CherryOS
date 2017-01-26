#include <stdio.h>
#include <CherryOS.h>

extern MemoryPtr memory;

TaskCtlPtr taskCtl;
TimerPtr timerPtr_task;

struct TASK *Task_now(void)
{
	struct TASKLEVEL *tl = &taskCtl->level[taskCtl->now_lv];
	return tl->tasks[tl->now];
}

void Task_add(struct TASK *task)
{
	struct TASKLEVEL *tl = &taskCtl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2; /* “®ì’† */
	return;
}

void Task_remove(struct TASK *task)
{
	int i;
	struct TASKLEVEL *tl = &taskCtl->level[task->level];

	/* task‚ª‚Ç‚±‚É‚¢‚é‚©‚ð’T‚· */
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			/* ‚±‚±‚É‚¢‚½ */
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--; /* ‚¸‚ê‚é‚Ì‚ÅA‚±‚ê‚à‚ ‚í‚¹‚Ä‚¨‚­ */
	}
	if (tl->now >= tl->running) {
		/* now‚ª‚¨‚©‚µ‚È’l‚É‚È‚Á‚Ä‚¢‚½‚çAC³‚·‚é */
		tl->now = 0;
	}
	task->flags = 1; /* ƒXƒŠ[ƒv’† */

	/* ‚¸‚ç‚µ */
	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;
}

void Task_switchsub(void)
{
	int i;
	/* ˆê”Ôã‚ÌƒŒƒxƒ‹‚ð’T‚· */
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskCtl->level[i].running > 0) {
			break; /* Œ©‚Â‚©‚Á‚½ */
		}
	}
	taskCtl->now_lv = i;
	taskCtl->lv_change = 0;
	return;
}


TaskPtr Task_init()
{
	int i;
	TaskPtr task;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	taskCtl = (TaskCtlPtr) Memory_alloc_4k(memory, sizeof (TaskCtl));
	for (i = 0; i < MAX_TASKS; i++) {
		taskCtl->tasks0[i].flags = 0;
		taskCtl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskCtl->tasks0[i].tss, AR_TSS32);
	}
	task = Task_alloc();
	task->flags = 2;
	task->priority = 2; //0.02s
	task->level = 0;
	Task_add(task);
	Task_switchsub();
	load_tr(task->sel);
	timerPtr_task = timer_alloc();
	timer_settime(timerPtr_task, task->priority);
	return task;
}

TaskPtr Task_alloc(void)
{
	int i;
	TaskPtr task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskCtl->tasks0[i].flags == 0) {
			task = &taskCtl->tasks0[i];
			task->flags = 1;
			task->tss.eflags = 0x00000202;
			task->tss.eax = 0;
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
	return 0;
}

void Task_run(TaskPtr task, int level, int priority)
{
	if (level < 0)
	{
		level = task->level;
	}

	if (priority > 0)
	{
		task->priority = priority;
	}

	if (task->flags != 2)
	{
		task->level = level;
		Task_add(task);
	}

	taskCtl->lv_change = 1;
	return;
}

void Task_switch(void)
{
	struct TASKLEVEL *tl = &taskCtl->level[taskCtl->now_lv];
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskCtl->lv_change != 0) {
		Task_switchsub();
		tl = &taskCtl->level[taskCtl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(timerPtr_task, new_task->priority);
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
	return;
}

void Task_sleep(TaskPtr task)
{
	int i;
	char ts = 0;
	struct TASK *now_task;
	if (task->flags == 2) {		/* 如果任务处于唤醒状态 */
		now_task = Task_now();
		Task_remove(task);
		if (task == now_task)
		{
			Task_switchsub();
			now_task = Task_now();
			farjmp(0, now_task->sel);
		}
	}
	return;
}

