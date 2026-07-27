#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "breadmachine.h"

/**
extern int  SetDough2(void) 
・コンベア上にあるパン⽣地を窯2に搬⼊する 
・戻り値 正常終了時はゼロ 異常発⽣時は666 
extern int  StartBake2( int param ) 
・引数  param  0のとき，生生地を焼く． 0じゃないとき，生焼けのパンを再加熱. パン焼き窯2に弱火着⽕する 
・戻り値 正常終了時はゼロ
異常発⽣時は666 
extern int  WaitBake2(void) 
・パン焼き窯2内のパンが焼き上がるまで待つ 
  本関数は焼き上がった時点で終了する 
・戻り値 正常終了時はゼロ 異常発⽣時は666，パンの生焼け時は10
extern int SetBread2(void) 
・パン焼き窯2内のパン（⽣地）を取り出し，コンベアに積載する． 
・戻り値 正常終了時はゼロ  異常発⽣時は666 
**/

static  int     state=0;
static  int     status=0;

static void delay(int milliseconds) {
    usleep(milliseconds * 1000); // usleepはマイクロ秒単位
}

extern int  SetDough2(void) 
{
    int     rtn=666;
    
    if( status == 0 )
    {
        if( (GetPosConveyor() <= OVEN2_MAX) && (GetPosConveyor() >= OVEN2_MIN ) )
            {
                status = 1;     // Working
                if( state == 0 )
                {
                    state = 1;          // A dough is there in the oven2
                    delay( 2500 );      // Pseudo, wait for moving time
                    rtn=0;
                }
                status = 0;
            }
    }
    return( rtn );
}

extern int  StartBake2(int param) 
{
    int     rtn=666;
    
    if( status == 0 )
    {
        status = 1;     // Working
        if( state == 1 )
        {
            state = 2;     // A dough is there in the oven2
        	if( param == 0 )
        	{
                delay( 250 );    // An ignition
        		rtn=0;
        	}
        	else
        	{
               delay( 1500 );                // lightly toasting
               state = 4;
        	   if( (rand() % 100) > 95 )     // 確率5%で生焼け
        	      rtn=10;
               else
        		  rtn=0;
        	}
        }
        status = 0;
    }
    return( rtn );
}

extern int  WaitBake2(void) 
{
    int     rtn=666;

    if( status == 0 )
    {
        status = 1;     // Working
        if( state == 2 )
        {
            state = 3;     // A dough is there in the oven2
            delay( 40000 );    // Wait for baking bread
            state = 4;
        	if( (rand() % 100) > 85 )      // 確率15%で生焼け
        	   rtn=10;
            else
        	   rtn=0;
        }
        status = 0;
    }
    return( rtn );
}

extern int  SetBread2(void) 
{
    int     rtn=666;
    
    if( status == 0 )
    {
            if( (GetPosConveyor() <= OVEN2_MAX) && (GetPosConveyor() >= OVEN2_MIN ) )
                {
                    status = 1;     // Working
                    if( state == 4 )
                    {
                        state = 5;          // Moving a bread onto conveyor
                        delay( 4000 );      // Wait for baking bread
                        state = 0;
                        rtn=0;
                    }
                    status = 0;
                }
    }
    return( rtn );
}

