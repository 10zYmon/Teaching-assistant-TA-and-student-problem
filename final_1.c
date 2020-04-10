#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void* student_act( void* student_id );
void* TA_act();

#define NUM_CHAIRS 3
#define num_students 5
sem_t sem_students;
sem_t sem_ta;
pthread_mutex_t mutex;

int wait_chairs[3];
int no_waiting = 0;
int next_pos = 0;
int nteach_pos = 0;
int ta_sleep = 0;

int main(){

    int i;
    
    int student_ids[num_students];
    pthread_t students[num_students];
    pthread_t ta;

    sem_init( &sem_students, 0, 0 );
    sem_init( &sem_ta, 0, 1 );

    //Create threads.
    pthread_mutex_init( &mutex, NULL );
    pthread_create( &ta, NULL, TA_act, NULL );
    
    for( i = 0; i < num_students; i++ )
    {
        student_ids[i] = i + 1;
        pthread_create( &students[i], NULL, student_act, (void*) &student_ids[i] );
    }

    //Join threads
    pthread_join(ta, NULL);
    for( i =0; i < num_students; i++ )
    {
        pthread_join( students[i],NULL );
    }

    return 0;
}

void* TA_act()
{
    printf( "Checking for students.\n" );
    while( 1 )
    {
        int a=0;
        //if students are waiting
        if ( no_waiting > 0 ) {

            ta_sleep = 0;
            sem_wait( &sem_students );
            pthread_mutex_lock( &mutex );

            //TA helping student.
            printf( "Helping a student. Students waiting = %d.\n", (no_waiting - 1) );
            printf( "Student %d receiving help.\n",wait_chairs[nteach_pos] );

            wait_chairs[nteach_pos]=0;
            no_waiting--;
            nteach_pos= ( nteach_pos + 1 ) % NUM_CHAIRS;

            sleep(rand()%5);//random sleep

            pthread_mutex_unlock( &mutex );
            sem_post( &sem_ta );

        }
        //if no students are waiting
        else
        {
            if ( ta_sleep == 0 )
            {

                printf( "No students waiting. Sleeping.\n" );
                ta_sleep = 1;
            }
        }
    }
}

void* student_act( void* student_id ) {

    int id_student = *(int*)student_id;

    while( 1 ) {

        //if student is waiting, continue waiting
        if ( isWaiting( id_student ) == 1 ) { continue; }

        //student is programming.
     
        printf( "\tStudent %d is programming.\n", id_student);
        
        sleep( rand() % 5 );
       
        pthread_mutex_lock( &mutex );

        if( no_waiting < NUM_CHAIRS ) {

            wait_chairs[next_pos] = id_student;
            no_waiting++;

            //student takes a seat in the hallway.
           id_student = id_student%6;
            printf( "\t\tStudent %d takes a seat. Students waiting = %d.\n", id_student, no_waiting );
            
                       printf("\nwaiting students : [1] %d [2] %d [3] %d\n\n",wait_chairs[0],wait_chairs[1],wait_chairs[2]);
            
            next_pos = ( next_pos + 1 ) % NUM_CHAIRS;

            pthread_mutex_unlock( &mutex );

            //wake TA if sleeping
            sem_post( &sem_students );
            sem_wait( &sem_ta );

        }
        else {

            pthread_mutex_unlock( &mutex );

            //No chairs available. Student will try later.
            printf( "\t\t\tStudent %d will try later.\n",id_student );

        }

    }

}

int isWaiting( int student_id ) {
    int i;
    for ( i = 0; i < 3; i++ ) {
        if ( wait_chairs[i] == student_id ) { return 1; }
    }
    return 0;
}
