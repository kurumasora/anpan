#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "breadmachine.h"

static  int     status;
static  int     state;

static void delay(int milliseconds) {
    usleep(milliseconds * 1000); // usleepはマイクロ秒単位
}

/**
extern int  ThrowBread (void) 
・コンベア上にあるパンをアンパンストッカーに搬⼊する 
・戻り値     正常終了時はゼロ
　　　　　　　異常発⽣時は666 
**/

extern int  ThrowBread ( void )
{
    int i;
    int rtn=666;
    double pos;
    
    if( status == 0 )
    {
        pos = GetPosConveyor();
        if( (pos <= BR_STOCKER_MAX) && (pos >= BR_STOCKER_MIN ) )
        {
                status = 1;
                delay( 3000 );      // Waiting for pseudo working time
                status = 0;
                rtn = 0;
        }
    }
    return( rtn );
}
