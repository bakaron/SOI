#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define true 1
#define num 5
#define size 5


struct fifo {
    int index;//0..4
    int buf[size];
};
typedef struct fifo fifo;

void add_elem(fifo *fif, int elem) {

    fif->buf[fif->index] = elem;
    fif->index += 1;
}

void stan_kolejki(fifo *fif) {
    int i = 0;
    for (i = 0; i <= fif->index-1; i++) {
        printf("%d ", fif->buf[i]);
    }
    printf("\n");
}

int get_elem(struct fifo *fif) {
    int ret = fif->buf[0];
    int i;
    fif->index -= 1;
    for(i=0;i<fif->index;i++){
        fif->buf[i]=fif->buf[i+1];
    }

}


fifo buf[num];
sem_t mutex[num];
sem_t empty[num];
sem_t full[num];
sem_t all_full;


pthread_t prod;
pthread_t konsum[num];


void *producent() {
    while (true) {

        int i = rand() % 5;
        int input = rand()% 18;//produce item

        sem_wait(&empty[i]);
        sem_wait(&mutex[i]);
        printf("Producent dodał do kolejki nr: %d kolejka:",i);
        add_elem(&buf[i], input);

        stan_kolejki(&buf[i]);

        sem_post(&mutex[i]);
        sem_post(&full[i]);

        sleep(1 + rand() % 3);

    }
}

void *konsument() {
    while (true) {

        //unsigned int i = 0;//which fifo to produce to
        //while (pthread_equal(konsum[i], pthread_self()) && i<num) {
       //     i++;
       // }
        int i=rand()&5;
        sem_wait(&full[i]);
        sem_wait(&mutex[i]);
        printf("Konsument o nr:%d ",i);
        get_elem(&buf[i]);
        stan_kolejki(&buf[i]);

        sem_post(&mutex[i]);
        sem_post(&empty[i]);
        //  sleep(1 + rand() % 3);

    }

}


int main() {
    srand(time(NULL));
    int i;
    for (i = 0; i < num; i++) {
        sem_init(&full[i], 0, 0);
        sem_init(&empty[i], 0, size);
        sem_init(&mutex[i], 0, 1);
    }


    pthread_create(&prod, NULL, &producent, NULL);
    for (i = 0; i < num; i++) {
        pthread_create(&konsum[i], NULL, &konsument, NULL);
    }

    pthread_join(prod, NULL);
    for (i = 0; i < num; i++) {
        pthread_join(konsum[i], NULL);
    }

    return 0;
}

