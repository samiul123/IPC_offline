#include <stdio.h>
#include <libltdl/lt_system.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include <zconf.h>
#include <semaphore.h>

#define STEPS_AB 4
#define STEPS_BC 5
#define STEPS_CD 5
#define STEPS_DD 5

int speed[5];
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int BCCounter = 0;
int CBCounter = STEPS_BC;
int CDCounter = 0;
int DCCounter = STEPS_CD;
void* functionBC(void* arg);
void * functionCD1(void* arg);
void* functionDD(void* arg);
void *functionCB(void *arg);
void* functionDC1(void* arg);
struct cdTraffic {
    int ID;
    bool exist;
    int pos;
};
struct boss{
    sem_t mutex;
    sem_t left;
    sem_t right;
};
struct boss structure;
struct cdTraffic cdArray[STEPS_CD];
struct bCTraffic{
    int ID;
    bool exist;
    int pos;
};
struct bCTraffic bcArray[STEPS_BC];
struct bCTraffic cbArray[STEPS_BC];
struct Persons{
    int ID;
    int personSpeed;
    int distanceCrossed;
    int BCpos;
    int CDpos;
};
int fileRead(){
    int N,firstLoop = 0,j = 0;
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("test1.txt", "r");
    if (fp == NULL)
        printf("File not opened");

    while ((read = getline(&line, &len, fp)) != -1) {
        firstLoop++;
        if(firstLoop == 1){
            N = atoi(line);
        }
        else{
            speed[j] = atoi(line);
            j++;
        }
    }
    int i;
    fclose(fp);
    if (line)
        free(line);
    return N;
}
void* functionAB(void* arg){
    struct Persons* info = (struct Persons *) arg;
    int i;
    float stepNumberAB = ceil((float)STEPS_AB/(float)info->personSpeed);
    int lastStepSize = STEPS_AB % info->personSpeed;
    int prevPos = info->distanceCrossed;
    for(i = 0; i < (int)stepNumberAB; i++){
        if((i == (int)stepNumberAB - 1) && lastStepSize != 0){
            prevPos = info->distanceCrossed;
            info->distanceCrossed += lastStepSize;
            printf("AB----1>Person %d moved along AB from %d to %d\n",info->ID,prevPos,info->distanceCrossed);
            sleep(1);
        }
        else{
            prevPos = info->distanceCrossed;
            info->distanceCrossed += info->personSpeed;
            printf("AB----2>Person %d moved along AB from %d to %d\n",info->ID,prevPos,info->distanceCrossed);
            sleep(1);
        }
    }
    if(info->distanceCrossed == STEPS_AB){
        info->distanceCrossed = 0;
        printf("AB----3>Person %d reached from A to B\n",info->ID);
        functionBC(info);
    }
}
void* functionBA(void* arg) {
    struct Persons *infoBA = (struct Persons *) arg;
    int i;
    float stepNumberBA = ceil((float) STEPS_AB / (float) infoBA->personSpeed);
    int lastStepSize = STEPS_AB % infoBA->personSpeed;
    infoBA->distanceCrossed = STEPS_AB;
    int prevPos = STEPS_AB;
    for (i = 0; i < (int) stepNumberBA; i++) {
        if ((i == (int) stepNumberBA - 1) && lastStepSize != 0) {
            prevPos = infoBA->distanceCrossed;
            infoBA->distanceCrossed -= lastStepSize;
            printf("BA----1>Person %d moved along BA from %d to %d\n", infoBA->ID, prevPos, infoBA->distanceCrossed);
            //printf("BA----2>Person %d finished walking\n",infoBA->ID);
            sleep(1);
        }
        else {
            prevPos = infoBA->distanceCrossed;
            infoBA->distanceCrossed -= infoBA->personSpeed;
            printf("BA----3>Person %d moved along BA from %d to %d\n", infoBA->ID, prevPos, infoBA->distanceCrossed);
            sleep(1);
        }
    }
    if (infoBA->distanceCrossed == 0) {
        infoBA->distanceCrossed = 0;
        printf("BA----2>Person %d finished walking\n", infoBA->ID);

    }
}
void * functionBC(void * arg){
    struct Persons* infoBC = (struct Persons *) arg;
    int i,j,limit = 0,localBCCounter = 0,prevLocalBCCounter = 0;
    //float stepNumberBC = ceil((float)STEPS_BC/(float)infoBC->personSpeed);
    //int lastStepSize = STEPS_BC % infoBC->personSpeed;
    int k;
    //int pos;
    //int outOfbcArray = 0;
    for(i = 0; i < STEPS_BC; i++){
        //searching for blocker
        int blocker = 0;
        pthread_mutex_lock(&mutex1);
        /*printf("BC ARRAY: person %d\n",infoBC->ID);
        for(k = 0; k < STEPS_BC;k++){
            printf("ID: %d EXIST: %d POS: %d\n",bcArray[k].ID,bcArray[k].exist,bcArray[k].pos);
        }*/
        int prevPos = infoBC->BCpos;
        for(j = infoBC->BCpos; j <= (infoBC->BCpos + infoBC->personSpeed); j++){
            if(bcArray[j].exist && bcArray[j].ID != infoBC->ID ){
                /*if(bcArray[j].ID == 3){
                    printf("3 BCPOS %d\n",bcArray[j].pos);
                }*/
                limit = j - 1;
                blocker = 1;
                break;
            }
        }
        if(blocker != 1){
            //pthread_mutex_lock(&mutex1);
            prevLocalBCCounter = localBCCounter;
            bcArray[prevLocalBCCounter].ID = 0;
            bcArray[prevLocalBCCounter].exist = false;
            bcArray[prevLocalBCCounter].pos = 0;
            localBCCounter += infoBC->personSpeed;
            BCCounter += infoBC->personSpeed;
            infoBC->BCpos += infoBC->personSpeed;
            if(infoBC->BCpos >= STEPS_BC){
                infoBC->BCpos = STEPS_BC;
                //printf(">>>>>>>Local_BC_Counter<<<<<<<%d\n",localBCCounter);
                printf("BC----1>Person %d moved along BC from %d to %d\n",infoBC->ID,prevPos,infoBC->BCpos);
                printf("BC----4>Person %d reached from B to C\n",infoBC->ID);
                BCCounter = 0;
                //blocker = 0;
                pthread_mutex_unlock(&mutex1);
                break;
            }
            else{
                bcArray[localBCCounter].ID = infoBC->ID;
                bcArray[localBCCounter].exist = true;
                bcArray[localBCCounter].pos = infoBC->BCpos;
                printf("BC----2>Person %d moved along BC from %d to %d\n",infoBC->ID,prevPos,infoBC->BCpos);
                BCCounter = 0;
                pthread_mutex_unlock(&mutex1);
            }
        }
        else{
            //pthread_mutex_lock(&mutex1);
            prevLocalBCCounter = localBCCounter;
            bcArray[prevLocalBCCounter].ID = 0;
            bcArray[prevLocalBCCounter].exist = false;
            bcArray[prevLocalBCCounter].pos = 0;
            localBCCounter = limit;
            BCCounter += limit;
            infoBC->BCpos = limit;
            //infoBC->BCpos += prevPos;
            if(localBCCounter >= STEPS_BC){
                infoBC->BCpos = STEPS_BC;
                printf("BC----3>Person %d moved along BC from %d to %d\n",infoBC->ID,prevPos,infoBC->BCpos);
                printf("BC----5>Person %d reached from B to C\n",infoBC->ID);
                BCCounter = 0;
                pthread_mutex_unlock(&mutex1);
                break;
            }
            else{
                bcArray[localBCCounter].ID = infoBC->ID;
                bcArray[localBCCounter].exist = true;
                bcArray[localBCCounter].pos = infoBC->BCpos;
                printf("BC----6>Person %d moved along BC from %d to %d\n",infoBC->ID,prevPos,infoBC->BCpos);
                BCCounter = 0;
                pthread_mutex_unlock(&mutex1);
            }

        }
        sleep(1);

    }
    functionCD1(infoBC);
    //functionDD(infoBC);
}
int leftWaiting = 0;
int rightWaiting = 0;
int left_in_bridge = 0;
int right_in_bridge = 0;
int count = 0;
void* functionCD1(void* arg){
    struct Persons *infoCD = (struct Persons *) arg;
    int i,j,limit = 0,localCDCounter = 0,prevLocalCDCounter = 0;
    //float stepNumberCD = ceil((float)STEPS_CD/(float)infoCD->personSpeed);
    //int lastStepSize = STEPS_CD % infoCD->personSpeed;
    int k;
    //int pos;
    //int outOfcdArray = 0;
    int * p = (int*) malloc(sizeof(int));
    sem_getvalue(&structure.right,p);
    count = *p;
    while(count != STEPS_CD){
        sem_getvalue(&structure.right,p);
        count = *p;
    }
    sem_wait(&structure.left);
    for(i = 0; i < STEPS_CD; i++){
        //searching for blocker
        //pthread_mutex_lock(&mutex1);
        int blocker = 0;
        pthread_mutex_lock(&mutex1);
        /*printf("CD ARRAY:\n");
        for(k = 0; k < STEPS_BC;k++){
            printf("ID: %d EXIST: %d POS: %d\n",cdArray[k].ID,cdArray[k].exist,cdArray[k].pos);
        }*/
        int prevPos = infoCD->CDpos;

        for(j = infoCD->CDpos; j < infoCD->CDpos + infoCD->personSpeed; j++){
            if(cdArray[j].exist && cdArray[j].ID != infoCD->ID){
                //printf("ID: %d EXIST:%d POS: %d\n",cdArray[j].ID,cdArray[j].exist,cdArray[j].pos);
                limit = j - 1;
                blocker = 1;
                //printf("<<<<<Person %d  %d>>>>>\n",infoCD->ID,j);
                break;
            }
        }
        //pthread_mutex_unlock(&mutex1);
        if(blocker != 1){
            //pthread_mutex_lock(&mutex1);
            prevLocalCDCounter = localCDCounter;
            cdArray[prevLocalCDCounter].ID = 0;
            cdArray[prevLocalCDCounter].exist = false;
            cdArray[prevLocalCDCounter].pos = 0;
            localCDCounter += infoCD->personSpeed;
            CDCounter += infoCD->personSpeed;
            infoCD->CDpos += infoCD->personSpeed;
            if(infoCD->CDpos >= STEPS_CD){
                infoCD->CDpos = STEPS_CD;
                //printf(">>>>>>>Local_CD_Counter<<<<<<<%d\n",localCDCounter);
                printf("CD----1>Person %d moved along CD from %d to %d\n",infoCD->ID,prevPos,infoCD->CDpos);
                printf("CD----4>Person %d reached from C to D\n",infoCD->ID);
                CDCounter = 0;
                pthread_mutex_unlock(&mutex1);
                break;
            }
            else{
                cdArray[localCDCounter].ID = infoCD->ID;
                cdArray[localCDCounter].exist = true;
                cdArray[localCDCounter].pos = infoCD->CDpos;
                printf("CD----2>Person %d moved along CD from %d to %d\n",infoCD->ID,prevPos,infoCD->CDpos);
                CDCounter = 0;
                pthread_mutex_unlock(&mutex1);
            }
        }
        else{
            //pthread_mutex_lock(&mutex1);
            prevLocalCDCounter = localCDCounter;
            //printf("prev %d\n",prevLocalCDCounter);
            cdArray[prevLocalCDCounter].ID = 0;
            cdArray[prevLocalCDCounter].exist = false;
            cdArray[prevLocalCDCounter].pos = 0;
            localCDCounter = limit;
            CDCounter += limit;
            infoCD->CDpos = limit;
            if(localCDCounter >= STEPS_CD){
                infoCD->CDpos = STEPS_CD;
                printf("CD----3>Person %d moved along CD from %d to %d\n",infoCD->ID,prevPos,infoCD->CDpos);
                printf("CD----5>Person %d reached from C to D\n",infoCD->ID);
                CDCounter = 0;
                pthread_mutex_unlock(&mutex1);
                break;
            }
            else{
                //printf("LocalCD %d\n",localCDCounter);
                cdArray[localCDCounter].ID = infoCD->ID;
                cdArray[localCDCounter].exist = true;
                cdArray[localCDCounter].pos = infoCD->CDpos;
                printf("CD----6>Person %d moved along CD from %d to %d\n",infoCD->ID,prevPos,infoCD->CDpos);
                CDCounter = 0;
                pthread_mutex_unlock(&mutex1);
            }

        }
        //pthread_mutex_unlock(&mutex1);
        sleep(1);

    }
    sem_post(&structure.left);
    functionDD(infoCD);
}

void* functionDC1(void* arg){
    struct Persons *infoDC = (struct Persons *) arg;
    infoDC->CDpos = STEPS_CD;
    int i,j,limit = 0,localDCCounter = STEPS_CD,prevLocalDCCounter = STEPS_CD;
    int * p = (int*)malloc(sizeof(int));
    sem_getvalue(&structure.left,p);
    count = *p;
    int k;
    while(count != STEPS_CD){
        sem_getvalue(&structure.left,p);
        count = *p;
    }
    sem_wait(&structure.right);
    for(i = 0; i < STEPS_CD; i++){
        //searching for blocker
        int blocker = 0;
        pthread_mutex_lock(&mutex1);
        /*printf("DC ARRAY: person %d\n",infoDC->ID);
        for(k = 0; k < STEPS_CD; k++){
            printf("ID: %d EXIST:%d POS: %d\n",cdArray[k].ID,cdArray[k].exist,cdArray[k].pos);
        }*/
        int prevPos = infoDC->CDpos;
        for(j = infoDC->CDpos - 1; j >= infoDC->CDpos - infoDC->personSpeed; j--){
            if(cdArray[j].exist && cdArray[j].ID != infoDC->ID){
                limit = j + 1;
                blocker = 1;
                break;
            }
        }
        if(blocker != 1){
            //pthread_mutex_lock(&mutex1);
            prevLocalDCCounter = localDCCounter;
            cdArray[prevLocalDCCounter].ID = 0;
            cdArray[prevLocalDCCounter].exist = false;
            cdArray[prevLocalDCCounter].pos = 0;
            localDCCounter -= infoDC->personSpeed;
            DCCounter -= infoDC->personSpeed;
            infoDC->CDpos -= infoDC->personSpeed;
            if(infoDC->CDpos <= 0){
                infoDC->CDpos = 0;
                //printf(">>>>>>>Local_CD_Counter<<<<<<<%d\n",localDCCounter);
                printf("DC----1>Person %d moved along DC from %d to %d\n",infoDC->ID,prevPos,infoDC->CDpos);
                printf("DC----4>Person %d reached from D to C\n",infoDC->ID);
                DCCounter = STEPS_CD;
                pthread_mutex_unlock(&mutex1);
                break;
            }
            else{
                cdArray[localDCCounter].ID = infoDC->ID;
                cdArray[localDCCounter].exist = true;
                cdArray[localDCCounter].pos = infoDC->CDpos;
                printf("DC----2>Person %d moved along DC from %d to %d\n",infoDC->ID,prevPos,infoDC->CDpos);
                DCCounter = STEPS_CD;
                pthread_mutex_unlock(&mutex1);
            }
        }
        else{
            //pthread_mutex_lock(&mutex1);
            prevLocalDCCounter = localDCCounter;
            cdArray[prevLocalDCCounter].ID = 0;
            cdArray[prevLocalDCCounter].exist = false;
            cdArray[prevLocalDCCounter].pos = 0;
            localDCCounter = limit;
            DCCounter -= limit;
            infoDC->CDpos = limit;
            if(localDCCounter <= 0){
                infoDC->CDpos = 0;
                printf("DC----3>Person %d moved along DC from %d to %d\n",infoDC->ID,prevPos,infoDC->CDpos);
                printf("DC----5>Person %d reached from D to C\n",infoDC->ID);
                DCCounter = STEPS_CD;
                pthread_mutex_unlock(&mutex1);
                break;
            }
            else{
                cdArray[localDCCounter].ID = infoDC->ID;
                cdArray[localDCCounter].exist = true;
                cdArray[localDCCounter].pos = infoDC->CDpos;
                printf("DC----6>Person %d moved along DC from %d to %d\n",infoDC->ID,prevPos,infoDC->CDpos);
                BCCounter = STEPS_CD;
                pthread_mutex_unlock(&mutex1);
            }

        }
        sleep(1);
    }
    sem_post(&structure.right);
    functionCB(infoDC);
}
void *functionCB(void *arg) {
    struct Persons *infoBC = (struct Persons *) arg;
    infoBC->BCpos = STEPS_BC;
    int k;
    int i, j, limit = 0, localCBCounter = STEPS_BC, prevCBCounter = STEPS_BC;
    for (i = 0; i < STEPS_BC; i++) {
        //searching for blocker
        int blocker = 0;
        pthread_mutex_lock(&mutex1);
        /*printf("CB ARRAY: person %d\n",infoBC->ID);
        for(k = 0; k < STEPS_BC;k++){
            printf("ID: %d EXIST: %d POS: %d\n",cbArray[k].ID,cbArray[k].exist,cbArray[k].pos);
        }*/
        int prevPos = infoBC->BCpos;
        for (j = infoBC->BCpos - 1; j >= infoBC->BCpos - infoBC->personSpeed; j--) {
            if (cbArray[j].exist && cbArray[j].ID != infoBC->ID) {
                limit = j + 1;
                blocker = 1;
                break;
            }
        }
        if (blocker != 1) {
            //pthread_mutex_lock(&mutex1);
            prevCBCounter = localCBCounter;
            cbArray[prevCBCounter].ID = 0;
            cbArray[prevCBCounter].exist = false;
            cbArray[prevCBCounter].pos = 0;
            localCBCounter -= infoBC->personSpeed;
            CBCounter -= infoBC->personSpeed;
            infoBC->BCpos -= infoBC->personSpeed;
            if (infoBC->BCpos <= 0) {
                infoBC->BCpos = 0;
                printf("CB----1>Person %d moved along CB from %d to %d\n", infoBC->ID, prevPos, infoBC->BCpos);
                printf("CB----4>Person %d reached from C to B\n", infoBC->ID);
                CBCounter = STEPS_BC;
                pthread_mutex_unlock(&mutex1);
                break;
            }
            else {
                cbArray[localCBCounter].ID = infoBC->ID;
                cbArray[localCBCounter].exist = true;
                cbArray[localCBCounter].pos = infoBC->BCpos;
                printf("CB----2>Person %d moved along CB from %d to %d\n", infoBC->ID, prevPos, infoBC->BCpos);
                CBCounter = STEPS_BC;
                pthread_mutex_unlock(&mutex1);
            }
        }
        else {
            //pthread_mutex_lock(&mutex1);
            prevCBCounter = localCBCounter;
            cbArray[prevCBCounter].ID = 0;
            cbArray[prevCBCounter].exist = false;
            cbArray[prevCBCounter].pos = 0;
            localCBCounter = limit;
            CBCounter -= limit;
            infoBC->BCpos = limit;
            if (localCBCounter <= 0) {
                infoBC->BCpos = 0;
                printf("CB----3>Person %d moved along CB from %d to %d\n", infoBC->ID, prevPos, infoBC->BCpos);
                printf("CB----5>Person %d reached from C to B\n", infoBC->ID);
                CBCounter = STEPS_BC;
                pthread_mutex_unlock(&mutex1);
                break;
            }
            else {
                cbArray[localCBCounter].ID = infoBC->ID;
                cbArray[localCBCounter].exist = true;
                cbArray[localCBCounter].pos = infoBC->BCpos;
                printf("CB----6>Person %d moved along CB from %d to %d\n", infoBC->ID, prevPos, infoBC->BCpos);
                CBCounter = STEPS_BC;
                pthread_mutex_unlock(&mutex1);
            }

        }
        sleep(1);

    }
    functionBA(infoBC);
}

void *functionDD(void *arg) {
    struct Persons *infoDD = (struct Persons *) arg;
    int i;
    infoDD->distanceCrossed = 0;
    float stepNumberDD = ceil((float) STEPS_DD / (float) infoDD->personSpeed);
    int lastStepSize = STEPS_DD % infoDD->personSpeed;
    int prevPos = infoDD->distanceCrossed;
    for (i = 0; i < (int) stepNumberDD; i++) {
        if ((i == (int) stepNumberDD - 1) && lastStepSize != 0) {
            prevPos = infoDD->distanceCrossed;
            infoDD->distanceCrossed += lastStepSize;
            printf("DD----1>Person %d moved along DD from %d to %d\n", infoDD->ID, prevPos,
                   infoDD->distanceCrossed);
            sleep(1);
        }
        else {
            prevPos = infoDD->distanceCrossed;
            infoDD->distanceCrossed += infoDD->personSpeed;
            printf("DD----2>Person %d moved along DD from %d to %d\n", infoDD->ID, prevPos,
                   infoDD->distanceCrossed);
            sleep(1);
        }
    }
    if (infoDD->distanceCrossed == STEPS_DD) {
        infoDD->distanceCrossed = 0;
        printf("DD----3>Person %d reached from D to D\n", infoDD->ID);
        functionDC1(infoDD);
    }
}

int main() {
    int j, k;
    int processes = fileRead();

    sem_init(&structure.mutex,0, STEPS_CD);
    sem_init(&structure.left, 0, STEPS_CD);
    sem_init(&structure.right, 0, STEPS_CD);

    for (k = 0; k < STEPS_BC; k++) {
        bcArray[k].ID = cdArray[k].ID = cbArray[k].ID = 0;
        bcArray[k].exist = cdArray[k].exist = cbArray[k].exist = false;
        bcArray[k].pos = cdArray[k].pos = cbArray[k].pos = 0;
    }
    struct Persons infoPerson[processes];
    pthread_t threads[processes];
    for (j = 0; j < processes; j++) {
        infoPerson[j].ID = j + 1;
        infoPerson[j].personSpeed = speed[j];
        infoPerson[j].distanceCrossed = 0;
        infoPerson[j].BCpos = 0;
        infoPerson[j].CDpos = 0;
        pthread_create(&threads[j], NULL, functionAB, &infoPerson[j]);
        sleep(1);
    }
    int i;
    for(i = 0; i < processes; i++){
        pthread_join(threads[i],NULL);
    }
    while (1);
}
