#ifndef _WCHAR_H
#define _WCHAR_H

#include <bits/utils.h>
#include <bits/mbstate_t.h>

__begin

typedef __WINT_TYPE__ wint_t;

#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
#endif

int mbsinit(const mbstate_t* __ps);

// posix
int wcwidth(wchar_t __ch);

__end

#endif
