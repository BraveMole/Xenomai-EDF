#include <stdio.h>
#include <stdlib.h>
#include <copperplate/traceobj.h>
#include <vxworks/errnoLib.h>
#include <vxworks/semLib.h>
#include <vxworks/tickLib.h>
#include <vxworks/kernelLib.h>

static struct traceobj trobj;

#define WAIT_TIME  100
#define TOLERANCE  20
#define MIN_WAIT   (WAIT_TIME - TOLERANCE)

SEM_ID sem_id;

void rootTask(long a0, long a1, long a2, long a3, long a4,
	      long a5, long a6, long a7, long a8, long a9)
{
	ULONG start;
	int ret;

	traceobj_enter(&trobj);

	sem_id = semCCreate(SEM_Q_PRIORITY, 0);
	traceobj_assert(&trobj, sem_id != 0);

	start = tickGet();
	ret = semTake(sem_id, WAIT_TIME);
	traceobj_assert(&trobj, ret == ERROR && errno == S_objLib_OBJ_TIMEOUT);
	traceobj_assert(&trobj, tickGet() - start >= MIN_WAIT);

	start = tickGet();
	ret = semTake(sem_id, WAIT_TIME);
	traceobj_assert(&trobj, ret == ERROR && errno == S_objLib_OBJ_TIMEOUT);
	traceobj_assert(&trobj, tickGet() - start >= MIN_WAIT);

	start = tickGet();
	ret = semTake(sem_id, WAIT_TIME);
	traceobj_assert(&trobj, ret == ERROR && errno == S_objLib_OBJ_TIMEOUT);
	traceobj_assert(&trobj, tickGet() - start >= MIN_WAIT);

	ret = semGive(sem_id);
	traceobj_assert(&trobj, ret == OK);
	ret = semTake(sem_id, WAIT_TIME);
	traceobj_assert(&trobj, ret == OK);
	ret = semGive(sem_id);
	traceobj_assert(&trobj, ret == OK);
	ret = semGive(sem_id);
	traceobj_assert(&trobj, ret == OK);
	ret = semTake(sem_id, WAIT_FOREVER);
	traceobj_assert(&trobj, ret == OK);
	ret = semTake(sem_id, WAIT_FOREVER);
	traceobj_assert(&trobj, ret == OK);

	traceobj_exit(&trobj);
}

int main(int argc, char *argv[])
{
	int ret;

	traceobj_init(&trobj, argv[0], 0);

	ret = kernelInit(rootTask, argc, argv);
	traceobj_assert(&trobj, ret == OK);

	traceobj_join(&trobj);

	ret = semDelete(sem_id);
	traceobj_assert(&trobj, ret == OK);

	exit(0);
}