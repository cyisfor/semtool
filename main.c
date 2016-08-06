#include <stdlib.h> // NULL
#include <semaphore.h> // sem_open etc
#include <fcntl.h> // O_CREAT
#include <string.h> // strcasecmp
#include <assert.h>
#include <math.h> // modf
#include <time.h> // clock_gettime



#define FOROPS X(POST) X(WAIT) X(UNLINK)

enum op { 
	#define X(op) op,
	FOROPS
	#undef X
	NUMOPS
};

const char* opname[] = {
#define X(op) #op,
	FOROPS
	#undef X
	NULL
};

enum op select_op(const char* name) {
	enum op ret;
	for(ret=POST;ret!=NUMOPS;++ret) {
		if(0==strcasecmp(opname[ret],name))
			return ret;
	}
	abort();
}

#include <error.h>
#include <errno.h>
#define die(num,...) error(num,errno, ## __VA_ARGS__)

enum deaths {
	DIE_OPEN = 1,
	DIE_WAIT,
	DIE_POST,
	DIE_UNLINK,
	DIE_USAGE
};
	

void process_WAIT(const char* name) {
	unsigned int initial_value = 0;
	const char* val = getenv("initial");
	char* err = NULL;
	if(val != NULL) {
		initial_value = strtol(val,&err,10);
		assert(err == NULL || *err == '\0');
	}
	sem_t* sem = sem_open(name, O_CREAT, 0600, initial_value);
	if(sem == SEM_FAILED)
		die(DIE_OPEN,"sem_open failed for %s",name);
	val = getenv("timeout");
	int res;
	if(val == NULL) {
		res = sem_wait(sem);
	} else {
		double iptr;
		double frac = modf(strtod(val, &err),&iptr);
		assert(err == NULL || *err == '\0');
		struct timespec timeout;
		clock_gettime(CLOCK_REALTIME, &timeout);
		timeout.tv_sec += iptr;
		timeout.tv_nsec += frac * 1000000000;
		res = sem_timedwait(sem, &timeout);
	}
	if(res != 0)
		die(DIE_WAIT,"sem_wait failed for %s",name);
}

void process_POST(const char* name) {
	sem_t* sem = sem_open(name, 0);
	if(sem == SEM_FAILED)
		die(DIE_OPEN,"sem_open failed for %s",name);

	if(0 != sem_post(sem))
		die(DIE_POST,"sem_post failed for %s",name);
}

void process_UNLINK(const char* name) {
	if(0 != sem_unlink(name))
		die(DIE_UNLINK,"sem_unlink failed for %s",name);
}

int process(const char* name, enum op op) {
	switch(op) {
#define X(op)																		\
		case op:																		\
			process_ ## op(name);											\
			break;
		FOROPS
	default:
		abort();
	}
	return 0;
}
			
int main(int argc, char *argv[])
{
	const char* name, *op;
	switch(argc) {
	case 1:
		name = getenv("name");
		op = getenv("op");
		if(!(name && op))
			die(DIE_USAGE,"usage: semtool name op");
		break;
	case 2:
		name = getenv("name");
		if(name == NULL)
			die(DIE_USAGE,"usage: name=name semtool op");
		op = argv[1];
		break;
	case 3:
		name = argv[1];
		op = argv[2];
		break;
	default:
		die(DIE_USAGE,"usage: semtool name op");
	};

	return process(name,select_op(op));
}
