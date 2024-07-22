#ifndef _LIBINTL_H
#define _LIBINTL_H

#include <bits/utils.h>

__begin

char* textdomain(const char* __domain_name);
char* bindtextdomain(const char* __domain_name, const char* __dir_name);
char* gettext(const char* __msg_id);
char* dcgettext(const char* __domain_name, const char* __msg_id, int __category);

char* ngettext(const char* __msg_id, const char* __msg_id_plural, unsigned long __n);

__end

#endif
