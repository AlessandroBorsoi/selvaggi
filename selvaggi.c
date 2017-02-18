#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "upo_semaphore.h"

struct shared_s
{
    int pasti_in_pentola;
    int pasti_cucinati;
    int pasti_da_effettuare;
    int porzioni;
    int cucina;
    int id_selvaggio;
};

typedef struct shared_s* shared_t;

upo_sem_t mutex, empty, full;

void* cuoco(void* args)
{
    shared_t shared = (shared_t)args;
    while (shared->cucina)
    {
  	    upo_sem_wait(&mutex);
        if (shared->pasti_in_pentola == 0)
        {
            printf("Cuoco svegliato\n");
            for (int i = 0; i < shared->porzioni; ++i)
            {
                upo_sem_wait(&empty);
                printf("Cucino la porzione %d\n", i + 1);
                fflush(stdout);
                shared->pasti_in_pentola++;
                upo_sem_post(&full);
            }
            shared->pasti_cucinati++;
        }
  	    upo_sem_post(&mutex);
    }
    return NULL;
}

void* selvaggio(void* args)
{
    int id = 0;
    shared_t shared = (shared_t)args;
    printf("Selvaggio creato\n");
    for (int i = 0; i < shared->pasti_da_effettuare; ++i)
    {
        upo_sem_wait(&full);
  	    upo_sem_wait(&mutex);
        if (id == 0) id = ++shared->id_selvaggio;
        printf("Selvaggio %d mangia la porzione: %d/%d\n", id, i + 1, shared->pasti_da_effettuare);
        fflush(stdout);
        shared->pasti_in_pentola--;
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

    shared_t shared = malloc(sizeof(struct shared_s));
    shared->pasti_in_pentola = 0;
    shared->pasti_cucinati = 0;
    shared->pasti_da_effettuare = pasti;
    shared->porzioni = porzioni;
    shared->cucina = 1;
    shared->id_selvaggio = 0;

    pthread_t t[selvaggi + 1];
    for (i = 0; i < selvaggi; ++i)
        pthread_create(&t[i], NULL, selvaggio, shared);
    pthread_create(&t[i], NULL, cuoco, shared);

    for (i = 0; i < selvaggi; ++i)
        pthread_join(t[i], NULL);

    upo_sem_wait(&mutex);
    shared->cucina = 0;
    upo_sem_post(&mutex);

    pthread_join(t[i], NULL);

    printf("Il cuoco ha cucinato %d volte\n", shared->pasti_cucinati);
    free(shared);
    return 0;
}
