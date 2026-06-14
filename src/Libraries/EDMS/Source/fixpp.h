#ifndef __FIXPP_H
#define __FIXPP_H

#include "fix.h"

typedef struct Q Q;
struct Q{
	fix val;
};

#ifdef FIXPOINT_SHIFTUP
#define SHIFTUP FIXPOINT_SHIFTUP
#else
#define SHIFTUP 16 // 16:16 default format.
#endif

#define SHIFTMULTIPLIER (1 << SHIFTUP)

static inline Q
Q_(int32_t l)
{
    Q f;
    f.val = l;
    return f;
}

#define f2Fixpoint(x) (Q_((int32_t)((x)*SHIFTMULTIPLIER)))

static inline Q
Q_neg(Q a)
{
	a.val = -a.val;
	return a;
}

static inline Q
Q_add(Q a, Q b)
{
	a.val += b.val;
	return a;
}

static inline Q
Q_sub(Q a, Q b)
{
	a.val -= b.val;
	return a;
}

static inline Q
Q_mul(Q a, Q b)
{
    a.val = (int32_t)fix_mul((fix)a.val, (fix)b.val);
    return a;
}

static inline Q
Q_div(Q a, Q b)
{
    a.val = (int32_t)fix_div((fix)a.val, (fix)b.val);
    // a.val=_fix_do_div(a.val,b.val);
    return a;
}

static inline Q
Q_as_int(int32_t i)
{
	Q a;

	a.val = i << SHIFTUP;
	return a;
}

static inline Q
Q_as_double(double d)
{
	Q a;

    a.val = (int32_t)(d * SHIFTMULTIPLIER);
    return a;
}

static inline Q
Q_as_fixang(fixang f)
{
	Q a;

	a.val = ((int32_t)(int16_t)(f - 1)) + 1;
	a = Q_mul(a, f2Fixpoint(6.283185306));
	return a;
}

static inline Q
Q_as_fix(fix f)
{
	Q a;

	a.val = f;
	return a;
}

/* static initializers */
#define	Q_from_double(x)	{.val = (int32_t)((x) * SHIFTMULTIPLIER)}
#define	Q_from_int(x)	{.val = (x) << SHIFTUP}

static inline double
Q_to_double(Q a)
{
	return ((double)a.val) / SHIFTMULTIPLIER;
}

static inline float
Q_to_float(Q a)
{
	return ((float)a.val) / SHIFTMULTIPLIER;
}

static inline int32_t
Q_to_int(Q a)
{
	return (int32_t)(a.val >> SHIFTUP);
}

static inline fix
Q_to_fix(Q a)
{
	return (fix)a.val;
}

static inline fixang
Q_to_fixang(Q a)
{
	a = Q_mul(a, f2Fixpoint(0.159154943));
    // for temp, 360 degrees = 1.0.
    // The lower 16 bits of the internal rep is the fixang.
    return (uint16_t)a.val;
}

static inline Q
Q_sin(Q a)
{
    a.val = fix_sin(Q_to_fixang(a));
    return a;
}

static inline Q
Q_cos(Q a)
{
    a.val = fix_cos(Q_to_fixang(a));
    return a;
}

static inline Q
Q_acos(Q a)
{
	a = Q_as_fixang(fix_acos(Q_to_fix(a)));
    return a;
}

static inline Q
Q_asin(Q a)
{
    a = Q_as_fixang(fix_asin(Q_to_fix(a)));
    return a;
}

static inline void
Q_sincos(Q a, Q *sn, Q *cs)
{
    fix fsn, fcs;

    fix_sincos(Q_to_fixang(a), &fsn, &fcs);
    sn->val = fsn;
    cs->val = fcs;
}

static inline Q
Q_sqrt(Q a)
{
    a.val = fix_sqrt(a.val);
    return a;
}

static inline Q
Q_abs(Q a)
{
    a.val = labs(a.val);
    return a;
}

static inline int32_t
Q_floor(Q a)
{
	return a.val >> SHIFTUP;
}

#endif
