//Deadlock Assignment

/* After the #includes, the function prototypes and the global variable, we come to the main function. There the semaphore is created with a call to semget, which returns the semaphore ID. If the program is the rst to be called (i.e. it's called with a parameter and argc > 1), a call is made to set_semvalue to initialize the semaphore and op_char is set to X. [./sem & ./sem1]*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

static int set_semvalue(int sem_id);
static void del_semvalue(int sem_id);
static int semaphore_p(int sem_id);
static int semaphore_v(int sem_id);
 
static int sem_id;

union semun{
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

void f1(int s1, int s2) {
    //if(child) {
        if (!semaphore_p(s1)) exit(EXIT_FAILURE);  // lock
        f2(s1,s2);
        if (!semaphore_v(s1)) exit(EXIT_FAILURE);  // unlock        
    //}
    
}

void f2(int s1, int s2) {
    //if(!child) {
        if (!semaphore_p(s2)) exit(EXIT_FAILURE);  // lock
        f1(s1,s2);
        if (!semaphore_v(s2)) exit(EXIT_FAILURE);  // unlock        
    //}
    
}

int main(int argc, char *argv[]) {
  int child;
  int s1 = semget((key_t)1234, 1, 0666 | IPC_CREAT);  // get semaphore id
  int s2 = semget((key_t)1235, 1, 0666 | IPC_CREAT);  // get semaphore id 

  set_semvalue(s1);
  set_semvalue(s2);

  child = fork();

  if(child) {
    f1(s1,s2);
  } else {
    f2(s2,s1);
  }
  printf("\n%d - Doned\n", getpid());

  del_semvalue(s1); // delete or remove semaphore
  del_semvalue(s2);
}

/* The function set_semvalue initializes the semaphore using the SETVAL command in a semctl call. We need to do this before we can use the semaphore. */
static int set_semvalue(int sem_id) {
  union semun sem_union;
  sem_union.val = 1;
  if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
    return (0);
  return (1);
}

/* The del_semvalue function has almost the same form, except the call to semctl uses the command IPC_RMID to remove the semaphore's ID. */
static void del_semvalue(int sem_id) {
  union semun sem_union;

  if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)  // remove semaphore
    fprintf(stderr, "Failed to delete semaphore\n");
}

/* semaphore_p changes the semaphore by -1 (waiting). */
static int semaphore_p(int sem_id) {
  struct sembuf sem_b;

  sem_b.sem_num = 0;
  sem_b.sem_op = -1; /* P() */  // decrement == lock
  sem_b.sem_flg = SEM_UNDO;
  if (semop(sem_id, &sem_b, 1) == -1) {
    fprintf(stderr, "semaphore_p failed\n");
    return (0);
  }
  return (1);
}

/* semaphore_v is similar except for setting the sem_op part of the sembuf structure to 1, so that the semaphore becomes available. */
static int semaphore_v(int sem_id) {
  struct sembuf sem_b;
  sem_b.sem_num = 0;
  sem_b.sem_op = 1; /* V() */  // increment == unlock
  sem_b.sem_flg = SEM_UNDO;
  if (semop(sem_id, &sem_b, 1) == -1) {
    fprintf(stderr, "semaphore_v failed\n");
    return (0);
  }
  return (1);
}

