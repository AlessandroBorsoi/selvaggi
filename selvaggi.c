#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "upo_semaphore.h"

struct info_s
{
    int pasti_in_pentola;
    int pasti_cucinati;
    int pasti_da_effettuare;
    int porzioni;
};

typedef struct info_s* info_t;

int idSelvaggio;
upo_sem_t mutex, empty, full;

void* cuoco(void* args)
{
    info_t info = (info_t)args;
    upo_sem_wait(&mutex);
    if (info->pasti_in_pentola == 0)
    {
        printf("Cuoco svegliato\n");
        for (int i = 0; i < info->porzioni; ++i)
        {
            upo_sem_wait(&empty);
            printf("Cucino pasto %d\n", i + 1);
            fflush(stdout);
            info->pasti_in_pentola++;
            upo_sem_post(&full);
        }
        info->pasti_cucinati++;
    }
    upo_sem_post(&mutex);
    return NULL;
}

void* selvaggio(void* args)
{
    int id = ++idSelvaggio;
    info_t info = (info_t)args;
    printf("Selvaggio creato\n");
    for (int i = 0; i < info->pasti_da_effettuare; ++i)
    {
        // if (info->pasti_in_pentola == 0)
        // {
            pthread_t t;
            pthread_create(&t, NULL, cuoco, info);
        // }
        upo_sem_wait(&full);
  	    upo_sem_wait(&mutex);
        printf("Selvaggio %d mangia\n", id);
        fflush(stdout);
        info->pasti_in_pentola--;
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
    info_t info = malloc(sizeof(struct info_s));
    info->pasti_in_pentola = 0;
    info->pasti_cucinati = 0;
    info->pasti_da_effettuare = pasti;
    info->porzioni = porzioni;
    pthread_t t[selvaggi];
    for (i = 0; i < selvaggi; ++i)
        pthread_create(&t[i], NULL, selvaggio, info);
    for (i = 0; i < selvaggi; ++i)
        pthread_join(t[i], NULL);
    printf("Il cuoco ha cucinato %d volte\n", info->pasti_cucinati);
    free(info);
    return 0;
}
