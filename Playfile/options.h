/**********************************************************************\
 Library     :  Utils / Common
 Filename    :  Common/options.h
 Purpose     :  Default options for Common Utils and DS
 Author      :  (c) RedSoft, Sergey Krasnitsky

 Description :
  All Common Utils & DS compilation flags.
  Note: a user can override this definitions by use of custom option.h,
  its search path must be specified as first in all involved makefiles.
\**********************************************************************/

#ifndef _OPTIONS_H
#define _OPTIONS_H


/*
 * Length of auto-allocated strings (AUTOSTR class or autostrGet func, STRING type)
 */
#ifndef UTILS_AUTOSTR_MAXLEN
#define UTILS_AUTOSTR_MAXLEN			512
#endif


/*
 * Auto-allocated strings Cyclic buffer size
 */
#ifndef UTILS_AUTOSTR_CYCLBUF_SIZE
#define UTILS_AUTOSTR_CYCLBUF_SIZE		512
#endif



#endif /* _OPTIONS_H */
