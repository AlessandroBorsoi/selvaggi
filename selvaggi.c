#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "upo_semaphore.h"

int pentola, cucinato;
upo_sem_t mutex, empty, full;

void cucina() 
{
    printf("Cucino pasto\n");
    fflush(stdout);
    pentola++;
}

void mangia()
{
    printf("Selvaggio mangia\n");
    fflush(stdout);
    pentola--;
}

void* cuoco(void* args)
{
    printf("Cuoco svegliato\n");
    int porzioni = *(int*)args;
    upo_sem_wait(&mutex);
    if (pentola == 0)
    {
        for (int i = 0; i < porzioni; ++i)
        {
            upo_sem_wait(&empty);
            cucina();
            upo_sem_post(&full);
        }
        cucinato++;
    }
    upo_sem_post(&mutex);
    return NULL;
}

void* selvaggio(void* args)
{
    int porzioni = *(int*)args;
    printf("Selvaggio creato\n");
    for (int i = 0; i < porzioni; ++i)
    {
        pthread_t t;
        pthread_create(&t, NULL, cuoco, &porzioni);
        pthread_join(t, NULL);
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
    pentola = 0;
    cucinato = 0;
    upo_sem_init(&mutex, 1);
    upo_sem_init(&empty, porzioni);
    upo_sem_init(&full, 0);
    pthread_t t[selvaggi];
    for (i = 0; i < selvaggi; ++i)
    {
        pthread_create(&t[i], NULL, selvaggio, &porzioni);
        pthread_join(t[i], NULL);
    }
    printf("Il cuoco ha cucinato %d volte\n", cucinato);
    return 0;
}
