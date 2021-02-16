/*
 * Atividade 1
 *
 * A) Tanto em_e quanto em_l devem ser semáforos inicializados com valor 1 pois devem ser utilizados para que haja
 * exclusão mútua.
 *
 * B) Não é possível, uma vez que é preciso manter todos os mecanismos de sincronização para que que se mantenha a
 * prioridade dos escritores.
 *
 * C) Código abaixo: ( resposta da letra D) segue abaixo também, depois do código da letra C) )
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define LEITORES 2
#define ESCRITORES 2
#define NTHREADS 4

sem_t em_e, em_l, escr, leit; //semáforos
int e = 0, l = 0; // globais

void *escritores(void *tId) {

    int *id = (int *) tId;
    while (1) {

        sem_wait(&em_e);
        printf("Escritor iniciando %d\n", id);
        e++;

        if (e == 1) {

            sem_wait(&leit);
        }
        sem_post(&em_e);
        sem_wait(&escr);
        printf("Thread %d realizando escrita...\n", id);
        sem_post(&escr);
        sem_wait(&em_e);
        e--;

        if (e == 0) {

            sem_post(&leit);
        }
        printf("Finalizando escrita %d\n", id);
        sem_post(&em_e);
    }
}

void *leitores(void *tId) {

    int *id = (int *) tId;

    while (1) {

        sem_wait(&leit);
        printf("Leitor iniciando %d\n", id);
        sem_wait(&em_l);
        l++;

        if (l == 1) {

            sem_wait(&escr);
        }

        sem_post(&em_l);
        sem_post(&leit);
        printf("Thread %d realizando leitura...\n", id);
        sem_wait(&em_l);

        if (l == 0) {

            sem_post(&escr);
        }
        printf("Finalizando leitura %d\n", id);
        sem_post(&em_l);
    }

    pthread_exit(NULL);
}

//função principal
int main(int argc, char *argv[]) {

    pthread_t tid[NTHREADS]; //vetor das threads
    int *id[3], i;

    for (i = 0; i < NTHREADS; i++) {

        if ((id[i] = malloc(sizeof(int))) == NULL) {

            pthread_exit(NULL);
            return 1;
        }

        *id[i] = i + 1;
    }

    //inicialização dos semáforos
    sem_init(&em_e, 0, 1);
    sem_init(&escr, 0, 1);
    sem_init(&em_l, 0, 1);
    sem_init(&leit, 0, LEITORES);

    //criação dos escritores
    for(i=ESCRITORES -1; i<LEITORES + ESCRITORES; i++) {

        if(pthread_create(&tid[i], NULL, escritores, (void *)id[i])) {

            fprintf(stderr, "Ocorreu um erro ao executar: pthread_create()\n");
            exit(-1);
        }
    }

    //criação dos leitores
    for(i=0; i<LEITORES; i++) {

        if(pthread_create(&tid[i], NULL, leitores, (void *)id[i])) {

            fprintf(stderr, "Ocorreu um erro ao executar: pthread_create()\n");
            exit(-1);
        }
    }

    //--espera todas as threads terminarem
    for(i=0; i<NTHREADS; i++) {

        if(pthread_join(tid[i], NULL)) {

            fprintf(stderr, "Ocorreu um erro ao executar: pthread_join()\n");
            exit(-1);
        }

        free(id[i]);
    }

    pthread_exit(NULL);
}

//d) Alterações pedidas:

pthread_mutex_t lock;

//escritores
while(1) {

    pthread_mutex_lock(&lock);
    e++;

    if(e==1) {

        sem_wait(&leit);
    }

    pthread_mutex_unlock(&lock);
    sem_wait(&escr);

    /*
    //Realizaria escrita nesse trecho de código (intervalo)
    */

    sem_post(&escr);
    pthread_mutex_lock(&lock);
    e--;

    if(e==0) {

        sem_post(&leit);
    }

    pthread_mutex_unlock(&lock);
}

//leitores
while(1) {

    sem_wait(&leit);
    pthread_mutex_lock(&lock);
    leitores++;

    if(leitores==1) {

        sem_wait(&escr);
    }

    pthread_mutex_unlock(&lock);
    sem_post(&leit);

    /*
    //Realizaria leitura nesse intervalo de código
    */

    pthread_mutex_lock(&lock);
    leitores--;

    if(leitores==0) {

        sem_post(&escr);
    }

    pthread_mutex_unlock(&lock);
}

/*
 * Letra E)
 * Os locks usados não são recursivos, já que, após um bloqueio ser realizado, sempre haverá um chamado de desbloqueio
 * vindo de outra thread antes que todas estejam bloqueadas (não deixa ocorrer "deadlock").
*/
