#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t mutex, empty, full;

struct selvaggio_s
{
    int id;
    int pasti;
    int pasti_max;
};

typedef struct selvaggio_s* selvaggio_t;

void cucina() 
{
    printf("Cucino\n");
    fflush(stdout);
}

void mangia(int id)
{
    printf("Selvaggio %d mangia\n", id);
    fflush(stdout);
}

void* cuoco(void* args)
{
    printf("Cuoco creato\n");
    for (int i = 0; i < 5; i++)
    {
        sem_wait(&empty);
        sem_wait(&mutex);
        cucina();
        sem_post(&mutex);
	    sem_post(&full);
    }
    return NULL;
}

void* selvaggio(void* args)
{
    selvaggio_t selvaggio = args;
    printf("Selvaggio %d creato\n", selvaggio->id);
    for (int i = 0; i < 5; ++i)
    {
        sem_wait(&full);
  	    sem_wait(&mutex);
  	    mangia(selvaggio->id);
  	    sem_post(&mutex);
  	    sem_post(&empty);
    }
    free(selvaggio);
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
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, porzioni);
    sem_init(&full, 0, 0);

    pthread_t t[selvaggi + 1];
    for (i = 0; i < selvaggi; ++i)
    {
        selvaggio_t dati_selvaggio = malloc(sizeof(struct selvaggio_s));
        dati_selvaggio->id = i;
        dati_selvaggio->pasti = 0;
        dati_selvaggio->pasti_max = pasti;
        pthread_create(&t[i], NULL, selvaggio, dati_selvaggio);
    }
    pthread_create(&t[i], NULL, cuoco, NULL);
    pthread_join(t[i], NULL);
    return 0;
}
