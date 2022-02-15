#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <cstring>

using namespace std;

#define number_of_serviceman 3
#define number_of_cycles 10
#define capacity_of_paymentroom 2

pthread_mutex_t serviceman[number_of_serviceman];

pthread_mutex_t mutex;

sem_t pr;

int waiting_room[number_of_cycles];
int departure_line[number_of_cycles];
int j, k = 0;

void *service(void *arg)
{
    for (int i = 0; i < number_of_serviceman; i++)
    {
        if (i == 0)
        {
            pthread_mutex_lock(&serviceman[i]);
        }
        printf("%s th cyclist has started taking service from serviceman %d\n", (char *)arg, i + 1);
        sleep(2);
        if (i < number_of_serviceman - 1)
        {
            pthread_mutex_lock(&serviceman[i + 1]);
        }
        printf("%s th cyclist has finished taking service from serviceman %d\n", (char *)arg, i + 1);
        pthread_mutex_unlock(&serviceman[i]);
    }
    pthread_mutex_lock(&mutex);
    waiting_room[j] = atoi((char *)arg);
    j++;
    pthread_mutex_unlock(&mutex);
}

void *payment(void *arg)
{
    int counter = 0;
    while (true)
    {
        if (counter == number_of_cycles)
            return 0;
        for (int i = 0; i < number_of_cycles; i++)
        {
            if (waiting_room[i] != -1)
            {
                counter++;
                sem_wait(&pr);
                pthread_mutex_lock(&mutex);
                printf("%d th cyclist has started paying Bill\n", waiting_room[i]);
                int temp = waiting_room[i];
                waiting_room[i] = -1;
                pthread_mutex_unlock(&mutex);
                sleep(2);
                pthread_mutex_lock(&mutex);
                printf("%d th cyclist has finished paying Bill\n", temp);
                departure_line[k] = temp;
                k++;
                pthread_mutex_unlock(&mutex);
                sem_post(&pr);
            }
        }
    }
}

void *departure(void *arg)
{
    int counter = 0;
    while (true)
    {
        if (counter == number_of_cycles)
            return 0;
        for (int i = 0; i < number_of_cycles; i++)
        {
            if (departure_line[i] != 0)
            {
                counter++;
                pthread_mutex_lock(&mutex);
                printf("%d th cyclist has departed\n", departure_line[i]);
                departure_line[i] = 0;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}

int main()
{
    int res;
    pthread_t cycle[number_of_cycles];
    pthread_t paymentroom;
    pthread_t departurepath;

    for (int i = 0; i < number_of_cycles; i++)
    {
        waiting_room[i] = -1;
        departure_line[i] = 0;
    }

    for (int i = 0; i < number_of_serviceman; i++)
    {
        res = pthread_mutex_init(&serviceman[i], NULL);
        if (res != 0)
        {
            printf("Failed!!\n");
        }
    }

    res = pthread_mutex_init(&mutex, NULL);

    if (res != 0)
    {
        printf("Failed\n");
    }
    res = pthread_create(&paymentroom, NULL, payment, NULL);

    if (res != 0)
    {
        printf("Thread Creation Failed\n");
    }

    for (int i = 0; i < number_of_cycles; i++)
    {
        char *id = new char[3];
        strcpy(id, to_string(i + 1).c_str());
        res = pthread_create(&cycle[i], NULL, service, (void *)id);
        if (res != 0)
        {
            printf("Thread Creation Failed!!\n");
        }
    }

    res = pthread_create(&departurepath, NULL, departure, NULL);

    if (res != 0)
    {
        printf("Thread Creation Failed\n");
    }

    res = sem_init(&pr, 0, capacity_of_paymentroom);

    if (res != 0)
    {
        printf("Failed\n");
    }

    for (int i = 0; i < number_of_cycles; i++)
    {
        pthread_join(cycle[i], NULL);
    }

    pthread_join(paymentroom, NULL);

    pthread_join(departurepath, NULL);

    for (int i = 0; i < number_of_serviceman; i++)
    {
        res = pthread_mutex_destroy(&serviceman[i]);
        if (res != 0)
        {
            printf("Failed To Destroy\n");
        }
    }
    res = pthread_mutex_destroy(&mutex);
    if (res != 0)
    {
        printf("Failed To Destroy\n");
    }
    res = sem_destroy(&pr);
    if (res != 0)
    {
        printf("Failed To Destroy\n");
    }
    return 0;
}