
#include "cifxlinux.h"
#include "cifXEndianess.h"
#include "Hil_Packet.h"
#include "Hil_SystemCmd.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <pthread.h>


int main(){
    printf("hello playground\n");
    char val[30];
    strcpy(val,"-1234");
    int input = atoi(val);
    printf("input: %d\n",input);

    
    int p1,p2,p3,p4;
    int input_abs=input;
    if(input<0){input_abs=-input;}
    p1 = input_abs%256;
    p2 = input_abs/256%256;
    p3 = input_abs/256/256%256;
    p4 = input_abs/256/256/256%256;
    if(input<0){
        p1=255-p1;
        p2=255-p2;
        p3=255-p3;
        p4=255-p4;
    }
    printf("p1,p2,p3,p4: %d %d %d %d\n",p1,p2,p3,p4);
    return 0;
}