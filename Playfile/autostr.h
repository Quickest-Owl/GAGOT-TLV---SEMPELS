/******************************************************************************\
 Library     :  Utils / Common
 Filename    :  autostr.h
 Purpose     :  Global string allocator for prints/debug purpose
 Author      :  (c) RedSoft, Sergey Krasnitsky
 Created     :  3.9.2009
 Note		 :  See also AUTOSTR & STRING classes in str.h
 Important	 :  FOR LOGGING/DEBUG USAGE ONLY! NOT MULTITHREAD PROTECTED 
\******************************************************************************/

#ifndef _AUTOSTR_H
#define _AUTOSTR_H

#ifdef __cplusplus
extern "C" {
#endif


enum {
    AUTOSTR_MAXSTRS = UTILS_AUTOSTR_CYCLBUF_SIZE,
    AUTOSTR_MAXLEN  = UTILS_AUTOSTR_MAXLEN
};


char* autostrGet();


#ifdef __cplusplus
}
#endif

#endif // _AUTOSTR_H
