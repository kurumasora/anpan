#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

/**
extern int  SetDough2(void) 
・コンベア上にあるパン⽣地を窯１に搬⼊する 
・戻り値 正常終了時はゼロ 異常発⽣時は666 
extern int  StartBake2( int param ) 
・引数  param  0のとき，生生地を焼く． 0じゃないとき，生焼けのパンを再加熱. パン焼き窯2に弱火着⽕する 
・パン焼き窯2に着⽕する 
・戻り値 正常終了時はゼロ
異常発⽣時は666 
extern int  WaitBake2(void) 
・パン焼き窯2内のパンが焼き上がるまで待つ 
  本関数は焼き上がった時点で終了する 
・戻り値 正常終了時はゼロ 異常発⽣時は666，パンの生焼け時は10
extern int SetBread1(void) 
・パン焼き窯2内のパン（⽣地）を取り出し，コンベアに積載する． 
・戻り値 正常終了時はゼロ  異常発⽣時は666 
**/

extern int  SetDough2( void ); 
extern int  StartBake2( int param ); 
extern int  WaitBake2( void ); 
extern int SetBread2( void ); 
