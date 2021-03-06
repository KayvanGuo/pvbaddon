//***************************************************************************
//                          main.cpp  -  description
//                             -------------------
//  begin            : Do. Dez 27 11:02:01 2012
//  generated by     : pvdevelop (C) Lehrig Software Engineering
//  email            : lehrig@t-online.de
//***************************************************************************
#include "pvapp.h"
#include "rlthread.h"     // include this header for thread support
rlThread mythread;        // define a thread instance
#include <wiringPi.h>     // include GPIO
#include <gertboard.h>    // include GPIO

char gpio[32];
static char *pinNames[] =
{
  "GPIO 0",
  "GPIO 1",
  "GPIO 2",
  "GPIO 3",
  "GPIO 4",
  "GPIO 5",
  "GPIO 6",
  "GPIO 7",
  "SDA   ",
  "SCL   ",
  "CE0   ",
  "CE1   ",
  "MOSI  ",
  "MISO  ",
  "SCLK  ",
  "TxD   ",
  "RxD   ",
  "GPIO 8",
  "GPIO 9",
  "GPIO10",
  "GPIO11",
};


int piWriteGPIO(int pin, int val)
{
  mythread.lock();    // do something critical
  //printf("writeGPIO(%d,%d)\n", pin, val);
  if(val) digitalWrite(pin,HIGH);
  else    digitalWrite(pin,LOW);
  mythread.unlock();
  return val;
}

void *myThread(void *arg) // define your thread function
{
  if(arg == NULL) return NULL;
  THREAD_PARAM  *p = (THREAD_PARAM *) arg;
  while(p->running)
  {
    for(int pin=0; pin<32; pin++)
    {
      p->thread->lock();    // do something critical
      if(digitalRead(pin) == HIGH) gpio[pin] = 1;
      else                         gpio[pin] = 0;
      p->thread->unlock();
      //printf("gpio[%d]=%d ",pin,gpio[pin]);
      //printf("gpio[%02d]=%d %s is wired to %d\n",pin,gpio[pin],pinNames[pin],wpiPinToGpio(pin));
    } 
    //printf("\n");
    rlsleep(50);        // goto sleep for 1 second
  }
  return arg;
}

int pvMain(PARAM *p)
{
int ret;

  pvSendVersion(p);
  pvSetCaption(p,"Raspberry Pi GPIO port state");
  pvResize(p,0,1280,1024);
  //pvScreenHint(p,1024,768); // this may be used to automatically set the zoomfactor
  ret = 1;
  pvGetInitialMask(p);
  if(strcmp(p->initial_mask,"mask1") == 0) ret = 1;

  while(1)
  {
    if(trace) printf("show_mask%d\n", ret);
    switch(ret)
    {
      case 1:
        pvStatusMessage(p,-1,-1,-1,"mask1");
        ret = show_mask1(p);
        break;
      default:
        return 0;
    }
  }
}

#ifdef USE_INETD
int main(int ac, char **av)
{
PARAM p;

  pvInit(ac,av,&p);
  /* here you may interpret ac,av and set p->user to your data */
  pvMain(&p);
  return 0;
}
#else  // multi threaded server
int main(int ac, char **av)
{
PARAM p;
int   s;

  pvInit(ac,av,&p);
  if(wiringPiSetup() == -1)
  {
    fprintf(stderr, "Unable to initialise wiringPi mode\n");
    exit (1) ;
  }
  for(int pin=0; pin<=25; pin++)
  {
    pinMode(pin,OUTPUT);
  }
  
  memset(gpio,0,sizeof(gpio));
  mythread.create(myThread,NULL); // start your thread here
  /* here you may interpret ac,av and set p->user to your data */
  while(1)
  {
    s = pvAccept(&p);
    if(s != -1) pvCreateThread(&p,s);
    else        break;
  }
  return 0;
}
#endif
