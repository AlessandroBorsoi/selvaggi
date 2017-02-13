#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "upo_semaphore.h"

upo_sem_t mutex, empty, full;

void cucina(int* pasti_da_cucinare) 
{
    printf("Cucino pasto %d\n", *pasti_da_cucinare);
    fflush(stdout);
    (*pasti_da_cucinare)--;
}

void mangia()
{
    printf("Selvaggio mangia\n");
    fflush(stdout);
}

void* cuoco(void* args)
{
    int pasti_da_cucinare = *(int*)args;
    printf("Cuoco creato\n");
    do
    {
        upo_sem_wait(&empty);
        upo_sem_wait(&mutex);
        cucina(&pasti_da_cucinare);
        upo_sem_post(&mutex);
	    upo_sem_post(&full);
    } while (pasti_da_cucinare);
    return NULL;
}

void* selvaggio(void* args)
{
    int porzioni = *(int*)args;
    printf("Selvaggio creato\n");
    for (int i = 0; i < porzioni; ++i)
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
    int pasti_da_cucinare = selvaggi * pasti;
    for (i = 0; i < selvaggi; ++i)
    {
        pthread_create(&t[i], NULL, selvaggio, &porzioni);
    }
    pthread_create(&t[i], NULL, cuoco, &pasti_da_cucinare);
    pthread_join(t[i], NULL);
    return 0;
}
