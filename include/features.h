#ifndef _FEATURES_H
#define _FEATURES_H

#define __GLIBC__ 2
#define __GLIBC_MINOR__ 40
#define __GLIBC_PREREQ(major, minor) ((major) <= __GLIBC__ && (minor) <= __GLIBC_MINOR__)

#undef __USE_XOPEN2K8

#ifdef _GNU_SOURCE
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE - 0) >= 200809L
#define __USE_XOPEN2K8 1
#endif

#ifdef	_XOPEN_SOURCE
#define __USE_XOPEN 1
#if (_XOPEN_SOURCE - 0) >= 500
	#if (_XOPEN_SOURCE - 0) >= 600
		#if (_XOPEN_SOURCE - 0) >= 700
			#define __USE_XOPEN2K8 1
			#define __USE_XOPEN2K8XSI 1
		#endif
		#define __USE_XOPEN2K 1
	#endif
#endif
#endif

#ifdef _GNU_SOURCE
#define __USE_GNU 1
#endif

#endif
