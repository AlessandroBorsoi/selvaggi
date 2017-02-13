#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef __APPLE__
/*
http://stackoverflow.com/questions/27736618/why-are-sem-init-sem-getvalue-sem-destroy-deprecated-on-mac-os-x-and-w
*/
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

struct upo_sem {
#ifdef __APPLE__
    dispatch_semaphore_t sem;
#else
    sem_t sem;
#endif
};

static inline void
upo_sem_init(struct upo_sem* semaphore, int value)
{
#ifdef __APPLE__
    dispatch_semaphore_t* sem = &semaphore->sem;
    *sem = dispatch_semaphore_create(value);
#else
    sem_init(&semaphore->sem, 0, value);
#endif
}

static inline void
upo_sem_wait(struct upo_sem* semaphore)
{
#ifdef __APPLE__
    dispatch_semaphore_wait(semaphore->sem, DISPATCH_TIME_FOREVER);
#else
    int r;
    do {
        r = sem_wait(&semaphore->sem);
    } while (r == -1 && errno == EINTR);
#endif
}

static inline void
upo_sem_post(struct upo_sem* semaphore)
{
#ifdef __APPLE__
    dispatch_semaphore_signal(semaphore->sem);
#else
    sem_post(&semaphore->sem);
#endif
}

struct upo_sem mutex, empty, full;

void cucina() 
{
    printf("Cucino\n");
    fflush(stdout);
}

void mangia()
{
    printf("Selvaggio mangia\n");
    fflush(stdout);
}

void* cuoco(void* args)
{
    printf("Cuoco creato\n");
    for (int i = 0; i < 5; i++)
    {
        upo_sem_wait(&empty);
        upo_sem_wait(&mutex);
        cucina();
        upo_sem_post(&mutex);
	    upo_sem_post(&full);
    }
    return NULL;
}

void* selvaggio(void* args)
{
    printf("Selvaggio creato\n");
    for (int i = 0; i < 5; ++i)
    {
        upo_sem_wait(&full);
  	    upo_sem_wait(&mutex);
  	    mangia();
  	    upo_sem_post(&mutex);
  	    upo_sem_post(&empty);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    int selvaggi, porzioni, pasti, i;
    if (argc != 4) {
        fprintf(stderr, "Parametri non corretti!\n");
        return EXIT_FAILURE;
    }
    selvaggi = atoi(argv[1]);
    porzioni = atoi(argv[2]);
    pasti = atoi(argv[3]);
    upo_sem_init(&mutex, 1);
    upo_sem_init(&empty, porzioni);
    upo_sem_init(&full, 0);
    pthread_t t[selvaggi + 1];
    for (i = 0; i < selvaggi; ++i)
    {
        pthread_create(&t[i], NULL, selvaggio, NULL);
    }
    pthread_create(&t[i], NULL, cuoco, NULL);
    pthread_join(t[i], NULL);
    return 0;
}
