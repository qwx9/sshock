#ifdef __plan9__
#ifndef __PLAN9_H
#define __PLAN9_H

#ifndef	__FUNCTION__
#define	__S1(x)	#x
#define	__S2(x)	__S1(x)
#define __FUNCTION__	__S2(__func__) ": " __FILE__ ":" __S2(__LINE__)
#endif

#endif
#endif
