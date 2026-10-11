/**
 *
 *  Copyright (C) 2026 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without RESTRICTion, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#include <arm_neon.h>


void CCALL simd_10007028_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint32_t mult4, uint32_t mult5, uint8_t * RESTRICT tmp, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int16x8_t q0, q1, q2, q3, q5, q6, q7, q8, q9;
    int16x8_t tmp0, tmp1;
    int32x4_t q12, q13;
    int16x8x2_t qq14, qq15;

    q7 = vmovq_n_s16((uint16_t)mult1);      // q7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    tmp0 = vmovq_n_s16((uint16_t)mult2);    // tmp0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    tmp1 = vmovq_n_s16((uint16_t)mult3);    // tmp1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    q5 = vmovq_n_s16((uint16_t)mult4);      // q5 = mul4:mul4:mul4:mul4:mul4:mul4:mul4:mul4
    q3 = vmovq_n_s16((uint16_t)mult5);      // q3 = mul5:mul5:mul5:mul5:mul5:mul5:mul5:mul5

    do
    {
        q6 = vld1q_s16((int16_t *)src);
        src += 16;
        q8 = vld1q_s16((int16_t *)dst2);
        q9 = vld1q_s16((int16_t *)dst3);
        qq15 = vld2q_s16((int16_t *)dst1);

        q12 = vmull_s16(vget_low_s16(q6), vget_low_s16(q7));
#if defined(ARMV8)
        q13 = vmull_high_s16(q6, q7);
        q6 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(q6), vget_high_s16(q7));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q6 = qq14.val[1];                   // q6 = [src] * mult1
#endif

        q12 = vmull_s16(vget_low_s16(tmp0), vget_low_s16(q6));
#if defined(ARMV8)
        q13 = vmull_high_s16(tmp0, q6);
        q0 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(tmp0), vget_high_s16(q6));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q0 = qq14.val[1];               // q0 = [src] * mult1 * mult2
#endif

        q12 = vmull_s16(vget_low_s16(tmp1), vget_low_s16(q6));
#if defined(ARMV8)
        q13 = vmull_high_s16(tmp1, q6);
        q1 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(tmp1), vget_high_s16(q6));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q1 = qq14.val[1];               // q1 = [src] * mult1 * mult3
#endif

        q12 = vmull_s16(vget_low_s16(q5), vget_low_s16(q6));
#if defined(ARMV8)
        q13 = vmull_high_s16(q5, q6);
        q2 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(q5), vget_high_s16(q6));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q2 = qq14.val[1];               // q2 = [src] * mult1 * mult4
#endif

        q12 = vmull_s16(vget_low_s16(q6), vget_low_s16(q3));
#if defined(ARMV8)
        q13 = vmull_high_s16(q6, q3);
        q6 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(q6), vget_high_s16(q3));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q6 = qq14.val[1];               // q6 = [src] * mult1 * mult5
#endif

        qq14.val[0] = vshlq_n_s16(q0, 1);   // qq14.val[0] = ([src] * mult1 * mult2) << 1
        qq14.val[1] = vshlq_n_s16(q1, 1);   // qq14.val[1] = ([src] * mult1 * mult3) << 1
        q2 = vshlq_n_s16(q2, 1);            // q2 = ([src] * mult1 * mult4) << 1
        q6 = vshlq_n_s16(q6, 1);            // q6 = ([src] * mult1 * mult5) << 1
        q2 = vqaddq_s16(q2, q8);
        vst1q_s16((int16_t *)dst2, q2);
        q6 = vqaddq_s16(q6, q9);
        vst1q_s16((int16_t *)dst3, q6);
        qq14.val[0] = vqaddq_s16(qq14.val[0], qq15.val[0]);
        qq14.val[1] = vqaddq_s16(qq14.val[1], qq15.val[1]);
        vst2q_s16((int16_t *)dst1, qq14);
        dst1 += 32;
        dst2 += 16;
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007104_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint32_t mult4, uint32_t mult5, uint8_t * RESTRICT tmp, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int16x8_t q0, q1, q2, q3, q5, q6, q7;
    int16x8_t tmp0, tmp1;
    int32x4_t q12, q13;
    int16x8x2_t qq14;

    q7 = vmovq_n_s16((uint16_t)mult1);      // q7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    tmp0 = vmovq_n_s16((uint16_t)mult2);    // tmp0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    tmp1 = vmovq_n_s16((uint16_t)mult3);    // tmp1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    q5 = vmovq_n_s16((uint16_t)mult4);      // q5 = mul4:mul4:mul4:mul4:mul4:mul4:mul4:mul4
    q3 = vmovq_n_s16((uint16_t)mult5);      // q3 = mul5:mul5:mul5:mul5:mul5:mul5:mul5:mul5

    do
    {
        q6 = vld1q_s16((int16_t *)src);
        src += 16;

        q12 = vmull_s16(vget_low_s16(q6), vget_low_s16(q7));
#if defined(ARMV8)
        q13 = vmull_high_s16(q6, q7);
        q6 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(q6), vget_high_s16(q7));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q6 = qq14.val[1];                   // q6 = [src] * mult1
#endif

        q12 = vmull_s16(vget_low_s16(tmp0), vget_low_s16(q6));
#if defined(ARMV8)
        q13 = vmull_high_s16(tmp0, q6);
        q0 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(tmp0), vget_high_s16(q6));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q0 = qq14.val[1];               // q0 = [src] * mult1 * mult2
#endif

        q12 = vmull_s16(vget_low_s16(tmp1), vget_low_s16(q6));
#if defined(ARMV8)
        q13 = vmull_high_s16(tmp1, q6);
        q1 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(tmp1), vget_high_s16(q6));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q1 = qq14.val[1];               // q1 = [src] * mult1 * mult3
#endif

        q12 = vmull_s16(vget_low_s16(q5), vget_low_s16(q6));
#if defined(ARMV8)
        q13 = vmull_high_s16(q5, q6);
        q2 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(q5), vget_high_s16(q6));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q2 = qq14.val[1];               // q2 = [src] * mult1 * mult4
#endif

        q12 = vmull_s16(vget_low_s16(q6), vget_low_s16(q3));
#if defined(ARMV8)
        q13 = vmull_high_s16(q6, q3);
        q6 = vuzp2q_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
#else
        q13 = vmull_s16(vget_high_s16(q6), vget_high_s16(q3));
        qq14 = vuzpq_s16(vreinterpretq_s16_s32(q12), vreinterpretq_s16_s32(q13));
        q6 = qq14.val[1];               // q6 = [src] * mult1 * mult5
#endif

        qq14.val[0] = vshlq_n_s16(q0, 1);   // qq14.val[0] = ([src] * mult1 * mult2) << 1
        qq14.val[1] = vshlq_n_s16(q1, 1);   // qq14.val[1] = ([src] * mult1 * mult3) << 1
        q2 = vshlq_n_s16(q2, 1);            // q2 = ([src] * mult1 * mult4) << 1
        q6 = vshlq_n_s16(q6, 1);            // q6 = ([src] * mult1 * mult5) << 1
        vst1q_s16((int16_t *)dst2, q2);
        vst1q_s16((int16_t *)dst3, q6);
        vst2q_s16((int16_t *)dst1, qq14);
        dst1 += 32;
        dst2 += 16;
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_100071D0_c(int32_t counter, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int16x8_t q0;

    q0 = vmovq_n_s16(0);

    do
    {
        vst1q_s16((int16_t *)dst1, q0);
        vst1q_s16((int16_t *)dst2, q0);
        dst1 += 16;
        dst2 += 16;
        vst1q_s16((int16_t *)dst1, q0);
        vst1q_s16((int16_t *)dst3, q0);
        dst1 += 16;
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_1000720B_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int16x8_t q2, q3, q4, q5, q6, q7, q8, q9, q10;
    int32x4_t q11, q12;
#if !defined(ARMV8)
    int16x8x2_t qq13;
#endif

    q7 = vmovq_n_s16((uint16_t)mult1);  // q7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    q5 = vmovq_n_s16((uint16_t)mult2);  // q5 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    q4 = vmovq_n_s16((uint16_t)mult3);  // q4 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3

    do
    {
        q6 = vld1q_s16((int16_t *)src);
        src += 16;
        q8 = vld1q_s16((int16_t *)dst2);
        q9 = vld1q_s16((int16_t *)dst3);
        q10 = vld1q_s16((int16_t *)dst1);

        q11 = vmull_s16(vget_low_s16(q6), vget_low_s16(q7));
#if defined(ARMV8)
        q12 = vmull_high_s16(q6, q7);
        q6 = vuzp2q_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
#else
        q12 = vmull_s16(vget_high_s16(q6), vget_high_s16(q7));
        qq13 = vuzpq_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
        q6 = qq13.val[1];               // q6 = [src] * mult1
#endif

        q11 = vmull_s16(vget_low_s16(q5), vget_low_s16(q6));
#if defined(ARMV8)
        q12 = vmull_high_s16(q5, q6);
        q2 = vuzp2q_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
#else
        q12 = vmull_s16(vget_high_s16(q5), vget_high_s16(q6));
        qq13 = vuzpq_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
        q2 = qq13.val[1];               // q2 = [src] * mult1 * mult2
#endif

        q11 = vmull_s16(vget_low_s16(q4), vget_low_s16(q6));
#if defined(ARMV8)
        q12 = vmull_high_s16(q4, q6);
        q3 = vuzp2q_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
#else
        q12 = vmull_s16(vget_high_s16(q4), vget_high_s16(q6));
        qq13 = vuzpq_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
        q3 = qq13.val[1];               // q3 = [src] * mult1 * mult3
#endif

        q2 = vshlq_n_s16(q2, 1);        // q2 = ([src] * mult1 * mult2) << 1
        q2 = vqaddq_s16(q2, q8);
        vst1q_s16((int16_t *)dst2, q2);
        q3 = vshlq_n_s16(q3, 1);        // q3 = ([src] * mult1 * mult3) << 1
        q3 = vqaddq_s16(q3, q9);
        vst1q_s16((int16_t *)dst3, q3);
        q6 = vqaddq_s16(q6, q10);
        vst1q_s16((int16_t *)dst1, q6);
        dst1 += 16;
        dst2 += 16;
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_100072A3_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int16x8_t q2, q3, q4, q5, q6, q7;
    int32x4_t q11, q12;
#if !defined(ARMV8)
    int16x8x2_t qq13;
#endif

    q7 = vmovq_n_s16((uint16_t)mult1);  // q7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    q5 = vmovq_n_s16((uint16_t)mult2);  // q5 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    q4 = vmovq_n_s16((uint16_t)mult3);  // q4 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3

    do
    {
        q6 = vld1q_s16((int16_t *)src);
        src += 16;

        q11 = vmull_s16(vget_low_s16(q6), vget_low_s16(q7));
#if defined(ARMV8)
        q12 = vmull_high_s16(q6, q7);
        q6 = vuzp2q_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
#else
        q12 = vmull_s16(vget_high_s16(q6), vget_high_s16(q7));
        qq13 = vuzpq_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
        q6 = qq13.val[1];               // q6 = [src] * mult1
#endif

        q11 = vmull_s16(vget_low_s16(q5), vget_low_s16(q6));
#if defined(ARMV8)
        q12 = vmull_high_s16(q5, q6);
        q2 = vuzp2q_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
#else
        q12 = vmull_s16(vget_high_s16(q5), vget_high_s16(q6));
        qq13 = vuzpq_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
        q2 = qq13.val[1];               // q2 = [src] * mult1 * mult2
#endif

        q11 = vmull_s16(vget_low_s16(q4), vget_low_s16(q6));
#if defined(ARMV8)
        q12 = vmull_high_s16(q4, q6);
        q3 = vuzp2q_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
#else
        q12 = vmull_s16(vget_high_s16(q4), vget_high_s16(q6));
        qq13 = vuzpq_s16(vreinterpretq_s16_s32(q11), vreinterpretq_s16_s32(q12));
        q3 = qq13.val[1];               // q3 = [src] * mult1 * mult3
#endif

        q2 = vshlq_n_s16(q2, 1);        // q2 = ([src] * mult1 * mult2) << 1
        vst1q_s16((int16_t *)dst2, q2);
        q3 = vshlq_n_s16(q3, 1);        // q3 = ([src] * mult1 * mult3) << 1
        vst1q_s16((int16_t *)dst3, q3);
        vst1q_s16((int16_t *)dst1, q6);
        dst1 += 16;
        dst2 += 16;
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_1000732F_c(int32_t counter, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int16x8_t q0;

    q0 = vmovq_n_s16(0);

    do
    {
        vst1q_s16((int16_t *)dst1, q0);
        vst1q_s16((int16_t *)dst2, q0);
        dst1 += 16;
        dst2 += 16;
        vst1q_s16((int16_t *)dst3, q0);
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007364_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst)
{
    int16x8_t q0, q1, q2, q3, q6, q7;
    int32x4_t q10, q11;
    int16x8x2_t qq12, qq13;

    q7 = vmovq_n_s16((uint16_t)mult1);      // q7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    q2 = vmovq_n_s16((uint16_t)mult2);      // q2 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    q3 = vmovq_n_s16((uint16_t)mult3);      // q3 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3

    do
    {
        q6 = vld1q_s16((int16_t *)src);
        src += 16;
        qq13 = vld2q_s16((int16_t *)dst);

        q10 = vmull_s16(vget_low_s16(q6), vget_low_s16(q7));
#if defined(ARMV8)
        q11 = vmull_high_s16(q6, q7);
        q6 = vuzp2q_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
#else
        q11 = vmull_s16(vget_high_s16(q6), vget_high_s16(q7));
        qq12 = vuzpq_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
        q6 = qq12.val[1];                   // q6 = [src] * mult1
#endif

        q10 = vmull_s16(vget_low_s16(q2), vget_low_s16(q6));
#if defined(ARMV8)
        q11 = vmull_high_s16(q2, q6);
        q0 = vuzp2q_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
#else
        q11 = vmull_s16(vget_high_s16(q2), vget_high_s16(q6));
        qq12 = vuzpq_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
        q0 = qq12.val[1];                   // q0 = [src] * mult1 * mult2
#endif

        q10 = vmull_s16(vget_low_s16(q3), vget_low_s16(q6));
#if defined(ARMV8)
        q11 = vmull_high_s16(q3, q6);
        q1 = vuzp2q_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
#else
        q11 = vmull_s16(vget_high_s16(q3), vget_high_s16(q6));
        qq12 = vuzpq_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
        q1 = qq12.val[1];                   // q1 = [src] * mult1 * mult3
#endif

        qq12.val[0] = vshlq_n_s16(q0, 1);   // qq12.val[0] = ([src] * mult1 * mult2) << 1
        qq12.val[1] = vshlq_n_s16(q1, 1);   // qq12.val[1] = ([src] * mult1 * mult3) << 1
        qq12.val[0] = vqaddq_s16(qq12.val[0], qq13.val[0]);
        qq12.val[1] = vqaddq_s16(qq12.val[1], qq13.val[1]);
        vst2q_s16((int16_t *)dst, qq12);
        dst += 32;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_100073F1_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst)
{
    int16x8_t q0, q1, q2, q3, q6, q7;
    int32x4_t q10, q11;
    int16x8x2_t qq12;

    q7 = vmovq_n_s16((uint16_t)mult1);      // q7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    q2 = vmovq_n_s16((uint16_t)mult2);      // q2 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    q3 = vmovq_n_s16((uint16_t)mult3);      // q3 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3

    do
    {
        q6 = vld1q_s16((int16_t *)src);
        src += 16;

        q10 = vmull_s16(vget_low_s16(q6), vget_low_s16(q7));
#if defined(ARMV8)
        q11 = vmull_high_s16(q6, q7);
        q6 = vuzp2q_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
#else
        q11 = vmull_s16(vget_high_s16(q6), vget_high_s16(q7));
        qq12 = vuzpq_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
        q6 = qq12.val[1];                   // q6 = [src] * mult1
#endif

        q10 = vmull_s16(vget_low_s16(q2), vget_low_s16(q6));
#if defined(ARMV8)
        q11 = vmull_high_s16(q2, q6);
        q0 = vuzp2q_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
#else
        q11 = vmull_s16(vget_high_s16(q2), vget_high_s16(q6));
        qq12 = vuzpq_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
        q0 = qq12.val[1];                   // q0 = [src] * mult1 * mult2
#endif

        q10 = vmull_s16(vget_low_s16(q3), vget_low_s16(q6));
#if defined(ARMV8)
        q11 = vmull_high_s16(q3, q6);
        q1 = vuzp2q_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
#else
        q11 = vmull_s16(vget_high_s16(q3), vget_high_s16(q6));
        qq12 = vuzpq_s16(vreinterpretq_s16_s32(q10), vreinterpretq_s16_s32(q11));
        q1 = qq12.val[1];                   // q1 = [src] * mult1 * mult3
#endif

        qq12.val[0] = vshlq_n_s16(q0, 1);   // qq12.val[0] = ([src] * mult1 * mult2) << 1
        qq12.val[1] = vshlq_n_s16(q1, 1);   // qq12.val[1] = ([src] * mult1 * mult3) << 1
        vst2q_s16((int16_t *)dst, qq12);
        dst += 32;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007472_c(int32_t counter, uint8_t * RESTRICT dst)
{
    int16x8_t q0;

    q0 = vmovq_n_s16(0);

    do
    {
        vst1q_s16((int16_t *)dst, q0);
        dst += 16;
        counter -= 4;
    } while (counter > 0);
}

void CCALL simd_10007491_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult, uint8_t * RESTRICT dst)
{
    int16x8_t q0, q7, q8;
    int32x4_t q9, q10;
#if !defined(ARMV8)
    int16x8x2_t qq11;
#endif

    q7 = vmovq_n_s16((uint16_t)mult);   // d7 = mult:mult:mult:mult:mult:mult:mult:mult

    do
    {
        q0 = vld1q_s16((int16_t *)src);
        q8 = vld1q_s16((int16_t *)dst);

        q9 = vmull_s16(vget_low_s16(q0), vget_low_s16(q7));
#if defined(ARMV8)
        q10 = vmull_high_s16(q0, q7);
        q0 = vuzp2q_s16(vreinterpretq_s16_s32(q9), vreinterpretq_s16_s32(q10));
#else
        q10 = vmull_s16(vget_high_s16(q0), vget_high_s16(q7));
        qq11 = vuzpq_s16(vreinterpretq_s16_s32(q9), vreinterpretq_s16_s32(q10));
        q0 = qq11.val[1];
#endif

        q0 = vqaddq_s16(q0, q8);
        vst1q_s16((int16_t *)dst, q0);
        src += 16;
        dst += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_100074CD_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult, uint8_t * RESTRICT dst)
{
    int16x8_t q0, q7;
    int32x4_t q9, q10;
#if !defined(ARMV8)
    int16x8x2_t qq11;
#endif

    q7 = vmovq_n_s16((uint16_t)mult);   // d7 = mult:mult:mult:mult:mult:mult:mult:mult

    do
    {
        q0 = vld1q_s16((int16_t *)src);

        q9 = vmull_s16(vget_low_s16(q0), vget_low_s16(q7));
#if defined(ARMV8)
        q10 = vmull_high_s16(q0, q7);
        q0 = vuzp2q_s16(vreinterpretq_s16_s32(q9), vreinterpretq_s16_s32(q10));
#else
        q10 = vmull_s16(vget_high_s16(q0), vget_high_s16(q7));
        qq11 = vuzpq_s16(vreinterpretq_s16_s32(q9), vreinterpretq_s16_s32(q10));
        q0 = qq11.val[1];
#endif

        vst1q_s16((int16_t *)dst, q0);
        src += 16;
        dst += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007506_c(int32_t counter, uint8_t * RESTRICT dst)
{
    int16x8_t q0;

    q0 = vmovq_n_s16(0);

    do
    {
        vst1q_s16((int16_t *)dst, q0);
        dst += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007525_c(int32_t counter1, int32_t multadd2, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16x8_t q0, q2;
    int16x8x2_t qq11;
    int32_t counter2;

    multadd2 = (multadd2 << 1) - 1;
    if (multadd2 >= 0)
    {
        do
        {
            q0 = vld1q_s16((int16_t *)src);
            qq11 = vld2q_s16((int16_t *)dst);
            counter2 = multadd2;
            q2 = q0;
            src += 16;

            do
            {
                q0 = vqaddq_s16(q0, q2);
                counter2--;
            } while (counter2 > 0);

            qq11.val[0] = vqaddq_s16(qq11.val[0], q0);
            qq11.val[1] = vqaddq_s16(qq11.val[1], q0);
            vst2q_s16((int16_t *)dst, qq11);
            dst += 32;
            counter1 -= 8;
        } while (counter1 > 0);
    }
    else
    {
        do
        {
            q0 = vld1q_s16((int16_t *)src);
            qq11 = vld2q_s16((int16_t *)dst);
            src += 16;
            qq11.val[0] = vqaddq_s16(qq11.val[0], q0);
            qq11.val[1] = vqaddq_s16(qq11.val[1], q0);
            vst2q_s16((int16_t *)dst, qq11);

            dst += 32;
            counter1 -= 8;
        } while (counter1 > 0);
    }
}

void CCALL simd_1000758D_c(int32_t counter1, int32_t multadd2, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16x8_t q0, q1, q8;
    int32_t counter2;

    multadd2 = (multadd2 << 1) - 1;
    if (multadd2 >= 0)
    {
        do
        {
            q0 = vld1q_s16((int16_t *)src);
            q8 = vld1q_s16((int16_t *)dst);
            counter2 = multadd2;
            q1 = q0;
            src += 16;

            do
            {
                q0 = vqaddq_s16(q0, q1);
                counter2--;
            } while (counter2 > 0);

            q0 = vqaddq_s16(q0, q8);
            vst1q_s16((int16_t *)dst, q0);
            dst += 16;
            counter1 -= 8;
        } while (counter1 > 0);
    }
    else
    {
        do
        {
            q0 = vld1q_s16((int16_t *)src);
            q8 = vld1q_s16((int16_t *)dst);
            src += 16;
            q0 = vqaddq_s16(q0, q8);
            vst1q_s16((int16_t *)dst, q0);
            dst += 16;
            counter1 -= 8;
        } while (counter1 > 0);
    }
}

void CCALL simd_100075CC_c(int32_t counter, uint32_t shift, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16x8_t q0, q7;
    int32x4_t q9, q10;
    int16x4_t d4, d5;

    if (cs_volume_multiplier == 8192)
    {
        q7 = vmovq_n_s16(1 - (int16_t)shift); // negative shift left is positive shift right

        do
        {
            q0 = vld1q_s16((int16_t *)src);
            q0 = vshlq_s16(q0, q7);
            vst1q_s16((int16_t *)dst, q0);
            dst += 16;
            src += 16;
            counter -= 8;
        } while (counter > 0);
    }
    else
    {
        q7 = vmovq_n_s16(cs_volume_multiplier >> ((int16_t)shift - 1));

        do
        {
            q0 = vld1q_s16((int16_t *)src);

            q9 = vmull_s16(vget_low_s16(q0), vget_low_s16(q7));
#if defined(ARMV8)
            q10 = vmull_high_s16(q0, q7);
#else
            q10 = vmull_s16(vget_high_s16(q0), vget_high_s16(q7));
#endif
            d4 = vqshrn_n_s32(q9, 11);
            vst1_s16((int16_t *)dst, d4);
            d5 = vqshrn_n_s32(q10, 11);
            vst1_s16((int16_t *)(dst + 8), d5);

            dst += 16;
            src += 16;
            counter -= 8;
        } while (counter > 0);
    }
}

// reverb

void CCALL simd_1000A920_c(int32_t counter1, int16_t * RESTRICT params1, uint16_t * RESTRICT params2, uint16_t * RESTRICT data1, uint32_t * RESTRICT data2, uint32_t * RESTRICT data3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16x4_t d0, d1, d2, d3, d4, d5, d6, d7, d8;
    int32x4_t q9;
    uint32_t counter2, ofs1, ofs2;
    int16_t * RESTRICT pval2;
    uint16_t * RESTRICT pval3;

    d7 = vld1_dup_s16(params1);         // d7 = p1_0:p1_0:p1_0:p1_0
    d6 = vld1_dup_s16(params1 + 1);     // d6 = p1_1:p1_1:p1_1:p1_1
    d5 = vld1_dup_s16(params1 + 2);     // d5 = p1_2:p1_2:p1_2:p1_2

    do
    {
        d0 = vld1_s16((int16_t *)src);
        src += 8;
        counter2 = 0;

        do
        {
            pval2 = ((PTR32(int16_t) *)data2)[counter2];
            pval3 = ((PTR32(uint16_t) *)data3)[counter2];
            ofs1 = *pval3;
            ofs2 = data1[counter2];

            if ((ofs2 & 2) == 0)
            {
                if ((ofs2 & 1) == 0)
                {
                    vst1_s16(pval2 + ofs1, d0);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 > ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d0 = vld1_s16(pval2 + ofs1);
                    *pval3 = ofs1;
                }
                else
                {
                    d2 = vld1_s16(pval2 + ofs2 + 3);
                    d1 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d0), 16));
                    d2 = vqadd_s16(d2, d1);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d0), 48));
                    vst1_s16(pval2 + ofs1, d2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d0 = vld1_s16(pval2 + ofs1);
                    vst1_s16(pval2 + ofs2 + 3, d1);
                    *pval3 = ofs1;
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    d2 = vld1_s16(pval2 + ofs2 + 2);
                    d1 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d0), 32));
                    d2 = vqadd_s16(d2, d1);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d0), 32));
                    vst1_s16(pval2 + ofs1, d2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d0 = vld1_s16(pval2 + ofs1);
                    vst1_s16(pval2 + ofs2 + 2, d1);
                    *pval3 = ofs1;
                }
                else
                {
                    d2 = vld1_s16(pval2 + ofs2 + 1);
                    d1 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d0), 48));
                    d2 = vqadd_s16(d2, d1);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d0), 16));
                    vst1_s16(pval2 + ofs1, d2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d0 = vld1_s16(pval2 + ofs1);
                    vst1_s16(pval2 + ofs2 + 1, d1);
                    *pval3 = ofs1;
                }
            }

            counter2++;
        } while (counter2 < 1);

        d4 = vld1_dup_s16((int16_t *)params2);
        d2 = vqsub_s16(d0, d4);
        q9 = vmull_s16(d2, d6);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d4 = vadd_s16(d4, d2);
        d0 = vext_s16(d0, d4, 1);
        d2 = vqsub_s16(d0, d4);
        q9 = vmull_s16(d2, d6);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d4 = vadd_s16(d4, d2);
        d0 = vext_s16(d0, d4, 1);
        d2 = vqsub_s16(d0, d4);
        q9 = vmull_s16(d2, d6);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d4 = vadd_s16(d4, d2);
        d0 = vext_s16(d0, d4, 1);
        d2 = vqsub_s16(d0, d4);
        q9 = vmull_s16(d2, d6);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d4 = vadd_s16(d4, d2);
        d0 = vext_s16(d0, d4, 1);
        vst1_lane_s16((int16_t *)params2, d4, 0);

        do
        {
            pval2 = ((PTR32(int16_t) *)data2)[counter2];
            pval3 = ((PTR32(uint16_t) *)data3)[counter2];
            ofs1 = *pval3;
            ofs2 = data1[counter2];

            if ((ofs2 & 2) == 0)
            {
                if ((ofs2 & 1) == 0)
                {
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 > ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d3 = vld1_s16(pval2 + ofs1);
                    q9 = vmull_s16(d3, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d2 = vqsub_s16(d0, d4);
                    q9 = vmull_s16(d2, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d0 = vqadd_s16(d3, d4);
                    if (ofs1 != 0)
                    {
                        vst1_s16(pval2 + ofs1 - 4, d2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        vst1_s16(pval2 + ofs2, d2);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    d2 = vld1_s16(pval2 + ofs2 + 3);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d3 = vld1_s16(pval2 + ofs1);
                    q9 = vmull_s16(d3, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d1 = vqsub_s16(d0, d4);
                    q9 = vmull_s16(d1, d7);
                    d0 = vshrn_n_s32(q9, 16);
                    d0 = vshl_n_s16(d0, 1);
                    d4 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d1), 16));
                    d0 = vqadd_s16(d3, d0);
                    d2 = vqadd_s16(d2, d4);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d1), 48));
                    if (ofs1 != 0)
                    {
                        vst1_s16(pval2 + ofs1 - 4, d2);
                        vst1_s16(pval2 + ofs2 + 3, d1);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        vst1_s16(pval2 + ofs2 - 1, d2);
                        vst1_s16(pval2 + ofs2 + 3, d1);
                        *pval3 = ofs1;
                    }
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    d2 = vld1_s16(pval2 + ofs2 + 2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d3 = vld1_s16(pval2 + ofs1);
                    q9 = vmull_s16(d3, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d1 = vqsub_s16(d0, d4);
                    q9 = vmull_s16(d1, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d0 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d1), 32));
                    d2 = vqadd_s16(d2, d0);
                    d0 = vqadd_s16(d3, d4);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d1), 32));
                    if (ofs1 != 0)
                    {
                        vst1_s16(pval2 + ofs1 - 4, d2);
                        vst1_s16(pval2 + ofs2 + 2, d1);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        vst1_s16(pval2 + ofs2 - 2, d2);
                        vst1_s16(pval2 + ofs2 + 2, d1);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    d2 = vld1_s16(pval2 + ofs2 + 1);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d3 = vld1_s16(pval2 + ofs1);
                    q9 = vmull_s16(d3, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d1 = vqsub_s16(d0, d4);
                    q9 = vmull_s16(d1, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d0 = vqadd_s16(d3, d4);
                    d3 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d1), 48));
                    d2 = vqadd_s16(d2, d3);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d1), 16));
                    if (ofs1 != 0)
                    {
                        vst1_s16(pval2 + ofs1 - 4, d2);
                        vst1_s16(pval2 + ofs2 + 1, d1);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        vst1_s16(pval2 + ofs2 - 3, d2);
                        vst1_s16(pval2 + ofs2 + 1, d1);
                        *pval3 = ofs1;
                    }
                }
            }

            counter2++;
        } while (counter2 < 5);

        d8 = vld1_s16((int16_t *)dst);
        q9 = vmull_s16(d0, d5);
        d0 = vshrn_n_s32(q9, 16);
        d0 = vshl_n_s16(d0, 1);
        d0 = vqadd_s16(d0, d8);
        vst1_s16((int16_t *)dst, d0);
        dst += 8;
        counter1 -= 4;
    } while (counter1 > 0);
}

void CCALL simd_1000AD02_c(int32_t counter1, int16_t * RESTRICT params1, uint16_t * RESTRICT params2, uint16_t * RESTRICT data1, uint32_t * RESTRICT data2, uint32_t * RESTRICT data3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16x4_t d0, d1, d2, d3, d4, d5, d6, d7;
    int32x4_t q9;
    uint32_t counter2, ofs1, ofs2;
    int16_t * RESTRICT pval2;
    uint16_t * RESTRICT pval3;

    d7 = vld1_dup_s16(params1);         // d7 = p1_0:p1_0:p1_0:p1_0
    d6 = vld1_dup_s16(params1 + 1);     // d6 = p1_1:p1_1:p1_1:p1_1
    d5 = vld1_dup_s16(params1 + 2);     // d5 = p1_2:p1_2:p1_2:p1_2

    do
    {
        d0 = vld1_s16((int16_t *)src);
        src += 8;
        counter2 = 0;

        do
        {
            pval2 = ((PTR32(int16_t) *)data2)[counter2];
            pval3 = ((PTR32(uint16_t) *)data3)[counter2];
            ofs1 = *pval3;
            ofs2 = data1[counter2];

            if ((ofs2 & 2) == 0)
            {
                if ((ofs2 & 1) == 0)
                {
                    vst1_s16(pval2 + ofs1, d0);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 > ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d0 = vld1_s16(pval2 + ofs1);
                    *pval3 = ofs1;
                }
                else
                {
                    d2 = vld1_s16(pval2 + ofs2 + 3);
                    d1 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d0), 16));
                    d2 = vqadd_s16(d2, d1);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d0), 48));
                    vst1_s16(pval2 + ofs1, d2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d0 = vld1_s16(pval2 + ofs1);
                    vst1_s16(pval2 + ofs2 + 3, d1);
                    *pval3 = ofs1;
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    d2 = vld1_s16(pval2 + ofs2 + 2);
                    d1 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d0), 32));
                    d2 = vqadd_s16(d2, d1);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d0), 32));
                    vst1_s16(pval2 + ofs1, d2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d0 = vld1_s16(pval2 + ofs1);
                    vst1_s16(pval2 + ofs2 + 2, d1);
                    *pval3 = ofs1;
                }
                else
                {
                    d2 = vld1_s16(pval2 + ofs2 + 1);
                    d1 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d0), 48));
                    d2 = vqadd_s16(d2, d1);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d0), 16));
                    vst1_s16(pval2 + ofs1, d2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d0 = vld1_s16(pval2 + ofs1);
                    vst1_s16(pval2 + ofs2 + 1, d1);
                    *pval3 = ofs1;
                }
            }

            counter2++;
        } while (counter2 < 1);

        d4 = vld1_dup_s16((int16_t *)params2);
        d2 = vqsub_s16(d0, d4);
        q9 = vmull_s16(d2, d6);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d4 = vadd_s16(d4, d2);
        d0 = vext_s16(d0, d4, 1);
        d2 = vqsub_s16(d0, d4);
        q9 = vmull_s16(d2, d6);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d4 = vadd_s16(d4, d2);
        d0 = vext_s16(d0, d4, 1);
        d2 = vqsub_s16(d0, d4);
        q9 = vmull_s16(d2, d6);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d4 = vadd_s16(d4, d2);
        d0 = vext_s16(d0, d4, 1);
        d2 = vqsub_s16(d0, d4);
        q9 = vmull_s16(d2, d6);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d4 = vadd_s16(d4, d2);
        d0 = vext_s16(d0, d4, 1);
        vst1_lane_s16((int16_t *)params2, d4, 0);

        do
        {
            pval2 = ((PTR32(int16_t) *)data2)[counter2];
            pval3 = ((PTR32(uint16_t) *)data3)[counter2];
            ofs1 = *pval3;
            ofs2 = data1[counter2];

            if ((ofs2 & 2) == 0)
            {
                if ((ofs2 & 1) == 0)
                {
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 > ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d3 = vld1_s16(pval2 + ofs1);
                    q9 = vmull_s16(d3, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d2 = vqsub_s16(d0, d4);
                    q9 = vmull_s16(d2, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d0 = vqadd_s16(d3, d4);
                    if (ofs1 != 0)
                    {
                        vst1_s16(pval2 + ofs1 - 4, d2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        vst1_s16(pval2 + ofs2, d2);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    d2 = vld1_s16(pval2 + ofs2 + 3);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d3 = vld1_s16(pval2 + ofs1);
                    q9 = vmull_s16(d3, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d1 = vqsub_s16(d0, d4);
                    q9 = vmull_s16(d1, d7);
                    d0 = vshrn_n_s32(q9, 16);
                    d0 = vshl_n_s16(d0, 1);
                    d4 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d1), 16));
                    d0 = vqadd_s16(d3, d0);
                    d2 = vqadd_s16(d2, d4);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d1), 48));
                    if (ofs1 != 0)
                    {
                        vst1_s16(pval2 + ofs1 - 4, d2);
                        vst1_s16(pval2 + ofs2 + 3, d1);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        vst1_s16(pval2 + ofs2 - 1, d2);
                        vst1_s16(pval2 + ofs2 + 3, d1);
                        *pval3 = ofs1;
                    }
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    d2 = vld1_s16(pval2 + ofs2 + 2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d3 = vld1_s16(pval2 + ofs1);
                    q9 = vmull_s16(d3, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d1 = vqsub_s16(d0, d4);
                    q9 = vmull_s16(d1, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d0 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d1), 32));
                    d2 = vqadd_s16(d2, d0);
                    d0 = vqadd_s16(d3, d4);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d1), 32));
                    if (ofs1 != 0)
                    {
                        vst1_s16(pval2 + ofs1 - 4, d2);
                        vst1_s16(pval2 + ofs2 + 2, d1);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        vst1_s16(pval2 + ofs2 - 2, d2);
                        vst1_s16(pval2 + ofs2 + 2, d1);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    d2 = vld1_s16(pval2 + ofs2 + 1);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    d3 = vld1_s16(pval2 + ofs1);
                    q9 = vmull_s16(d3, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d1 = vqsub_s16(d0, d4);
                    q9 = vmull_s16(d1, d7);
                    d4 = vshrn_n_s32(q9, 16);
                    d4 = vshl_n_s16(d4, 1);
                    d0 = vqadd_s16(d3, d4);
                    d3 = vreinterpret_s16_u64(vshl_n_u64(vreinterpret_u64_s16(d1), 48));
                    d2 = vqadd_s16(d2, d3);
                    d1 = vreinterpret_s16_u64(vshr_n_u64(vreinterpret_u64_s16(d1), 16));
                    if (ofs1 != 0)
                    {
                        vst1_s16(pval2 + ofs1 - 4, d2);
                        vst1_s16(pval2 + ofs2 + 1, d1);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        vst1_s16(pval2 + ofs2 - 3, d2);
                        vst1_s16(pval2 + ofs2 + 1, d1);
                        *pval3 = ofs1;
                    }
                }
            }

            counter2++;
        } while (counter2 < 5);

        q9 = vmull_s16(d0, d5);
        d0 = vshrn_n_s32(q9, 16);
        d0 = vshl_n_s16(d0, 1);
        vst1_s16((int16_t *)dst, d0);
        dst += 8;
        counter1 -= 4;
    } while (counter1 > 0);
}

// chorus

void CCALL simd_1000B2EE_c(uint8_t * RESTRICT params1, uint8_t * RESTRICT params2, uint16_t * RESTRICT data, uint8_t * RESTRICT params3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16x4_t d0, d1, d2, d3, d4, d5, d6, d7, d8;
    int32x4_t q9;
    int32_t counter, p2_0, shift;
    uint32_t length, offset, ofs2;
    uint16_t * RESTRICT poffs, * RESTRICT pval;

    d5 = vld1_dup_s16((int16_t *)(params2 + 4));    // d5 = p2_4:p2_4:p2_4:p2_4
    d4 = vld1_dup_s16((int16_t *)(params2 + 6));    // d4 = p2_6:p2_6:p2_6:p2_6
    p2_0 = *(int16_t *)params2;
    shift = *(uint16_t *)(params1 + 8);
    d7 = vmov_n_s16(p2_0 >> shift);                 // d7 = (p2_0 >> shift):(p2_0 >> shift):(p2_0 >> shift):(p2_0 >> shift)
    d7 = vset_lane_s16(2 * (p2_0 >> shift), d7, 1);
    d7 = vset_lane_s16(3 * (p2_0 >> shift), d7, 2);
    d7 = vset_lane_s16(4 * (p2_0 >> shift), d7, 3); // d7 = 4 * (p2_0 >> shift):3 * (p2_0 >> shift):2 * (p2_0 >> shift):(p2_0 >> shift)
    d6 = vld1_dup_s16((int16_t *)(params2 + 2));    // d6 = p2_2:p2_2:p2_2:p2_2
    d3 = vld1_dup_s16((int16_t *)(params2 + 8));    // d3 = p2_8:p2_8:p2_8:p2_8
    length = *(uint32_t *)(params1 + 4);
    counter = *(int32_t *)params1;
    poffs = *(PTR32(uint16_t) *)params3;
    pval = *(PTR32(uint16_t) *)(params3 + 4);
    offset = *poffs;

    do
    {
        d1 = vld1_dup_s16((int16_t *)pval);     // d1 = *pval:*pval:*pval:*pval
        d2 = vadd_s16(d1, d7);                  // d2 = (*pval + d7[3]):(*pval + d7[2]):(*pval + d7[1]):(*pval + d7[0])
        vst1_lane_s16((int16_t *)pval, d2, 3);  // *pval = *pval + d7[3]
        d2 = vext_s16(d1, d2, 3);               // d2 = (*pval + d7[2]):(*pval + d7[1]):(*pval + d7[0]):*pval
        d1 = vabs_s16(d2);
        d1 = vshr_n_s16(d1, 1);
        d2 = vmov_n_s16(0x3fff);                // d2 = 3fff:3fff:3fff:3fff
        d1 = vsub_s16(d1, d2);
        d1 = vshl_n_s16(d1, 1);
        q9 = vmull_s16(d1, d6);
        d1 = vshrn_n_s32(q9, 16);
        d1 = vadd_s16(d1, d2);
        q9 = vmull_s16(d1, d5);
        d2 = vmovn_s32(q9);
        d1 = vshrn_n_s32(q9, 15);
        d0 = vmov_n_s16(4);                     // d0 = 0004:0004:0004:0004
        d1 = vmax_s16(d0, d1);                  // d1 = max(4, d1)
        d0 = vmov_n_s16(0x8000);                // d0 = 8000:8000:8000:8000
        d2 = vbic_s16(d2, d0);                  // d2 = d2 & 0x7fff

        ofs2 = offset - vget_lane_u16(vreinterpret_u16_s16(d1), 0);
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d1 = vld1_lane_s16((int16_t *)(data + ofs2), d1, 0);

        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d0 = vld1_lane_s16((int16_t *)(data + ofs2), d0, 0);

        ofs2 = offset + 1 - vget_lane_u16(vreinterpret_u16_s16(d1), 1);
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d1 = vld1_lane_s16((int16_t *)(data + ofs2), d1, 1);

        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d0 = vld1_lane_s16((int16_t *)(data + ofs2), d0, 1);

        ofs2 = offset + 2 - vget_lane_u16(vreinterpret_u16_s16(d1), 2);
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d1 = vld1_lane_s16((int16_t *)(data + ofs2), d1, 2);

        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d0 = vld1_lane_s16((int16_t *)(data + ofs2), d0, 2);

        ofs2 = offset + 3 - vget_lane_u16(vreinterpret_u16_s16(d1), 3);
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d1 = vld1_lane_s16((int16_t *)(data + ofs2), d1, 3);

        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d0 = vld1_lane_s16((int16_t *)(data + ofs2), d0, 3);

        d0 = vsub_s16(d0, d1);
        q9 = vmull_s16(d0, d2);
        d0 = vshrn_n_s32(q9, 16);
        d0 = vshl_n_s16(d0, 1);
        d1 = vadd_s16(d1, d0);

        d0 = vld1_s16((int16_t *)src);
        src += 8;
        d2 = d1;
        q9 = vmull_s16(d2, d4);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d0 = vqadd_s16(d0, d2);
        vst1_s16((int16_t *)(data + offset), d0);
        offset += 4;
        d0 = d1;

        if (UNLIKELY_COND(offset >= length)) UNLIKELY_ATTR
        {
            offset = 0;
        }

        d8 = vld1_s16((int16_t *)dst);
        q9 = vmull_s16(d0, d3);
        d0 = vshrn_n_s32(q9, 16);
        d0 = vshl_n_s16(d0, 1);
        d0 = vqadd_s16(d0, d8);
        vst1_s16((int16_t *)dst, d0);
        dst += 8;
        counter -= 4;
    } while (counter > 0);

    *poffs = offset;
}

void CCALL simd_1000B7C9_c(uint8_t * RESTRICT params1, uint8_t * RESTRICT params2, uint16_t * RESTRICT data, uint8_t * RESTRICT params3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16x4_t d0, d1, d2, d3, d4, d5, d6, d7;
    int32x4_t q9;
    int32_t counter, p2_0, shift;
    uint32_t length, offset, ofs2;
    uint16_t * RESTRICT poffs, * RESTRICT pval;

    d5 = vld1_dup_s16((int16_t *)(params2 + 4));    // d5 = p2_4:p2_4:p2_4:p2_4
    d4 = vld1_dup_s16((int16_t *)(params2 + 6));    // d4 = p2_6:p2_6:p2_6:p2_6
    p2_0 = *(int16_t *)params2;
    shift = *(uint16_t *)(params1 + 8);
    d7 = vmov_n_s16(p2_0 >> shift);                 // d7 = (p2_0 >> shift):(p2_0 >> shift):(p2_0 >> shift):(p2_0 >> shift)
    d7 = vset_lane_s16(2 * (p2_0 >> shift), d7, 1);
    d7 = vset_lane_s16(3 * (p2_0 >> shift), d7, 2);
    d7 = vset_lane_s16(4 * (p2_0 >> shift), d7, 3); // d7 = 4 * (p2_0 >> shift):3 * (p2_0 >> shift):2 * (p2_0 >> shift):(p2_0 >> shift)
    d6 = vld1_dup_s16((int16_t *)(params2 + 2));    // d6 = p2_2:p2_2:p2_2:p2_2
    d3 = vld1_dup_s16((int16_t *)(params2 + 8));    // d3 = p2_8:p2_8:p2_8:p2_8
    length = *(uint32_t *)(params1 + 4);
    counter = *(int32_t *)params1;
    poffs = *(PTR32(uint16_t) *)params3;
    pval = *(PTR32(uint16_t) *)(params3 + 4);
    offset = *poffs;

    do
    {
        d1 = vld1_dup_s16((int16_t *)pval);     // d1 = *pval:*pval:*pval:*pval
        d2 = vadd_s16(d1, d7);                  // d2 = (*pval + d7[3]):(*pval + d7[2]):(*pval + d7[1]):(*pval + d7[0])
        vst1_lane_s16((int16_t *)pval, d2, 3);  // *pval = *pval + d7[3]
        d2 = vext_s16(d1, d2, 3);               // d2 = (*pval + d7[2]):(*pval + d7[1]):(*pval + d7[0]):*pval
        d1 = vabs_s16(d2);
        d1 = vshr_n_s16(d1, 1);
        d2 = vmov_n_s16(0x3fff);                // d2 = 3fff:3fff:3fff:3fff
        d1 = vsub_s16(d1, d2);
        d1 = vshl_n_s16(d1, 1);
        q9 = vmull_s16(d1, d6);
        d1 = vshrn_n_s32(q9, 16);
        d1 = vadd_s16(d1, d2);
        q9 = vmull_s16(d1, d5);
        d2 = vmovn_s32(q9);
        d1 = vshrn_n_s32(q9, 15);
        d0 = vmov_n_s16(4);                     // d0 = 0004:0004:0004:0004
        d1 = vmax_s16(d0, d1);                  // d1 = max(4, d1)
        d0 = vmov_n_s16(0x8000);                // d0 = 8000:8000:8000:8000
        d2 = vbic_s16(d2, d0);                  // d2 = d2 & 0x7fff

        ofs2 = offset - vget_lane_u16(vreinterpret_u16_s16(d1), 0);
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d1 = vld1_lane_s16((int16_t *)(data + ofs2), d1, 0);

        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d0 = vld1_lane_s16((int16_t *)(data + ofs2), d0, 0);

        ofs2 = offset + 1 - vget_lane_u16(vreinterpret_u16_s16(d1), 1);
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d1 = vld1_lane_s16((int16_t *)(data + ofs2), d1, 1);

        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d0 = vld1_lane_s16((int16_t *)(data + ofs2), d0, 1);

        ofs2 = offset + 2 - vget_lane_u16(vreinterpret_u16_s16(d1), 2);
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d1 = vld1_lane_s16((int16_t *)(data + ofs2), d1, 2);

        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d0 = vld1_lane_s16((int16_t *)(data + ofs2), d0, 2);

        ofs2 = offset + 3 - vget_lane_u16(vreinterpret_u16_s16(d1), 3);
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d1 = vld1_lane_s16((int16_t *)(data + ofs2), d1, 3);

        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        d0 = vld1_lane_s16((int16_t *)(data + ofs2), d0, 3);

        d0 = vsub_s16(d0, d1);
        q9 = vmull_s16(d0, d2);
        d0 = vshrn_n_s32(q9, 16);
        d0 = vshl_n_s16(d0, 1);
        d1 = vadd_s16(d1, d0);

        d0 = vld1_s16((int16_t *)src);
        src += 8;
        d2 = d1;
        q9 = vmull_s16(d2, d4);
        d2 = vshrn_n_s32(q9, 16);
        d2 = vshl_n_s16(d2, 1);
        d0 = vqadd_s16(d0, d2);
        vst1_s16((int16_t *)(data + offset), d0);
        offset += 4;
        d0 = d1;

        if (UNLIKELY_COND(offset >= length)) UNLIKELY_ATTR
        {
            offset = 0;
        }

        q9 = vmull_s16(d0, d3);
        d0 = vshrn_n_s32(q9, 16);
        d0 = vshl_n_s16(d0, 1);
        vst1_s16((int16_t *)dst, d0);
        dst += 8;
        counter -= 4;
    } while (counter > 0);

    *poffs = offset;
}