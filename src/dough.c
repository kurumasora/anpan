#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "breadmachine.h"

static  int     stocker[12];
static  int     CountDough;
static  int     status;

static void delay(int milliseconds) {
    usleep(milliseconds * 1000); // usleepはマイクロ秒単位
}

extern  int  GetDoughCount(void)
{
    return( CountDough );
}

extern  int InitDough( void )
{
    int     i;
    
    status = -2;                         // 初期化中
    CountDough = 0;                     // パン生地数
    srand((unsigned int)time(NULL));    // 乱数の初期化
    
    for( i=0; i< 12; i++ )
    {
        if( (rand() % 10) == 0 )
        {
            stocker[i] = 0;             // パン生地なし
        }
        else
        {
            stocker[i] = 1;             // パン生地あり
            CountDough++;
        }
    }
    delay( 5000 );                      // パン生地を休ませる
    status = 0;
    return( 0 );
}

extern int  GetOneDough( void )
{
    int i;
    int rtn=666;
    double pos;
    
    if( status != 0 ) return( rtn );
    
    pos = GetPosConveyor();
    if( (pos <= DO_STOCKER_MAX) && (pos >= DO_STOCKER_MIN ) )
    {
            status = -1;                         // 動作中
            for( i=0; i< 12; i++ )
            {
                if( stocker[i] != 0  )
                {
                    delay( 3000 );              // Pseudo working time
                    stocker[i] = 0;             // パン生地なし
                    CountDough--;
                    rtn = 0;
                    break;
                }
            }
            status = 0;
    }
    return( rtn );
}
