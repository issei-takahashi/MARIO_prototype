#pragma once



using namespace std;

#include <Windows.h>
#include <string>

#define debugMes(title,text) MessageBox(0,TEXT(title),TEXT(text.c_str()),MB_OK)
#define passert(text,exp) if((exp)==0){cout<<text<<endl;;assert(exp);}
#define foreach(it,container) for(auto (it)=(container).begin();(it)!=(container).end();(it)++)

/* private */
#define capcelled__ private
#define inner__     private
#define outofuse__  private

/* public */
#define interface__ public
#define open__      public
#define virtual2__  public

/* protected */
#define virtual1__  protected
#define virtual0__  protected
#define strategy__ protected

#define act__ void
#define event__ bool
#define st__ void

#define intc    int const
#define doublec double const
#define floatc  float const
#define stringc string const

#define SIZE(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))
#define ZERO(ARRAY) for(int III=0;III<SIZE(ARRAY);III++){ARRAY[III]=0;}
#define NZERO(ARRAY,N) for(int III=0;III<N;III++){ARRAY[III]=0;}
#define VZERO(VECTOR,N,ZERO) (VECTOR).clear();for(int III=0;III<N;III++){(VECTOR).push_back(ZERO);}
#define MINSERT(MAP,KEY,VAR) (MAP).insert(std::make_pair((KEY),(VAR)))
#define MERASE(MAP,KEY) auto ITER=(MAP).find(KEY);assert(ITER!=(MAP).end());(MAP).erase(ITER); 
#define FOR(I,BEGIN,END) for(int I=BEGIN;I<END;I++)
#define PEAK(VAR,MAX) if((VAR)>(MAX)){(VAR)=(MAX);}

#define ef else if
#define el else
#define ifn(EXP) if(!(EXP))
#define efn(EXP) ef(!(EXP))
#define sp shared_ptr
#define re_cast reinterpret_cast

// 矢印キー
#define  CV_WAITKEY_CURSORKEY_TOP     (2490368)
#define  CV_WAITKEY_CURSORKEY_BOTTOM  (2621440)
#define  CV_WAITKEY_CURSORKEY_RIGHT   (2555904)
#define  CV_WAITKEY_CURSORKEY_LEFT    (2424832)
// エンターキーとか
#define  CV_WAITKEY_ENTER             (13)
#define  CV_WAITKEY_ESC               (27)
#define  CV_WAITKEY_SPACE             (32)
#define  CV_WAITKEY_TAB               (9)
