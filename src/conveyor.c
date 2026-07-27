#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "breadmachine.h"

static  double  torr = 0.02;

static  double  a0 = (double)0.07859368981489481;
static  double  b1 = (double)-1.717726878071494;
static  double  b2 = (double)0.7963205678863887;
static  int     T = (int)100;

static  double  varz[2];
static  double  rs;
static  double  y;
static  double  u;

static  double  LimitArea[2];
//static  double  torr = 0.01;
static  int     errSts;
static  int     state;

static void delay(int milliseconds) {
    usleep(milliseconds * 1000); // usleepはマイクロ秒単位
}

extern  double  GetPosConveyor(void)
{
    return( y + ((int)(rand() % 10))*torr );
}

extern  int InitConveyor( void )
{
    varz[0]=varz[1]=0.0;
    u=0.0;
    LimitArea[0] = 0.0;
    LimitArea[1] = 3.5;
    errSts = 0;
    state = 0;

	srand((unsigned int)time(NULL));  // パン焼き窯用の乱数シードを設定（無理やりだけれど...）

    return( 0 );
}

extern double  StartConveyor( double target )
{
    int sts = -1;
    int count = 0;
    
    if( state != 0 )
    {
        return( -666.0 );
    }
    else
    {
        state = -1;     // 動作中
    }
    
    rs = target;
    if( errSts == 0 )
    {
        do {
            y = a0*u;
            if( y < LimitArea[0] ) break;
            if( y > LimitArea[1] ) break;
            if( fabs(rs - y) <= torr ) count++;
            if( count >= 5 )
            {
                sts=0;
                break;
            }
            u=rs-b1*varz[0]-b2*varz[1];
            varz[1]=varz[0];
            varz[0]=u;
            delay(T);    // Wait, 100ms
        } while( -1 );
    }
    state = 0;  // 動作終了
    if( sts == 0 )
    {
        return( y + ((int)(rand() % 10))*torr );
    }
    else
    {
        errSts = -1;
        return( -666.0 );
    }
}
