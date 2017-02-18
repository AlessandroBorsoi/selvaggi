#ifndef _UPO_SEMAPHORE_H
#define _UPO_SEMAPHORE_H

typedef struct upo_sem_s upo_sem_t;

void upo_sem_init(upo_sem_t* semaphore, int value);

void upo_sem_wait(upo_sem_t* semaphore);

void upo_sem_post(upo_sem_t* semaphore);

#ifdef __APPLE__
#include <dispatch/dispatch.h>

struct upo_sem_s 
{
    dispatch_semaphore_t sem;
};

void upo_sem_init(upo_sem_t* semaphore, int value)
{
    dispatch_semaphore_t* sem = &semaphore->sem;
    *sem = dispatch_semaphore_create(value);
}

void upo_sem_wait(upo_sem_t* semaphore)
{
    dispatch_semaphore_wait(semaphore->sem, DISPATCH_TIME_FOREVER);
}

void upo_sem_post(upo_sem_t* semaphore)
{
    dispatch_semaphore_signal(semaphore->sem);
}
#else
#include <semaphore.h>

struct upo_sem_s 
{
    sem_t sem;
};

void upo_sem_init(upo_sem_t* semaphore, int value)
{
    sem_init(&semaphore->sem, 0, value);
}

void upo_sem_wait(upo_sem_t* semaphore)
{
    sem_wait(&semaphore->sem);
}

void upo_sem_post(upo_sem_t* semaphore)
{
    sem_post(&semaphore->sem);
}
#endif

#endif /* _UPO_SEMAPHORE_H */
