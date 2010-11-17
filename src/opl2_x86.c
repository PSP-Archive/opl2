#include "opl2_int.h"
#include "opl2_vm.h"

#include <math.h>

/******************************************************************************/

void pl2MultMatrix4f_SSE(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b)
{
/*
R = [[ Axx*Bxx+Ayx*Bxy+Azx*Bxz+Awx*Bxw, ... ]]
*/
    asm volatile(
        "movaps  0 %2, %%xmm4\n"
        "movaps 16 %2, %%xmm5\n"
        "movaps 32 %2, %%xmm6\n"
        "movaps 48 %2, %%xmm7\n"

        "pshufd $0x00,  0 %1, %%xmm0\n"
        "pshufd $0x55,  0 %1, %%xmm1\n"
        "pshufd $0xaa,  0 %1, %%xmm2\n"
        "pshufd $0xff,  0 %1, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0,  0 %0\n"

        "pshufd $0x00, 16 %1, %%xmm0\n"
        "pshufd $0x55, 16 %1, %%xmm1\n"
        "pshufd $0xaa, 16 %1, %%xmm2\n"
        "pshufd $0xff, 16 %1, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0, 16 %0\n"

        "pshufd $0x00, 32 %1, %%xmm0\n"
        "pshufd $0x55, 32 %1, %%xmm1\n"
        "pshufd $0xaa, 32 %1, %%xmm2\n"
        "pshufd $0xff, 32 %1, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0, 32 %0\n"

        "pshufd $0x00, 48 %1, %%xmm0\n"
        "pshufd $0x55, 48 %1, %%xmm1\n"
        "pshufd $0xaa, 48 %1, %%xmm2\n"
        "pshufd $0xff, 48 %1, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0, 48 %0\n"

        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2MultMatrix4f_NoSSE(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b)
{
    fmatrix4_t c;
# define MMUL(i,j) \
    c.i.j = a->i.x * b->x.j + a->i.y * b->y.j + a->i.z * b->z.j + a->i.w * b->w.j
    MMUL(x,x); MMUL(y,x); MMUL(z,x); MMUL(w,x);
    MMUL(x,y); MMUL(y,y); MMUL(z,y); MMUL(w,y);
    MMUL(x,z); MMUL(y,z); MMUL(z,z); MMUL(w,z);
    MMUL(x,w); MMUL(y,w); MMUL(z,w); MMUL(w,w);
# undef MMUL
    *out = c;
}

void pl2VectorTransform4f_SSE(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v)
{
    asm volatile(
        "movaps  0 %1, %%xmm4\n"
        "movaps 16 %1, %%xmm5\n"
        "movaps 32 %1, %%xmm6\n"
        "movaps 48 %1, %%xmm7\n"
#if 1
        "pshufd $0x00, 0 %2, %%xmm0\n"
        "pshufd $0x55, 0 %2, %%xmm1\n"
        "pshufd $0xaa, 0 %2, %%xmm2\n"
        "pshufd $0xff, 0 %2, %%xmm3\n"
#else
        "movaps 0 %2, %%xmm3\n"
        "pshufd $0x00, %%xmm3, %%xmm0\n"
        "pshufd $0x55, %%xmm3, %%xmm1\n"
        "pshufd $0xaa, %%xmm3, %%xmm2\n"
        "pshufd $0xff, %%xmm3, %%xmm3\n"
#endif
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0,  0 %0\n"
        :"=m"(*out)
        :"m"(*m), "m"(*v)
    );
}

void pl2VectorTransform4f_NoSSE(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v)
{
    fvector4_t u;
# define VTFM(i) \
    u.i = m->x.i * v->x + m->y.i * v->y + m->z.i * v->z + m->w.i*v->w
    VTFM(x); VTFM(y); VTFM(z); VTFM(w);
# undef VTFM
    *out = u;
}

void pl2TransposeMatrix4f_SSE(fmatrix4_t *out, const fmatrix4_t *m)
{
    asm volatile(
        "movaps  0 %1, %%xmm0\n"
        "movaps 16 %1, %%xmm1\n"
        "movaps 32 %1, %%xmm2\n"
        "movaps 48 %1, %%xmm3\n"
        "movaps %%xmm0, %%xmm4\n"
        "movaps %%xmm2, %%xmm5\n"
        "movaps %%xmm0, %%xmm6\n"
        "movaps %%xmm2, %%xmm7\n"
        "shufps $0x44, %%xmm1, %%xmm4\n"
        "shufps $0x44, %%xmm1, %%xmm5\n"
        "shufps $0xee, %%xmm3, %%xmm6\n"
        "shufps $0xee, %%xmm3, %%xmm7\n"
        "movaps %%xmm4, %%xmm0\n"
        "movaps %%xmm4, %%xmm1\n"
        "movaps %%xmm6, %%xmm2\n"
        "movaps %%xmm6, %%xmm3\n"
        "shufps $0x88, %%xmm5, %%xmm0\n"
        "shufps $0xdd, %%xmm5, %%xmm1\n"
        "shufps $0x88, %%xmm7, %%xmm2\n"
        "shufps $0xdd, %%xmm7, %%xmm3\n"
        "movaps %%xmm0,  0 %0\n"
        "movaps %%xmm1, 16 %0\n"
        "movaps %%xmm2, 32 %0\n"
        "movaps %%xmm3, 48 %0\n"
        :"=m"(*out) :"m"(*m)
    );
}

void pl2TransposeMatrix4f_NoSSE(fmatrix4_t *out, const fmatrix4_t *m)
{
# define MTRAN(i,j) \
    n.i.j = m->j.i

    fmatrix4_t n;
    MTRAN(x,x); MTRAN(y,x); MTRAN(z,x); MTRAN(w,x);
    MTRAN(x,y); MTRAN(y,y); MTRAN(z,y); MTRAN(w,y);
    MTRAN(x,z); MTRAN(y,z); MTRAN(z,z); MTRAN(w,z);
    MTRAN(x,w); MTRAN(y,w); MTRAN(z,w); MTRAN(w,w);
    *out = n;

# undef MTRAN
}

void pl2VectorAdd4f_SSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    asm volatile(
        "movaps %1, %%xmm0\n"
        "movaps %2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorAdd4f_NoSSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
    out->w = a->w + b->w;
}

void pl2VectorSub4f_SSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    asm volatile(
        "movaps %1, %%xmm0\n"
        "movaps %2, %%xmm1\n"
        "subps  %%xmm1, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorSub4f_NoSSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
    out->w = a->w - b->w;
}

float pl2VectorDot4f_SSE(const fvector4_t *a, const fvector4_t *b)
{
    float r;
    asm volatile(
        "movaps %1, %%xmm0\n"
        "movaps %2, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "pshufd $0xb1, %%xmm0, %%xmm1\n"
        "addps %%xmm1, %%xmm0\n"
        "pshufd $0x1b, %%xmm0, %%xmm2\n"
        "addps %%xmm1, %%xmm0\n"
        "movss %%xmm0, %0\n"
        :"=m"(r) :"m"(*a), "m"(*b)
    );
    return r;
}

float pl2VectorDot4f_NoSSE(const fvector4_t *a, const fvector4_t *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

void pl2VectorScale4f_SSE(fvector4_t *out, const fvector4_t *v, float s)
{
    asm volatile(
        "movss  %2, %%xmm1\n"
        "movaps %1, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        //"pshufd $0, %2, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
}

void pl2VectorScale4f_NoSSE(fvector4_t *out, const fvector4_t *v, float s)
{
    out->x = v->x * s;
    out->y = v->y * s;
    out->z = v->z * s;
    out->w = v->w * s;
}

void pl2VectorScaleAdd4f_SSE(fvector4_t *out, const fvector4_t *v, float s)
{
    asm volatile(
        "movss  %2, %%xmm2\n"
        "movaps %1, %%xmm1\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "movaps %0, %%xmm0\n"
        //"pshufd $0, %2, %%xmm2\n"
        "mulps  %%xmm2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
}

void pl2VectorScaleAdd4f_NoSSE(fvector4_t *out, const fvector4_t *v, float s)
{
    out->x += v->x * s;
    out->y += v->y * s;
    out->z += v->z * s;
    out->w += v->w * s;
}

void pl2VectorTransScaleAdd4f_NoSSE(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v, float s)
{
    fvector4_t t;
    pl2VectorTransform4f_NoSSE(&t, m, v);
    pl2VectorScaleAdd4f_NoSSE(out, &t, s);
}

void pl2VectorTransScaleAdd4f_SSE(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v, float s)
{
    asm volatile(
        "movaps  0 %1, %%xmm4\n"
        "movaps 16 %1, %%xmm5\n"
        "movaps 32 %1, %%xmm6\n"
        "movaps 48 %1, %%xmm7\n"
#if 1
        "pshufd $0x00, 0 %2, %%xmm0\n"
        "pshufd $0x55, 0 %2, %%xmm1\n"
        "pshufd $0xaa, 0 %2, %%xmm2\n"
        "pshufd $0xff, 0 %2, %%xmm3\n"
#else
        "movaps 0 %2, %%xmm3\n"
        "pshufd $0x00, %%xmm3, %%xmm0\n"
        "pshufd $0x55, %%xmm3, %%xmm1\n"
        "pshufd $0xaa, %%xmm3, %%xmm2\n"
        "pshufd $0xff, %%xmm3, %%xmm3\n"
#endif
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "movss  %3, %%xmm5\n"
        "movaps %0, %%xmm4\n"
        "shufps $0, %%xmm5, %%xmm5\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "mulps  %%xmm5, %%xmm0\n"
        "addps  %%xmm4, %%xmm0\n"
        "movaps %%xmm0,  0 %0\n"
        :"=m"(*out)
        :"m"(*m), "m"(*v), "m"(s)
    );
}

void pl2VectorAdd3f_SSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    asm volatile(
        "movups %1, %%xmm0\n"
        "movups %2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorAdd3f_NoSSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void pl2VectorSub3f_SSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    asm volatile(
        "movups %1, %%xmm0\n"
        "movups %2, %%xmm1\n"
        "subps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorSub3f_NoSSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

float pl2VectorDot3f_SSE(const fvector3_t *a, const fvector3_t *b)
{
    float r;
    asm volatile(
        "movups %1, %%xmm0\n"
        "movups %2, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "pshufd $1, %%xmm0, %%xmm1\n"
        "pshufd $2, %%xmm0, %%xmm2\n"
        "addss %%xmm1, %%xmm0\n"
        "addss %%xmm2, %%xmm0\n"
        "movss %%xmm0, %0\n"
        :"=m"(r) :"m"(*a), "m"(*b)
    );
    return r;
}

float pl2VectorDot3f_NoSSE(const fvector3_t *a, const fvector3_t *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void pl2VectorCross3f_SSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    asm volatile(
        "movups %1, %%xmm0\n"
        "movups %2, %%xmm1\n"
        "pshufd $0xd2, %%xmm0, %%xmm2\n"
        "pshufd $0xc9, %%xmm1, %%xmm3\n"
        "pshufd $0xc9, %%xmm0, %%xmm0\n"
        "pshufd $0xd2, %%xmm1, %%xmm1\n"
        "mulps  %%xmm3, %%xmm2\n"
        "mulps  %%xmm1, %%xmm0\n"
        "subps  %%xmm2, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out) :"m"(*a), "m"(*b)
    );
}

void pl2VectorCross3f_NoSSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    fvector3_t c;
    c.x = a->y * b->z - a->z * b->y;
    c.y = a->z * b->x - a->x * b->z;
    c.z = a->x * b->y - a->y * b->x;
    *out = c;
}

void pl2VectorScale3f_SSE(fvector3_t *out, const fvector3_t *v, float s)
{
    asm volatile(
        "movss  %2, %%xmm1\n"
        "movups %1, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        //"pshufd $0, %2, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
}

void pl2VectorScale3f_NoSSE(fvector3_t *out, const fvector3_t *v, float s)
{
    out->x = v->x * s;
    out->y = v->y * s;
    out->z = v->z * s;
}

void pl2VectorScaleAdd3f_SSE(fvector3_t *out, const fvector3_t *v, float s)
{
    asm volatile(
        "movss  %2, %%xmm2\n"
        "movups %1, %%xmm1\n"
        "movups %0, %%xmm0\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        //"pshufd $0, %2, %%xmm1\n"
        "mulps  %%xmm2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
}

void pl2VectorScaleAdd3f_NoSSE(fvector3_t *out, const fvector3_t *v, float s)
{
    DEBUGPRINTIF(s != s, "%s: s is NaN!\n", __func__);
    out->x += v->x * s;
    out->y += v->y * s;
    out->z += v->z * s;
}

float pl2VectorLength3f_SSE(const fvector3_t *v)
{
    float r;
    asm volatile(
        "movups %1, %%xmm0\n"
        "movaps %%xmm0, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "pshufd $1, %%xmm0, %%xmm1\n"
        "pshufd $2, %%xmm0, %%xmm2\n"
        "addss %%xmm1, %%xmm0\n"
        "addss %%xmm2, %%xmm0\n"
        "sqrtss %%xmm0, %%xmm0\n"
        "movss %%xmm0, %0\n"
        :"=m"(r) :"m"(*v)
    );
    return r;
}

float pl2VectorLength3f_NoSSE(const fvector3_t *v)
{
    return sqrtf(pl2VectorDot3f(v, v));
}

void pl2VectorNormalize3f_SSE(fvector3_t *out, const fvector3_t *v)
{
    asm volatile(
        "movups %1, %%xmm0\n"
        "movaps %%xmm0, %%xmm1\n"
        "mulps  %%xmm0, %%xmm1\n"
        "pshufd $1, %%xmm1, %%xmm2\n"
        "pshufd $2, %%xmm1, %%xmm3\n"
        "addss %%xmm2, %%xmm1\n"
        "addss %%xmm3, %%xmm1\n"
        "rsqrtss %%xmm1, %%xmm1\n"
        "pshufd $0x55, %%xmm1, %%xmm1\n"
        "mulps %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out) :"m"(*v)
    );
}

void pl2VectorNormalize3f_NoSSE(fvector3_t *out, const fvector3_t *v)
{
    float len = pl2VectorLength3f(v);
    if(len) len = 1.0f / len;
    pl2VectorScale3f(out, v, len);
}

/* From:
http://listengine.tuxfamily.org/lists.tuxfamily.org/eigen/2009/03/msg00019.html
*/
void pl2QuatMultiply_SSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    static const uint32_t mask[] __attribute__((aligned(16))) =
        { 0x00000000, 0x00000000, 0x00000000, 0x80000000 };
    asm volatile(
        "movaps %2, %%xmm1\n"
        "movaps %1, %%xmm0\n"
        "pshufd $0xff, %%xmm1, %%xmm7\n"
        "pshufd $0x09, %%xmm1, %%xmm6\n"
        "pshufd $0x64, %%xmm1, %%xmm5\n"
        "pshufd $0x12, %%xmm0, %%xmm4\n"
        "pshufd $0x7f, %%xmm0, %%xmm3\n"
        "pshufd $0x89, %%xmm0, %%xmm2\n"
        "pshufd $0x92, %%xmm1, %%xmm1\n"
        "mulps  %%xmm5, %%xmm3\n"
        "mulps  %%xmm1, %%xmm2\n"
        "movaps %3, %%xmm1\n"
        "mulps  %%xmm6, %%xmm4\n"
        "mulps  %%xmm7, %%xmm0\n"
        "xorps  %%xmm1, %%xmm2\n"
        "xorps  %%xmm1, %%xmm3\n"
        "subps  %%xmm4, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b), "m"(mask[0])
    );
}

void pl2QuatMultiply_NoSSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    fvector4_t c;
    c.x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
    c.y = a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z;
    c.z = a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x;
    c.w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    *out = c;
}

/******************************************************************************/

typedef void (*PL2MULTMATRIX4F)(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b);
typedef void (*PL2VECTORTRANSFORM4F)(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v);
typedef void (*PL2TRANSPOSEMATRIX4F)(fmatrix4_t *out, const fmatrix4_t *m);
typedef void (*PL2VECTORADD4F)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
typedef void (*PL2VECTORSUB4F)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
typedef float (*PL2VECTORDOT4F)(const fvector4_t *a, const fvector4_t *b);
typedef void (*PL2VECTORSCALE4F)(fvector4_t *out, const fvector4_t *v, float s);
typedef void (*PL2VECTORSCALEADD4F)(fvector4_t *out, const fvector4_t *v, float s);
typedef void (*PL2VECTORTRANSSCALEADD4F)(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v, float s);
typedef void (*PL2VECTORADD3F)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
typedef void (*PL2VECTORSUB3F)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
typedef float (*PL2VECTORDOT3F)(const fvector3_t *a, const fvector3_t *b);
typedef void (*PL2VECTORCROSS3F)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
typedef void (*PL2VECTORSCALE3F)(fvector3_t *out, const fvector3_t *v, float s);
typedef void (*PL2VECTORSCALEADD3F)(fvector3_t *out, const fvector3_t *v, float s);
typedef float (*PL2VECTORLENGTH3F)(const fvector3_t *v);
typedef void (*PL2VECTORNORMALIZE3F)(fvector3_t *out, const fvector3_t *v);
typedef void (*PL2QUATMULTIPLY)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
typedef void (*PL2QUATROTATE)(fvector3_t *out, const fvector3_t *v, const fvector3_t *axis, float angle);
typedef void (*PL2VECTORORBIT)(fvector3_t *planet, const fvector3_t *sun, const fvector3_t *up, const fvector3_t *rotate);
typedef void (*PL2VECTORZOOM)(fvector3_t *obj, const fvector3_t *targ, float distance);

PL2MULTMATRIX4F pl2MultMatrix4f = pl2MultMatrix4f_NoSSE;
PL2VECTORTRANSFORM4F pl2VectorTransform4f = pl2VectorTransform4f_NoSSE;
PL2TRANSPOSEMATRIX4F pl2TransposeMatrix4f = pl2TransposeMatrix4f_NoSSE;
PL2VECTORADD4F pl2VectorAdd4f = pl2VectorAdd4f_NoSSE;
PL2VECTORSUB4F pl2VectorSub4f = pl2VectorSub4f_NoSSE;
PL2VECTORDOT4F pl2VectorDot4f = pl2VectorDot4f_NoSSE;
PL2VECTORSCALE4F pl2VectorScale4f = pl2VectorScale4f_NoSSE;
PL2VECTORSCALEADD4F pl2VectorScaleAdd4f = pl2VectorScaleAdd4f_NoSSE;
PL2VECTORTRANSSCALEADD4F pl2VectorTransScaleAdd4f = pl2VectorTransScaleAdd4f_NoSSE;
PL2VECTORADD3F pl2VectorAdd3f = pl2VectorAdd3f_NoSSE;
PL2VECTORSUB3F pl2VectorSub3f = pl2VectorSub3f_NoSSE;
PL2VECTORDOT3F pl2VectorDot3f = pl2VectorDot3f_NoSSE;
PL2VECTORCROSS3F pl2VectorCross3f = pl2VectorCross3f_NoSSE;
PL2VECTORSCALE3F pl2VectorScale3f = pl2VectorScale3f_NoSSE;
PL2VECTORSCALEADD3F pl2VectorScaleAdd3f = pl2VectorScaleAdd3f_NoSSE;
PL2VECTORLENGTH3F pl2VectorLength3f = pl2VectorLength3f_NoSSE;
PL2VECTORNORMALIZE3F pl2VectorNormalize3f = pl2VectorNormalize3f_NoSSE;
PL2QUATMULTIPLY pl2QuatMultiply = pl2QuatMultiply_NoSSE;

/******************************************************************************/

#ifdef VMTEST

#include <time.h>

#define NUM_ROUNDS 10000000

#define TIME(f, x...) \
    start = clock(); \
    for(i = 0; i < NUM_ROUNDS; i++) { \
        f(x); } \
    stop = clock(); \
    fprintf(stderr, "%s: %d rounds in %.2fs = %.1f rounds/s\n", #f, NUM_ROUNDS, \
        (float)(stop - start) / (float)(CLOCKS_PER_SEC), \
        (float)((long long)NUM_ROUNDS * (long long)CLOCKS_PER_SEC) / (float)(stop - start));

#define PRINTVECTOR(V) \
    fprintf(stderr, #V " = < %14g %14g %14g %14g >\n", V.x, V.y, V.z, V.w);

#define PRINTMATRIX(M) \
    fprintf(stderr, #M " =\n[ %14g %14g %14g %14g ]\n[ %14g %14g %14g %14g ]\n" \
                           "[ %14g %14g %14g %14g ]\n[ %14g %14g %14g %14g ]\n", \
            M.x.x, M.y.x, M.z.x, M.w.x, M.x.y, M.y.y, M.z.y, M.w.y, \
            M.x.z, M.y.z, M.z.z, M.w.z, M.x.w, M.y.w, M.z.w, M.w.w );

int main(int argc, char *argv[])
{
    const fvector4_t zero = { 0, 0, 0, 0 }, one = { 1, 1, 1, 1 };

    fvector4_t tv, a, b = {1,2,3,4}, c = {5,6,7,8};
    fmatrix4_t tm, m,
               n = {{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}},
               o = {{0,0,0,1},{0,0,1,0},{0,1,0,0},{1,0,0,0}};

    clock_t start, stop; int i;

    PRINTVECTOR(b);
    PRINTMATRIX(n);
    PRINTMATRIX(o);

    TIME(pl2VectorTransform4f_SSE, &a, &n, &b);
    PRINTVECTOR(a);
    TIME(pl2VectorTransform4f_NoSSE, &a, &n, &b);
    PRINTVECTOR(a);
    TIME(pl2MultMatrix4f_SSE, &m, &n, &o);
    PRINTMATRIX(m);
    TIME(pl2MultMatrix4f_NoSSE, &m, &n, &o);
    PRINTMATRIX(m);
    TIME(pl2TransposeMatrix4f_SSE, &m, &n);
    PRINTMATRIX(m);
    TIME(pl2TransposeMatrix4f_NoSSE, &m, &n);
    PRINTMATRIX(m);

    return 0;
}

#else // VMTEST

/******************************************************************************/

void pl2DetectSSE()
{
#if SDL_VERSION_ATLEAST(1,2,7)
    if(SDL_HasSSE())
#else
    uint32_t cpu_features[2] = { 0, 0 };

    asm volatile(
        "mov $1, %%eax\n"
        "cpuid\n"
        "mov %%ecx, 0 %0\n"
        "mov %%edx, 4 %0\n"
        :"=m"(cpu_features)
    );

    if(cpu_features[1] & 0x20000000)
#endif
    {
        DEBUGPRINT("%s: SSE support detected\n", __func__);

        pl2MultMatrix4f = pl2MultMatrix4f_SSE;
        pl2VectorTransform4f = pl2VectorTransform4f_SSE;
        pl2TransposeMatrix4f = pl2TransposeMatrix4f_SSE;
        pl2VectorAdd4f = pl2VectorAdd4f_SSE;
        pl2VectorSub4f = pl2VectorSub4f_SSE;
        pl2VectorDot4f = pl2VectorDot4f_SSE;
        pl2VectorScale4f = pl2VectorScale4f_SSE;
        pl2VectorScaleAdd4f = pl2VectorScaleAdd4f_SSE;
        pl2VectorTransScaleAdd4f = pl2VectorTransScaleAdd4f_SSE;
        pl2VectorAdd3f = pl2VectorAdd3f_SSE;
        pl2VectorSub3f = pl2VectorSub3f_SSE;
        pl2VectorDot3f = pl2VectorDot3f_SSE;
        pl2VectorCross3f = pl2VectorCross3f_SSE;
        pl2VectorScale3f = pl2VectorScale3f_SSE;
        pl2VectorScaleAdd3f = pl2VectorScaleAdd3f_SSE;
        pl2VectorLength3f = pl2VectorLength3f_SSE;
        pl2VectorNormalize3f = pl2VectorNormalize3f_SSE;
        pl2QuatMultiply = pl2QuatMultiply_SSE;
    }
    else
    {
        DEBUGPRINT("%s: no SSE support detected\n", __func__);
    }
}

/******************************************************************************/

int main(int argc, char *argv[])
{
    pl2DetectSSE();

    if(!pl2GameInit(&argc, argv))
        return 1;

    pl2GameRun();
    return 0;
}

#endif
