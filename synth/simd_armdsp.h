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

#if defined(__ARM_ACLE) && (__ARM_32BIT_STATE | __ARM_FEATURE_SIMD32 | __ARM_FEATURE_SAT)
#include <arm_acle.h>
#endif


void CCALL simd_10007028_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint32_t mult4, uint32_t mult5, uint8_t * RESTRICT tmp, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    uint32_t r5, r7;
    int32_t r0, r1, r2, r3, r6, r8, r9, r10;

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
    r7 = (uint16_t)mult1 | (((uint16_t)mult2) << 16);   // r7 = mult2:mult1
    r5 = (uint16_t)mult3 | (((uint16_t)mult4) << 16);   // r5 = mult4:mult3
#else
    asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r7) : [low1] "r" (mult1) , [low2] "r" (mult2) );
    asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r5) : [low1] "r" (mult3) , [low2] "r" (mult4) );
#endif
    r3 = (int16_t)mult5;

    do
    {
        r6 = *(int16_t *)src;
        src += 2;
        r8 = *(int16_t *)dst2;
        r9 = *(int16_t *)dst3;
        r10 = *(int32_t *)dst1;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r6 = __builtin_arm_smulwb(r6, r7); // r6 = [src] * mult1
        r0 = __builtin_arm_smulwt(r6, r7); // r0 = [src] * mult1 * mult2
        r1 = __builtin_arm_smulwb(r6, r5); // r1 = [src] * mult1 * mult3
        r2 = __builtin_arm_smulwt(r6, r5); // r2 = [src] * mult1 * mult4
        r6 = __builtin_arm_smulwb(r6, r3); // r6 = [src] * mult1 * mult5
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r6) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smulwt %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r1) : [src1] "r" (r6) , [src2] "r" (r5) );
        asm ( "smulwt %[result], %[src1], %[src2]" : [result] "=r" (r2) : [src1] "r" (r6) , [src2] "r" (r5) );
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r6) : [src1] "r" (r6) , [src2] "r" (r3) );
#endif

        r0 += r0;   // r0 = ([src] * mult1 * mult2) << 1
        r1 += r1;   // r1 = ([src] * mult1 * mult3) << 1
        r2 += r2;   // r2 = ([src] * mult1 * mult4) << 1
        r6 += r6;   // r6 = ([src] * mult1 * mult5) << 1

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r0 = (r0 & UINT32_C(0x0000ffff)) | (r1 << 16);

        r2 = __qadd16(r2, r8);
        r6 = __qadd16(r6, r9);
        r0 = __qadd16(r0, r10);
#else
        asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r0) : [low1] "r" (r0) , [low2] "r" (r1) );

        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2) : [src1] "r" (r2) , [src2] "r" (r8) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r6) : [src1] "r" (r6) , [src2] "r" (r9) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (r10) : "cc" );
#endif

        *(int16_t *)dst2 = r2;
        *(int16_t *)dst3 = r6;
        *(int32_t *)dst1 = r0;
        dst1 += 4;
        dst2 += 2;
        dst3 += 2;
        counter--;
    } while (counter > 0);
}

void CCALL simd_10007104_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint32_t mult4, uint32_t mult5, uint8_t * RESTRICT tmp, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    uint32_t r5, r7;
    int32_t r0, r1, r2, r3, r6;

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
    r7 = (uint16_t)mult1 | (((uint16_t)mult2) << 16);   // r7 = mult2:mult1
    r5 = (uint16_t)mult3 | (((uint16_t)mult4) << 16);   // r5 = mult4:mult3
#else
    asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r7) : [low1] "r" (mult1) , [low2] "r" (mult2) );
    asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r5) : [low1] "r" (mult3) , [low2] "r" (mult4) );
#endif
    r3 = (int16_t)mult5;

    do
    {
        r6 = *(int16_t *)src;
        src += 2;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r6 = __builtin_arm_smulwb(r6, r7); // r6 = [src] * mult1
        r0 = __builtin_arm_smulwt(r6, r7); // r0 = [src] * mult1 * mult2
        r1 = __builtin_arm_smulwb(r6, r5); // r1 = [src] * mult1 * mult3
        r2 = __builtin_arm_smulwt(r6, r5); // r2 = [src] * mult1 * mult4
        r6 = __builtin_arm_smulwb(r6, r3); // r6 = [src] * mult1 * mult5
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r6) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smulwt %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r1) : [src1] "r" (r6) , [src2] "r" (r5) );
        asm ( "smulwt %[result], %[src1], %[src2]" : [result] "=r" (r2) : [src1] "r" (r6) , [src2] "r" (r5) );
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r6) : [src1] "r" (r6) , [src2] "r" (r3) );
#endif

        r0 += r0;   // r0 = ([src] * mult1 * mult2) << 1
        r1 += r1;   // r1 = ([src] * mult1 * mult3) << 1
        r2 += r2;   // r2 = ([src] * mult1 * mult4) << 1
        r6 += r6;   // r6 = ([src] * mult1 * mult5) << 1

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r0 = (r0 & UINT32_C(0x0000ffff)) | (r1 << 16);
#else
        asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r0) : [low1] "r" (r0) , [low2] "r" (r1) );
#endif

        *(int16_t *)dst2 = r2;
        *(int16_t *)dst3 = r6;
        *(int32_t *)dst1 = r0;
        dst1 += 4;
        dst2 += 2;
        dst3 += 2;
        counter--;
    } while (counter > 0);
}

void CCALL simd_100071D0_c(int32_t counter, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    uint64_t r0;

    r0 = 0;

    do
    {
        *(uint64_t *)dst1 = r0;
        *(uint64_t *)dst2 = r0;
        dst1 += 8;
        dst2 += 8;
        *(uint64_t *)dst1 = r0;
        *(uint64_t *)dst3 = r0;
        dst1 += 8;
        dst3 += 8;
        counter -= 4;
    } while (counter > 0);
}

void CCALL simd_1000720B_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    uint32_t r2, r3, r6, r7, r8, r9, r10;
    int32_t r4;
#if !(defined(__ARM_ACLE) && __ARM_32BIT_STATE)
    uint32_t r11, r12;
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
    r7 = (uint16_t)mult1 | (((uint16_t)mult2) << 16);   // r7 = mult2:mult1
#else
    asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r7) : [low1] "r" (mult1) , [low2] "r" (mult2) );
#endif
    r4 = (int16_t)mult3;

    do
    {
        r6 = *(uint32_t *)src;
        src += 4;
        r8 = *(uint32_t *)dst2;
        r9 = *(uint32_t *)dst3;
        r10 = *(uint32_t *)dst1;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r6 = ((__builtin_arm_smulbb(r6, r7) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r6, r7) & UINT32_C(0xffff0000)); // r6 = [src] * mult1
        r2 = ((__builtin_arm_smulbt(r6, r7) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultt(r6, r7) & UINT32_C(0xffff0000)); // r2 = [src] * mult1 * mult2
        r3 = ((__builtin_arm_smulbb(r6, r4) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r6, r4) & UINT32_C(0xffff0000)); // r3 = [src] * mult1 * mult3
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r12) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r6) : [high1] "r" (r12) , [high2] "r" (r11) );

        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smultt %[result], %[src1], %[src2]" : [result] "=r" (r12) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r2) : [high1] "r" (r12) , [high2] "r" (r11) );

        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r4) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r12) : [src1] "r" (r6) , [src2] "r" (r4) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r3) : [high1] "r" (r12) , [high2] "r" (r11) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r2 = ((uint32_t)__sadd16(r2, r2));  // r2 = ([src] * mult1 * mult2) << 1
        r3 = ((uint32_t)__sadd16(r3, r3));  // r3 = ([src] * mult1 * mult3) << 1

        r2 = __qadd16(r2, r8);
        r3 = __qadd16(r3, r9);
        r6 = __qadd16(r6, r10);
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r2) : [src] "r" (r2) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r3) : [src] "r" (r3) : "cc" );

        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2) : [src1] "r" (r2) , [src2] "r" (r8) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r3) : [src1] "r" (r3) , [src2] "r" (r9) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r6) : [src1] "r" (r6) , [src2] "r" (r10) : "cc" );
#endif

        *(uint32_t *)dst2 = r2;
        *(uint32_t *)dst3 = r3;
        *(uint32_t *)dst1 = r6;
        dst1 += 4;
        dst2 += 4;
        dst3 += 4;
        counter -= 2;
    } while (counter > 0);
}

void CCALL simd_100072A3_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    uint32_t r2, r3, r6, r7;
    int32_t r4;
#if !(defined(__ARM_ACLE) && __ARM_32BIT_STATE)
    uint32_t r11, r12;
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
    r7 = (uint16_t)mult1 | (((uint16_t)mult2) << 16);   // r7 = mult2:mult1
#else
    asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r7) : [low1] "r" (mult1) , [low2] "r" (mult2) );
#endif
    r4 = (int16_t)mult3;

    do
    {
        r6 = *(uint32_t *)src;
        src += 4;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r6 = ((__builtin_arm_smulbb(r6, r7) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r6, r7) & UINT32_C(0xffff0000)); // r6 = [src] * mult1
        r2 = ((__builtin_arm_smulbt(r6, r7) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultt(r6, r7) & UINT32_C(0xffff0000)); // r2 = [src] * mult1 * mult2
        r3 = ((__builtin_arm_smulbb(r6, r4) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r6, r4) & UINT32_C(0xffff0000)); // r3 = [src] * mult1 * mult3
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r12) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r6) : [high1] "r" (r12) , [high2] "r" (r11) );

        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smultt %[result], %[src1], %[src2]" : [result] "=r" (r12) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r2) : [high1] "r" (r12) , [high2] "r" (r11) );

        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r4) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r12) : [src1] "r" (r6) , [src2] "r" (r4) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r3) : [high1] "r" (r12) , [high2] "r" (r11) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r2 = ((uint32_t)__sadd16(r2, r2));  // r2 = ([src] * mult1 * mult2) << 1
        r3 = ((uint32_t)__sadd16(r3, r3));  // r3 = ([src] * mult1 * mult3) << 1
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r2) : [src] "r" (r2) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r3) : [src] "r" (r3) : "cc" );
#endif

        *(uint32_t *)dst2 = r2;
        *(uint32_t *)dst3 = r3;
        *(uint32_t *)dst1 = r6;
        dst1 += 4;
        dst2 += 4;
        dst3 += 4;
        counter -= 2;
    } while (counter > 0);
}

void CCALL simd_1000732F_c(int32_t counter, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    uint64_t r0;

    r0 = 0;

    do
    {
        *(uint64_t *)dst1 = r0;
        *(uint64_t *)dst2 = r0;
        dst1 += 8;
        dst2 += 8;
        *(uint64_t *)dst3 = r0;
        dst3 += 8;
        counter -= 4;
    } while (counter > 0);
}

void CCALL simd_10007364_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst)
{
    uint32_t r0, r1, r6, r7;
    int32_t r3;
    uint32_t r10, r11;
    uint64_t r8, r9;

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
    r7 = (uint16_t)mult1 | (((uint16_t)mult2) << 16);   // r7 = mult2:mult1
#else
    asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r7) : [low1] "r" (mult1) , [low2] "r" (mult2) );
#endif
    r3 = (int16_t)mult3;

    do
    {
        r6 = *(uint32_t *)src;
        src += 4;
        r8 = *(uint64_t *)dst;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r6 = ((__builtin_arm_smulbb(r6, r7) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r6, r7) & UINT32_C(0xffff0000)); // r6 = [src] * mult1
        r0 = ((__builtin_arm_smulbt(r6, r7) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultt(r6, r7) & UINT32_C(0xffff0000)); // r0 = [src] * mult1 * mult2
        r1 = ((__builtin_arm_smulbb(r6, r3) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r6, r3) & UINT32_C(0xffff0000)); // r1 = [src] * mult1 * mult3
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r6) : [high1] "r" (r11) , [high2] "r" (r10) );

        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smultt %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0) : [high1] "r" (r11) , [high2] "r" (r10) );

        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r6) , [src2] "r" (r3) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r3) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r1) : [high1] "r" (r11) , [high2] "r" (r10) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r0 = ((uint32_t)__sadd16(r0, r0));  // r0 = ([src] * mult1 * mult2) << 1
        r1 = ((uint32_t)__sadd16(r1, r1));  // r1 = ([src] * mult1 * mult3) << 1

        r10 = (r0 & UINT32_C(0x0000ffff)) | (r1 << 16);
        r11 = (r1 & UINT32_C(0xffff0000)) | ((r0 >> 16) & UINT32_C(0x0000ffff));

        r10 = __qadd16(r10, r8);
        r11 = __qadd16(r11, r8 >> 32);
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0) : [src] "r" (r0) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r1) : [src] "r" (r1) : "cc" );

        asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r10) : [low1] "r" (r0) , [low2] "r" (r1) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r11) : [high1] "r" (r1) , [high2] "r" (r0) );

        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r10) , [src2] "r" (r8) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r11) , [src2] "r" (r8 >> 32) : "cc" );
#endif

        r9 = r10 | (((uint64_t)r11) << 32);
        *(uint64_t *)dst = r9;
        dst += 8;
        counter -= 2;
    } while (counter > 0);
}

void CCALL simd_100073F1_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst)
{
    uint32_t r0, r1, r6, r7;
    int32_t r3;
    uint32_t r10, r11;
    uint64_t r9;

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
    r7 = (uint16_t)mult1 | (((uint16_t)mult2) << 16);   // r7 = mult2:mult1
#else
    asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r7) : [low1] "r" (mult1) , [low2] "r" (mult2) );
#endif
    r3 = (int16_t)mult3;

    do
    {
        r6 = *(uint32_t *)src;
        src += 4;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r6 = ((__builtin_arm_smulbb(r6, r7) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r6, r7) & UINT32_C(0xffff0000)); // r6 = [src] * mult1
        r0 = ((__builtin_arm_smulbt(r6, r7) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultt(r6, r7) & UINT32_C(0xffff0000)); // r0 = [src] * mult1 * mult2
        r1 = ((__builtin_arm_smulbb(r6, r3) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r6, r3) & UINT32_C(0xffff0000)); // r1 = [src] * mult1 * mult3
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r6) : [high1] "r" (r11) , [high2] "r" (r10) );

        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "smultt %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r7) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0) : [high1] "r" (r11) , [high2] "r" (r10) );

        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r6) , [src2] "r" (r3) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r6) , [src2] "r" (r3) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r1) : [high1] "r" (r11) , [high2] "r" (r10) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r0 = ((uint32_t)__sadd16(r0, r0));  // r0 = ([src] * mult1 * mult2) << 1
        r1 = ((uint32_t)__sadd16(r1, r1));  // r1 = ([src] * mult1 * mult3) << 1

        r10 = (r0 & UINT32_C(0x0000ffff)) | (r1 << 16);
        r11 = (r1 & UINT32_C(0xffff0000)) | ((r0 >> 16) & UINT32_C(0x0000ffff));
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0) : [src] "r" (r0) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r1) : [src] "r" (r1) : "cc" );

        asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r10) : [low1] "r" (r0) , [low2] "r" (r1) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r11) : [high1] "r" (r1) , [high2] "r" (r0) );
#endif

        r9 = r10 | (((uint64_t)r11) << 32);
        *(uint64_t *)dst = r9;
        dst += 8;
        counter -= 2;
    } while (counter > 0);
}

void CCALL simd_10007472_c(int32_t counter, uint8_t * RESTRICT dst)
{
    uint64_t r0;

    r0 = 0;

    do
    {
        *(uint64_t *)dst = r0;
        dst += 8;
        counter -= 2;
    } while (counter > 0);
}

void CCALL simd_10007491_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult, uint8_t * RESTRICT dst)
{
    uint64_t r0, r10;
    int32_t r7;
    uint32_t r8, r9;
#if !(defined(__ARM_ACLE) && __ARM_32BIT_STATE)
    uint32_t r11, r12;
#endif

    r7 = (int16_t)mult;

    do
    {
        r0 = *(uint64_t *)src;
        r10 = *(uint64_t *)dst;
        r8 = r0;
        r9 = r0 >> 32;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r8 = (__builtin_arm_smulwb(r7, r8) & UINT32_C(0x0000ffff)) | (__builtin_arm_smulbt(r7, r8) & UINT32_C(0xffff0000));
        r9 = (__builtin_arm_smulwb(r7, r9) & UINT32_C(0x0000ffff)) | (__builtin_arm_smulbt(r7, r9) & UINT32_C(0xffff0000));
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r7) , [src2] "r" (r8) );
        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r12) : [src1] "r" (r7) , [src2] "r" (r8) );
        asm ( "pkhbt %[result], %[low], %[high]" : [result] "=r" (r8) : [low] "r" (r11) , [high] "r" (r12) );

        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r7) , [src2] "r" (r9) );
        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r12) : [src1] "r" (r7) , [src2] "r" (r9) );
        asm ( "pkhbt %[result], %[low], %[high]" : [result] "=r" (r9) : [low] "r" (r11) , [high] "r" (r12) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r8 = __qadd16(r8, r10);
        r9 = __qadd16(r9, r10 >> 32);
#else
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r8) , [src2] "r" (r10) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r9) , [src2] "r" (r10 >> 32) : "cc" );
#endif

        r0 = r8 | (((uint64_t)r9) << 32);
        *(uint64_t *)dst = r0;
        src += 8;
        dst += 8;
        counter -= 4;
    } while (counter > 0);
}

void CCALL simd_100074CD_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult, uint8_t * RESTRICT dst)
{
    uint64_t r0;
    int32_t r7;
    uint32_t r8, r9;
#if !(defined(__ARM_ACLE) && __ARM_32BIT_STATE)
    uint32_t r10, r11;
#endif

    r7 = (int16_t)mult;

    do
    {
        r0 = *(uint64_t *)src;
        r8 = r0;
        r9 = r0 >> 32;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r8 = (__builtin_arm_smulwb(r7, r8) & UINT32_C(0x0000ffff)) | (__builtin_arm_smulbt(r7, r8) & UINT32_C(0xffff0000));
        r9 = (__builtin_arm_smulwb(r7, r9) & UINT32_C(0x0000ffff)) | (__builtin_arm_smulbt(r7, r9) & UINT32_C(0xffff0000));
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r7) , [src2] "r" (r8) );
        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r7) , [src2] "r" (r8) );
        asm ( "pkhbt %[result], %[low], %[high]" : [result] "=r" (r8) : [low] "r" (r10) , [high] "r" (r11) );

        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r7) , [src2] "r" (r9) );
        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r7) , [src2] "r" (r9) );
        asm ( "pkhbt %[result], %[low], %[high]" : [result] "=r" (r9) : [low] "r" (r10) , [high] "r" (r11) );
#endif

        r0 = r8 | (((uint64_t)r9) << 32);
        *(uint64_t *)dst = r0;
        src += 8;
        dst += 8;
        counter -= 4;
    } while (counter > 0);
}

void CCALL simd_10007506_c(int32_t counter, uint8_t * RESTRICT dst)
{
    uint64_t r0;

    r0 = 0;

    do
    {
        *(uint64_t *)dst = r0;
        dst += 8;
        counter -= 4;
    } while (counter > 0);
}

void CCALL simd_10007525_c(int32_t counter1, int32_t multadd2, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    uint32_t r0, r1, r2;
    uint64_t r8;
    int32_t counter2;

    multadd2 = (multadd2 << 1) - 1;
    if (multadd2 >= 0)
    {
        do
        {
            r0 = *(uint32_t *)src;
            r8 = *(uint64_t *)dst;
            counter2 = multadd2;
            r2 = r0;
            src += 4;

            do
            {
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                r0 = __qadd16(r0, r2);
#else
                asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (r2) : "cc" );
#endif

                counter2--;
            } while (counter2 > 0);

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
            r1 = (r0 & UINT32_C(0xffff0000)) | ((r0 >> 16) & UINT32_C(0x0000ffff));
            r0 = (r0 & UINT32_C(0x0000ffff)) | (r0 << 16);
#else
            asm ( "pkhtb %[result], %[high], %[high], asr #16" : [result] "=r" (r1) : [high] "r" (r0) );
            asm ( "pkhbt %[result], %[low], %[low], lsl #16" : [result] "=r" (r0) : [low] "r" (r0) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
            r0 = __qadd16(r0, r8);
            r1 = __qadd16(r1, r8 >> 32);
#else
            asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (r8) : "cc" );
            asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r1) : [src1] "r" (r1) , [src2] "r" (r8 >> 32) : "cc" );
#endif
            r8 = r0 | (((uint64_t)r1) << 32);
            *(uint64_t *)dst = r8;
            dst += 8;
            counter1 -= 2;
        } while (counter1 > 0);
    }
    else
    {
        do
        {
            r0 = *(uint32_t *)src;
            r8 = *(uint64_t *)dst;

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
            r1 = (r0 & UINT32_C(0xffff0000)) | ((r0 >> 16) & UINT32_C(0x0000ffff));
            r0 = (r0 & UINT32_C(0x0000ffff)) | (r0 << 16);
#else
            asm ( "pkhtb %[result], %[high], %[high], asr #16" : [result] "=r" (r1) : [high] "r" (r0) );
            asm ( "pkhbt %[result], %[low], %[low], lsl #16" : [result] "=r" (r0) : [low] "r" (r0) );
#endif

            src += 4;

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
            r0 = __qadd16(r0, r8);
            r1 = __qadd16(r1, r8 >> 32);
#else
            asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (r8) : "cc" );
            asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r1) : [src1] "r" (r1) , [src2] "r" (r8 >> 32) : "cc" );
#endif
            r8 = r0 | (((uint64_t)r1) << 32);
            *(uint64_t *)dst = r8;
            dst += 8;
            counter1 -= 2;
        } while (counter1 > 0);
    }
}

void CCALL simd_1000758D_c(int32_t counter1, int32_t multadd2, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    uint64_t r0, r1, r8;
    int32_t counter2;
#if !(defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32)
    uint32_t r10, r11;
#endif

    multadd2 = (multadd2 << 1) - 1;
    if (multadd2 >= 0)
    {
        do
        {
            r0 = *(uint64_t *)src;
            r8 = *(uint64_t *)dst;
            counter2 = multadd2;
            r1 = r0;
            src += 8;

            do
            {
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                r0 = ((uint32_t)__qadd16(r0, r1)) | (((uint64_t)(uint32_t)__qadd16(r0 >> 32, r1 >> 32)) << 32);
#else
                r10 = r0;
                r11 = r0 >> 32;
                asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r10) , [src2] "r" (r1) : "cc" );
                asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r11) , [src2] "r" (r1 >> 32) : "cc" );
                r0 = r10 | (((uint64_t)r11) << 32);
#endif
                counter2--;
            } while (counter2 > 0);

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
            r0 = ((uint32_t)__qadd16(r0, r8)) | (((uint64_t)(uint32_t)__qadd16(r0 >> 32, r8 >> 32)) << 32);
#else
            r10 = r0;
            r11 = r0 >> 32;
            asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r10) , [src2] "r" (r8) : "cc" );
            asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r11) , [src2] "r" (r8 >> 32) : "cc" );
            r0 = r10 | (((uint64_t)r11) << 32);
#endif
            *(uint64_t *)dst = r0;
            dst += 8;
            counter1 -= 4;
        } while (counter1 > 0);
    }
    else
    {
        do
        {
            r0 = *(uint64_t *)src;
            r8 = *(uint64_t *)dst;
            src += 8;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
            r0 = ((uint32_t)__qadd16(r0, r8)) | (((uint64_t)(uint32_t)__qadd16(r0 >> 32, r8 >> 32)) << 32);
#else
            r10 = r0;
            r11 = r0 >> 32;
            asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r10) : [src1] "r" (r10) , [src2] "r" (r8) : "cc" );
            asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r11) : [src1] "r" (r11) , [src2] "r" (r8 >> 32) : "cc" );
            r0 = r10 | (((uint64_t)r11) << 32);
#endif
            *(uint64_t *)dst = r0;
            dst += 8;
            counter1 -= 4;
        } while (counter1 > 0);
    }
}

void CCALL simd_100075CC_c(int32_t counter, uint32_t shift, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    uint64_t r0;
    uint32_t r7, r8, r9, mult;

    r7 = (int16_t)shift - 1;

    if (cs_volume_multiplier == 8192)
    {
        do
        {
            r0 = *(uint64_t *)src;
            r8 = r0;
            r9 = r0 >> 32;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
            r8 = ((((int32_t)(int16_t)r8) >> r7) & UINT32_C(0x0000ffff)) | ((((int32_t)r8) >> r7) & UINT32_C(0xffff0000));
            r9 = ((((int32_t)(int16_t)r9) >> r7) & UINT32_C(0x0000ffff)) | ((((int32_t)r9) >> r7) & UINT32_C(0xffff0000));
#else
            asm ( "pkhbt %[result], %[low], %[high]" : [result] "=r" (r8) : [low] "r" (((int32_t)(int16_t)r8) >> r7) , [high] "r" (((int32_t)r8) >> r7) );
            asm ( "pkhbt %[result], %[low], %[high]" : [result] "=r" (r9) : [low] "r" (((int32_t)(int16_t)r9) >> r7) , [high] "r" (((int32_t)r9) >> r7) );
#endif
            r0 = r8 | (((uint64_t)r9) << 32);
            *(uint64_t *)dst = r0;
            dst += 8;
            src += 8;
            counter -= 4;
        } while (counter > 0);
    }
    else
    {
        mult = cs_volume_multiplier >> r7;

        do
        {
            r0 = *(uint64_t *)src;
            r8 = r0;
            r9 = r0 >> 32;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
            r7 = __builtin_arm_smulbb(r8, mult);
            r8 = __builtin_arm_smultb(r8, mult);
#else
            asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r7) : [src1] "r" (r8) , [src2] "r" (mult) );
            asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r8) , [src2] "r" (mult) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SAT
            r7 = __ssat(((int32_t)r7) >> 11, 16);
            r8 = __ssat(((int32_t)r8) >> 11, 16);
            r8 = (r7 & UINT32_C(0x0000ffff)) | ((r8 & UINT32_C(0x0000ffff)) << 16);
#else
            asm ( "ssat %[result], #16, %[value], asr #11" : [result] "=r" (r7) : [value] "r" (r7) : "cc" );
            asm ( "ssat %[result], #16, %[value], asr #11" : [result] "=r" (r8) : [value] "r" (r8) : "cc" );
            asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r8) : [low1] "r" (r7) , [low2] "r" (r8) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
            r7 = __builtin_arm_smulbb(r9, mult);
            r9 = __builtin_arm_smultb(r9, mult);
#else
            asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r7) : [src1] "r" (r9) , [src2] "r" (mult) );
            asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r9) , [src2] "r" (mult) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SAT
            r7 = __ssat(((int32_t)r7) >> 11, 16);
            r9 = __ssat(((int32_t)r9) >> 11, 16);
            r9 = (r7 & UINT32_C(0x0000ffff)) | ((r9 & UINT32_C(0x0000ffff)) << 16);
#else
            asm ( "ssat %[result], #16, %[value], asr #11" : [result] "=r" (r7) : [value] "r" (r7) : "cc" );
            asm ( "ssat %[result], #16, %[value], asr #11" : [result] "=r" (r9) : [value] "r" (r9) : "cc" );
            asm ( "pkhbt %[result], %[low1], %[low2], lsl #16" : [result] "=r" (r9) : [low1] "r" (r7) , [low2] "r" (r9) );
#endif

            r0 = r8 | (((uint64_t)r9) << 32);
            *(uint64_t *)dst = r0;
            dst += 8;
            src += 8;
            counter -= 4;
        } while (counter > 0);
    }
}

// reverb

void CCALL simd_1000A920_c(int32_t counter1, int16_t * RESTRICT params1, uint16_t * RESTRICT params2, uint16_t * RESTRICT data1, uint32_t * RESTRICT data2, uint32_t * RESTRICT data3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    union {
        int16_t h[4];
        uint32_t w[2];
        uint64_t d;
    } r0, r1, r2, r3;
    uint32_t counter2, ofs1, ofs2, index;
    int32_t tmp1, p1_0, p1_1, p1_2;
#if !(defined(__ARM_ACLE) && __ARM_32BIT_STATE)
    int32_t tmp2;
#endif
    uint16_t *pval2, *pval3;

    p1_0 = params1[0];
    p1_1 = params1[1];
    p1_2 = params1[2];

    do
    {
        r0.d = *(uint64_t *)src;
        src += 8;
        counter2 = 0;

        do
        {
            pval2 = ((PTR32(uint16_t) *)data2)[counter2];
            pval3 = ((PTR32(uint16_t) *)data3)[counter2];
            ofs1 = *pval3;
            ofs2 = data1[counter2];

            if ((ofs2 & 2) == 0)
            {
                if ((ofs2 & 1) == 0)
                {
                    unaligned_write_64(pval2 + ofs1, r0.d);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 > ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r0.d = unaligned_read_64(pval2 + ofs1);
                    *pval3 = ofs1;
                }
                else
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 3);
                    r1.w[0] = r0.w[1] >> 16;
                    r0.d = r0.d << 16;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[0] = __qadd16(r2.w[0], r0.w[0]);
                    r2.w[1] = __qadd16(r2.w[1], r0.w[1]);
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[0]) : [src1] "r" (r2.w[0]) , [src2] "r" (r0.w[0]) : "cc" );
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r0.w[1]) : "cc" );
#endif
                    unaligned_write_64(pval2 + ofs1, r2.d);
                    r1.w[1] = 0;
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r0.d = unaligned_read_64(pval2 + ofs1);
                    unaligned_write_64(pval2 + ofs2 + 3, r1.d);
                    *pval3 = ofs1;
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 2);
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[1] = __qadd16(r2.w[1], r0.w[0]);
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r0.w[0]) : "cc" );
#endif
                    r1.w[0] = r0.w[1];
                    r1.w[1] = 0;
                    unaligned_write_64(pval2 + ofs1, r2.d);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r0.d = unaligned_read_64(pval2 + ofs1);
                    unaligned_write_64(pval2 + ofs2 + 2, r1.d);
                    *pval3 = ofs1;
                }
                else
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 1);
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[1] = __qadd16(r2.w[1], r0.w[0] << 16);
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r0.w[0] << 16) : "cc" );
#endif
                    r1.d = r0.d >> 16;
                    unaligned_write_64(pval2 + ofs1, r2.d);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r0.d = unaligned_read_64(pval2 + ofs1);
                    unaligned_write_64(pval2 + ofs2 + 1, r1.d);
                    *pval3 = ofs1;
                }
            }

            counter2++;
        } while (counter2 < 1);

        tmp1 = *params2;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        tmp2 = __qsub16(r0.h[0], tmp1);
#else
        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.h[0]) , [src2] "r" (tmp1) : "cc" );
#endif
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        tmp2 = __builtin_arm_smulwb(p1_1, tmp2);
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (p1_1) , [src2] "r" (tmp2) );
#endif
        tmp1 += tmp2 << 1;
        r0.h[0] = tmp1;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        tmp2 = __qsub16(r0.h[1], tmp1);
#else
        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.h[1]) , [src2] "r" (tmp1) : "cc" );
#endif
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        tmp2 = __builtin_arm_smulwb(p1_1, tmp2);
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (p1_1) , [src2] "r" (tmp2) );
#endif
        tmp1 += tmp2 << 1;
        r0.h[1] = tmp1;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        tmp2 = __qsub16(r0.h[2], tmp1);
#else
        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.h[2]) , [src2] "r" (tmp1) : "cc" );
#endif
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        tmp2 = __builtin_arm_smulwb(p1_1, tmp2);
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (p1_1) , [src2] "r" (tmp2) );
#endif
        tmp1 += tmp2 << 1;
        r0.h[2] = tmp1;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        tmp2 = __qsub16(r0.h[3], tmp1);
#else
        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.h[3]) , [src2] "r" (tmp1) : "cc" );
#endif
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        tmp2 = __builtin_arm_smulwb(p1_1, tmp2);
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (p1_1) , [src2] "r" (tmp2) );
#endif
        tmp1 += tmp2 << 1;
        r0.h[3] = tmp1;
        *params2 = tmp1;

        do
        {
            pval2 = ((PTR32(uint16_t) *)data2)[counter2];
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
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 2; index++)
                    {
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r3.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r3.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r2.w[index] = ((uint32_t)__qsub16(r0.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[index]) : [src1] "r" (r0.w[index]) , [src2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r2.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r2.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r2.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r2.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r0.w[index] = ((uint32_t)__qadd16(r3.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[index]) : [src1] "r" (r3.w[index]) , [src2] "r" (tmp1) );
#endif
                    }

                    if (ofs1 != 0)
                    {
                        unaligned_write_64(pval2 + ofs1 - 4, r2.d);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        unaligned_write_64(pval2 + ofs2, r2.d);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 3);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 2; index++)
                    {
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r3.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r3.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r1.w[index] = ((uint32_t)__qsub16(r0.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (r1.w[index]) : [src1] "r" (r0.w[index]) , [src2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r1.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r0.w[index] = ((uint32_t)__qadd16(r3.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[index]) : [src1] "r" (r3.w[index]) , [src2] "r" (tmp1) );
#endif
                    }

                    r3.d = r1.d << 16;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[0] = ((uint32_t)__qadd16(r2.w[0], r3.w[0]));
                    r2.w[1] = ((uint32_t)__qadd16(r2.w[1], r3.w[1]));
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[0]) : [src1] "r" (r2.w[0]) , [src2] "r" (r3.w[0]) );
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r3.w[1]) );
#endif
                    r1.w[0] = r1.w[1] >> 16;
                    r1.w[1] = 0;
                    if (ofs1 != 0)
                    {
                        unaligned_write_64(pval2 + ofs1 - 4, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 3, r1.d);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        unaligned_write_64(pval2 + ofs2 - 1, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 3, r1.d);
                        *pval3 = ofs1;
                    }
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 2; index++)
                    {
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r3.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r3.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r1.w[index] = ((uint32_t)__qsub16(r0.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (r1.w[index]) : [src1] "r" (r0.w[index]) , [src2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r1.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r0.w[index] = ((uint32_t)__qadd16(r3.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[index]) : [src1] "r" (r3.w[index]) , [src2] "r" (tmp1) );
#endif
                    }

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[1] = ((uint32_t)__qadd16(r2.w[1], r1.w[0]));
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r1.w[0]) );
#endif
                    r1.w[0] = r1.w[1];
                    r1.w[1] = 0;
                    if (ofs1 != 0)
                    {
                        unaligned_write_64(pval2 + ofs1 - 4, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 2, r1.d);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        unaligned_write_64(pval2 + ofs2 - 2, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 2, r1.d);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 1);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 2; index++)
                    {
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r3.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r3.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r1.w[index] = ((uint32_t)__qsub16(r0.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (r1.w[index]) : [src1] "r" (r0.w[index]) , [src2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r1.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r0.w[index] = ((uint32_t)__qadd16(r3.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[index]) : [src1] "r" (r3.w[index]) , [src2] "r" (tmp1) );
#endif
                    }

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[1] = ((uint32_t)__qadd16(r2.w[1], r1.w[0] << 16));
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r1.w[0] << 16) );
#endif
                    r1.d = r1.d >> 16;
                    if (ofs1 != 0)
                    {
                        unaligned_write_64(pval2 + ofs1 - 4, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 1, r1.d);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        unaligned_write_64(pval2 + ofs2 - 3, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 1, r1.d);
                        *pval3 = ofs1;
                    }
                }
            }

            counter2++;
        } while (counter2 < 5);

        r3.d = *(uint64_t *)dst;
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r0.w[0] = ((__builtin_arm_smulbb(r0.w[0], p1_2) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r0.w[0], p1_2) & UINT32_C(0xffff0000));
        r0.w[1] = ((__builtin_arm_smulbb(r0.w[1], p1_2) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r0.w[1], p1_2) & UINT32_C(0xffff0000));
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r0.w[0]) , [src2] "r" (p1_2) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.w[0]) , [src2] "r" (p1_2) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0.w[0]) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );

        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r0.w[1]) , [src2] "r" (p1_2) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.w[1]) , [src2] "r" (p1_2) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0.w[1]) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r0.w[0] = ((uint32_t)__sadd16(r0.w[0], r0.w[0]));
        r0.w[1] = ((uint32_t)__sadd16(r0.w[1], r0.w[1]));

        r0.w[0] = ((uint32_t)__qadd16(r0.w[0], r3.w[0]));
        r0.w[1] = ((uint32_t)__qadd16(r0.w[1], r3.w[1]));
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0.w[0]) : [src] "r" (r0.w[0]) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0.w[1]) : [src] "r" (r0.w[1]) : "cc" );

        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[0]) : [src1] "r" (r0.w[0]) , [src2] "r" (r3.w[0]) );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[1]) : [src1] "r" (r0.w[1]) , [src2] "r" (r3.w[1]) );
#endif
        *(uint64_t *)dst = r0.d;
        dst += 8;
        counter1 -= 4;
    } while (counter1 > 0);
}

void CCALL simd_1000AD02_c(int32_t counter1, int16_t * RESTRICT params1, uint16_t * RESTRICT params2, uint16_t * RESTRICT data1, uint32_t * RESTRICT data2, uint32_t * RESTRICT data3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    union {
        int16_t h[4];
        uint32_t w[2];
        uint64_t d;
    } r0, r1, r2, r3;
    uint32_t counter2, ofs1, ofs2, index;
    int32_t tmp1, p1_0, p1_1, p1_2;
#if !(defined(__ARM_ACLE) && __ARM_32BIT_STATE)
    int32_t tmp2;
#endif
    uint16_t *pval2, *pval3;

    p1_0 = params1[0];
    p1_1 = params1[1];
    p1_2 = params1[2];

    do
    {
        r0.d = *(uint64_t *)src;
        src += 8;
        counter2 = 0;

        do
        {
            pval2 = ((PTR32(uint16_t) *)data2)[counter2];
            pval3 = ((PTR32(uint16_t) *)data3)[counter2];
            ofs1 = *pval3;
            ofs2 = data1[counter2];

            if ((ofs2 & 2) == 0)
            {
                if ((ofs2 & 1) == 0)
                {
                    unaligned_write_64(pval2 + ofs1, r0.d);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 > ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r0.d = unaligned_read_64(pval2 + ofs1);
                    *pval3 = ofs1;
                }
                else
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 3);
                    r1.w[0] = r0.w[1] >> 16;
                    r0.d = r0.d << 16;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[0] = __qadd16(r2.w[0], r0.w[0]);
                    r2.w[1] = __qadd16(r2.w[1], r0.w[1]);
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[0]) : [src1] "r" (r2.w[0]) , [src2] "r" (r0.w[0]) : "cc" );
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r0.w[1]) : "cc" );
#endif
                    unaligned_write_64(pval2 + ofs1, r2.d);
                    r1.w[1] = 0;
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r0.d = unaligned_read_64(pval2 + ofs1);
                    unaligned_write_64(pval2 + ofs2 + 3, r1.d);
                    *pval3 = ofs1;
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 2);
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[1] = __qadd16(r2.w[1], r0.w[0]);
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r0.w[0]) : "cc" );
#endif
                    r1.w[0] = r0.w[1];
                    r1.w[1] = 0;
                    unaligned_write_64(pval2 + ofs1, r2.d);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r0.d = unaligned_read_64(pval2 + ofs1);
                    unaligned_write_64(pval2 + ofs2 + 2, r1.d);
                    *pval3 = ofs1;
                }
                else
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 1);
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[1] = __qadd16(r2.w[1], r0.w[0] << 16);
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r0.w[0] << 16) : "cc" );
#endif
                    r1.d = r0.d >> 16;
                    unaligned_write_64(pval2 + ofs1, r2.d);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r0.d = unaligned_read_64(pval2 + ofs1);
                    unaligned_write_64(pval2 + ofs2 + 1, r1.d);
                    *pval3 = ofs1;
                }
            }

            counter2++;
        } while (counter2 < 1);

        tmp1 = *params2;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        tmp2 = __qsub16(r0.h[0], tmp1);
#else
        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.h[0]) , [src2] "r" (tmp1) : "cc" );
#endif
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        tmp2 = __builtin_arm_smulwb(p1_1, tmp2);
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (p1_1) , [src2] "r" (tmp2) );
#endif
        tmp1 += tmp2 << 1;
        r0.h[0] = tmp1;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        tmp2 = __qsub16(r0.h[1], tmp1);
#else
        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.h[1]) , [src2] "r" (tmp1) : "cc" );
#endif
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        tmp2 = __builtin_arm_smulwb(p1_1, tmp2);
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (p1_1) , [src2] "r" (tmp2) );
#endif
        tmp1 += tmp2 << 1;
        r0.h[1] = tmp1;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        tmp2 = __qsub16(r0.h[2], tmp1);
#else
        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.h[2]) , [src2] "r" (tmp1) : "cc" );
#endif
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        tmp2 = __builtin_arm_smulwb(p1_1, tmp2);
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (p1_1) , [src2] "r" (tmp2) );
#endif
        tmp1 += tmp2 << 1;
        r0.h[2] = tmp1;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        tmp2 = __qsub16(r0.h[3], tmp1);
#else
        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.h[3]) , [src2] "r" (tmp1) : "cc" );
#endif
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        tmp2 = __builtin_arm_smulwb(p1_1, tmp2);
#else
        asm ( "smulwb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (p1_1) , [src2] "r" (tmp2) );
#endif
        tmp1 += tmp2 << 1;
        r0.h[3] = tmp1;
        *params2 = tmp1;

        do
        {
            pval2 = ((PTR32(uint16_t) *)data2)[counter2];
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
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 2; index++)
                    {
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r3.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r3.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r2.w[index] = ((uint32_t)__qsub16(r0.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[index]) : [src1] "r" (r0.w[index]) , [src2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r2.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r2.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r2.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r2.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r0.w[index] = ((uint32_t)__qadd16(r3.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[index]) : [src1] "r" (r3.w[index]) , [src2] "r" (tmp1) );
#endif
                    }

                    if (ofs1 != 0)
                    {
                        unaligned_write_64(pval2 + ofs1 - 4, r2.d);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        unaligned_write_64(pval2 + ofs2, r2.d);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 3);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 2; index++)
                    {
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r3.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r3.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r1.w[index] = ((uint32_t)__qsub16(r0.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (r1.w[index]) : [src1] "r" (r0.w[index]) , [src2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r1.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r0.w[index] = ((uint32_t)__qadd16(r3.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[index]) : [src1] "r" (r3.w[index]) , [src2] "r" (tmp1) );
#endif
                    }

                    r3.d = r1.d << 16;
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[0] = ((uint32_t)__qadd16(r2.w[0], r3.w[0]));
                    r2.w[1] = ((uint32_t)__qadd16(r2.w[1], r3.w[1]));
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[0]) : [src1] "r" (r2.w[0]) , [src2] "r" (r3.w[0]) );
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r3.w[1]) );
#endif
                    r1.w[0] = r1.w[1] >> 16;
                    r1.w[1] = 0;
                    if (ofs1 != 0)
                    {
                        unaligned_write_64(pval2 + ofs1 - 4, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 3, r1.d);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        unaligned_write_64(pval2 + ofs2 - 1, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 3, r1.d);
                        *pval3 = ofs1;
                    }
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 2; index++)
                    {
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r3.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r3.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r1.w[index] = ((uint32_t)__qsub16(r0.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (r1.w[index]) : [src1] "r" (r0.w[index]) , [src2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r1.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r0.w[index] = ((uint32_t)__qadd16(r3.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[index]) : [src1] "r" (r3.w[index]) , [src2] "r" (tmp1) );
#endif
                    }

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[1] = ((uint32_t)__qadd16(r2.w[1], r1.w[0]));
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r1.w[0]) );
#endif
                    r1.w[0] = r1.w[1];
                    r1.w[1] = 0;
                    if (ofs1 != 0)
                    {
                        unaligned_write_64(pval2 + ofs1 - 4, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 2, r1.d);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        unaligned_write_64(pval2 + ofs2 - 2, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 2, r1.d);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    r2.d = unaligned_read_64(pval2 + ofs2 + 1);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 2; index++)
                    {
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r3.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r3.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r3.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r1.w[index] = ((uint32_t)__qsub16(r0.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qsub16 %[result], %[src1], %[src2]" : [result] "=r" (r1.w[index]) : [src1] "r" (r0.w[index]) , [src2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
                        tmp1 = ((__builtin_arm_smulbb(r1.w[index], p1_0) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[index], p1_0) & UINT32_C(0xffff0000));
#else
                        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r1.w[index]) , [src2] "r" (p1_0) );
                        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (tmp1) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                        tmp1 = ((uint32_t)__sadd16(tmp1, tmp1));
                        r0.w[index] = ((uint32_t)__qadd16(r3.w[index], tmp1));
#else
                        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (tmp1) : [src] "r" (tmp1) : "cc" );
                        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0.w[index]) : [src1] "r" (r3.w[index]) , [src2] "r" (tmp1) );
#endif
                    }

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    r2.w[1] = ((uint32_t)__qadd16(r2.w[1], r1.w[0] << 16));
#else
                    asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2.w[1]) : [src1] "r" (r2.w[1]) , [src2] "r" (r1.w[0] << 16) );
#endif
                    r1.d = r1.d >> 16;
                    if (ofs1 != 0)
                    {
                        unaligned_write_64(pval2 + ofs1 - 4, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 1, r1.d);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        unaligned_write_64(pval2 + ofs2 - 3, r2.d);
                        unaligned_write_64(pval2 + ofs2 + 1, r1.d);
                        *pval3 = ofs1;
                    }
                }
            }

            counter2++;
        } while (counter2 < 5);

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r0.w[0] = ((__builtin_arm_smulbb(r0.w[0], p1_2) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r0.w[0], p1_2) & UINT32_C(0xffff0000));
        r0.w[1] = ((__builtin_arm_smulbb(r0.w[1], p1_2) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r0.w[1], p1_2) & UINT32_C(0xffff0000));
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r0.w[0]) , [src2] "r" (p1_2) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.w[0]) , [src2] "r" (p1_2) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0.w[0]) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );

        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (tmp1) : [src1] "r" (r0.w[1]) , [src2] "r" (p1_2) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (tmp2) : [src1] "r" (r0.w[1]) , [src2] "r" (p1_2) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0.w[1]) : [high1] "r" (tmp2) , [high2] "r" (tmp1) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r0.w[0] = ((uint32_t)__sadd16(r0.w[0], r0.w[0]));
        r0.w[1] = ((uint32_t)__sadd16(r0.w[1], r0.w[1]));
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0.w[0]) : [src] "r" (r0.w[0]) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0.w[1]) : [src] "r" (r0.w[1]) : "cc" );
#endif
        *(uint64_t *)dst = r0.d;
        dst += 8;
        counter1 -= 4;
    } while (counter1 > 0);
}

// chorus

void CCALL simd_1000B2EE_c(uint8_t * RESTRICT params1, uint8_t * RESTRICT params2, uint16_t * RESTRICT data, uint8_t * RESTRICT params3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    union {
        int16_t h[4];
        uint32_t w[2];
    } r1;
    int32_t counter, index, tmp1, r0, r2, r7;
    uint32_t length, offset, ofs2, p2_24, p2_68;
    uint16_t * RESTRICT poffs, * RESTRICT pval;
#if !(defined(__ARM_ACLE) && __ARM_32BIT_STATE)
    uint32_t r8, r9;
#endif

    r7 = (*(int16_t *)params2) >> (*(uint16_t *)(params1 + 8));
    p2_24 = *(uint16_t *)(params2 + 2) + (*(uint16_t *)(params2 + 4) << 16);
    p2_68 = *(uint16_t *)(params2 + 6) + (*(uint16_t *)(params2 + 8) << 16);
    length = *(uint32_t *)(params1 + 4);
    counter = *(int32_t *)params1;
    poffs = *(PTR32(uint16_t) *)params3;
    pval = *(PTR32(uint16_t) *)(params3 + 4);
    offset = *poffs;

    do
    {
        r1.h[0] = *pval;
        r1.h[1] = r1.h[0] + r7;
        r1.h[2] = r1.h[0] + 2 * r7;
        r1.h[3] = r1.h[0] + 3 * r7;
        *pval = r1.h[0] + 4 * r7;

        for (index = 3; index >= 0; index--)
        {
            if (r1.h[index] < 0) r1.h[index] = -r1.h[index];
            r0 = ((r1.h[index] >> 1) - 0x3fff) << 1;
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
            r0 = __builtin_arm_smulbb(r0, p2_24);
#else
            asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (p2_24) );
#endif
            r0 = (r0 >> 16) + 0x3fff;
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
            r0 = __builtin_arm_smulbt(r0, p2_24);
#else
            asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (p2_24) );
#endif
            r2 = r0 & 0x7fff;
            r1.h[index] = r0 >> 15;
            if (r1.h[index] < 4) r1.h[index] = 4;

            ofs2 = offset + index - r1.h[index];
            if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
            {
                ofs2 += length;
            }

            tmp1 = data[ofs2];

            ofs2--;
            if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
            {
                ofs2 += length;
            }

            r0 = data[ofs2] - tmp1;
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
            r0 = __builtin_arm_smulbb(r0, r2);
#else
            asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (r2) );
#endif
            r1.h[index] = ((r0 >> 16) << 1) + tmp1;
        }

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r2 = ((__builtin_arm_smulbb(r1.w[0], p2_68) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[0], p2_68) & UINT32_C(0xffff0000));
        r0 = ((__builtin_arm_smulbt(r1.w[0], p2_68) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultt(r1.w[0], p2_68) & UINT32_C(0xffff0000));
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r1.w[0]) , [src2] "r" (p2_68) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r1.w[0]) , [src2] "r" (p2_68) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r2) : [high1] "r" (r9) , [high2] "r" (r8) );

        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r1.w[0]) , [src2] "r" (p2_68) );
        asm ( "smultt %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r1.w[0]) , [src2] "r" (p2_68) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0) : [high1] "r" (r9) , [high2] "r" (r8) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r2 = ((uint32_t)__sadd16(r2, r2));
        r0 = ((uint32_t)__sadd16(r0, r0));
        r2 = ((uint32_t)__qadd16(r2, *(uint32_t *)src));
        r0 = ((uint32_t)__qadd16(r0, *(uint32_t *)dst));
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r2) : [src] "r" (r2) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0) : [src] "r" (r0) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2) : [src1] "r" (r2), [src2] "r" (*(uint32_t *)src) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0), [src2] "r" (*(uint32_t *)dst) : "cc" );
#endif
        src += 4;
        *(uint32_t *)(data + offset) = r2;
        offset += 2;
        *(uint32_t *)(dst) = r0;
        dst += 4;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r2 = ((__builtin_arm_smulbb(r1.w[1], p2_68) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[1], p2_68) & UINT32_C(0xffff0000));
        r0 = ((__builtin_arm_smulbt(r1.w[1], p2_68) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultt(r1.w[1], p2_68) & UINT32_C(0xffff0000));
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r1.w[1]) , [src2] "r" (p2_68) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r1.w[1]) , [src2] "r" (p2_68) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r2) : [high1] "r" (r9) , [high2] "r" (r8) );

        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r1.w[1]) , [src2] "r" (p2_68) );
        asm ( "smultt %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r1.w[1]) , [src2] "r" (p2_68) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0) : [high1] "r" (r9) , [high2] "r" (r8) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r2 = ((uint32_t)__sadd16(r2, r2));
        r0 = ((uint32_t)__sadd16(r0, r0));
        r2 = ((uint32_t)__qadd16(r2, *(uint32_t *)src));
        r0 = ((uint32_t)__qadd16(r0, *(uint32_t *)dst));
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r2) : [src] "r" (r2) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0) : [src] "r" (r0) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2) : [src1] "r" (r2), [src2] "r" (*(uint32_t *)src) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0), [src2] "r" (*(uint32_t *)dst) : "cc" );
#endif
        src += 4;
        *(uint32_t *)(data + offset) = r2;
        offset += 2;
        *(uint32_t *)(dst) = r0;
        dst += 4;

        if (UNLIKELY_COND(offset >= length)) UNLIKELY_ATTR
        {
            offset = 0;
        }

        counter -= 4;
    } while (counter > 0);

    *poffs = offset;
}

void CCALL simd_1000B7C9_c(uint8_t * RESTRICT params1, uint8_t * RESTRICT params2, uint16_t * RESTRICT data, uint8_t * RESTRICT params3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    union {
        int16_t h[4];
        uint32_t w[2];
    } r1;
    int32_t counter, index, tmp1, r0, r2, r7;
    uint32_t length, offset, ofs2, p2_24, p2_68;
    uint16_t * RESTRICT poffs, * RESTRICT pval;
#if !(defined(__ARM_ACLE) && __ARM_32BIT_STATE)
    uint32_t r8, r9;
#endif

    r7 = (*(int16_t *)params2) >> (*(uint16_t *)(params1 + 8));
    p2_24 = *(uint16_t *)(params2 + 2) + (*(uint16_t *)(params2 + 4) << 16);
    p2_68 = *(uint16_t *)(params2 + 6) + (*(uint16_t *)(params2 + 8) << 16);
    length = *(uint32_t *)(params1 + 4);
    counter = *(int32_t *)params1;
    poffs = *(PTR32(uint16_t) *)params3;
    pval = *(PTR32(uint16_t) *)(params3 + 4);
    offset = *poffs;

    do
    {
        r1.h[0] = *pval;
        r1.h[1] = r1.h[0] + r7;
        r1.h[2] = r1.h[0] + 2 * r7;
        r1.h[3] = r1.h[0] + 3 * r7;
        *pval = r1.h[0] + 4 * r7;

        for (index = 3; index >= 0; index--)
        {
            if (r1.h[index] < 0) r1.h[index] = -r1.h[index];
            r0 = ((r1.h[index] >> 1) - 0x3fff) << 1;
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
            r0 = __builtin_arm_smulbb(r0, p2_24);
#else
            asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (p2_24) );
#endif
            r0 = (r0 >> 16) + 0x3fff;
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
            r0 = __builtin_arm_smulbt(r0, p2_24);
#else
            asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (p2_24) );
#endif
            r2 = r0 & 0x7fff;
            r1.h[index] = r0 >> 15;
            if (r1.h[index] < 4) r1.h[index] = 4;

            ofs2 = offset + index - r1.h[index];
            if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
            {
                ofs2 += length;
            }

            tmp1 = data[ofs2];

            ofs2--;
            if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
            {
                ofs2 += length;
            }

            r0 = data[ofs2] - tmp1;
#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
            r0 = __builtin_arm_smulbb(r0, r2);
#else
            asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r0) : [src1] "r" (r0) , [src2] "r" (r2) );
#endif
            r1.h[index] = ((r0 >> 16) << 1) + tmp1;
        }

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r2 = ((__builtin_arm_smulbb(r1.w[0], p2_68) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[0], p2_68) & UINT32_C(0xffff0000));
        r0 = ((__builtin_arm_smulbt(r1.w[0], p2_68) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultt(r1.w[0], p2_68) & UINT32_C(0xffff0000));
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r1.w[0]) , [src2] "r" (p2_68) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r1.w[0]) , [src2] "r" (p2_68) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r2) : [high1] "r" (r9) , [high2] "r" (r8) );

        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r1.w[0]) , [src2] "r" (p2_68) );
        asm ( "smultt %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r1.w[0]) , [src2] "r" (p2_68) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0) : [high1] "r" (r9) , [high2] "r" (r8) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r2 = ((uint32_t)__sadd16(r2, r2));
        r0 = ((uint32_t)__sadd16(r0, r0));
        r2 = ((uint32_t)__qadd16(r2, *(uint32_t *)src));
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r2) : [src] "r" (r2) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0) : [src] "r" (r0) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2) : [src1] "r" (r2), [src2] "r" (*(uint32_t *)src) : "cc" );
#endif
        src += 4;
        *(uint32_t *)(data + offset) = r2;
        offset += 2;
        *(uint32_t *)(dst) = r0;
        dst += 4;

#if defined(__ARM_ACLE) && __ARM_32BIT_STATE
        r2 = ((__builtin_arm_smulbb(r1.w[1], p2_68) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultb(r1.w[1], p2_68) & UINT32_C(0xffff0000));
        r0 = ((__builtin_arm_smulbt(r1.w[1], p2_68) & UINT32_C(0xffff0000)) >> 16) | (__builtin_arm_smultt(r1.w[1], p2_68) & UINT32_C(0xffff0000));
#else
        asm ( "smulbb %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r1.w[1]) , [src2] "r" (p2_68) );
        asm ( "smultb %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r1.w[1]) , [src2] "r" (p2_68) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r2) : [high1] "r" (r9) , [high2] "r" (r8) );

        asm ( "smulbt %[result], %[src1], %[src2]" : [result] "=r" (r8) : [src1] "r" (r1.w[1]) , [src2] "r" (p2_68) );
        asm ( "smultt %[result], %[src1], %[src2]" : [result] "=r" (r9) : [src1] "r" (r1.w[1]) , [src2] "r" (p2_68) );
        asm ( "pkhtb %[result], %[high1], %[high2], asr #16" : [result] "=r" (r0) : [high1] "r" (r9) , [high2] "r" (r8) );
#endif

#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
        r2 = ((uint32_t)__sadd16(r2, r2));
        r0 = ((uint32_t)__sadd16(r0, r0));
        r2 = ((uint32_t)__qadd16(r2, *(uint32_t *)src));
#else
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r2) : [src] "r" (r2) : "cc" );
        asm ( "sadd16 %[result], %[src], %[src]" : [result] "=r" (r0) : [src] "r" (r0) : "cc" );
        asm ( "qadd16 %[result], %[src1], %[src2]" : [result] "=r" (r2) : [src1] "r" (r2), [src2] "r" (*(uint32_t *)src) : "cc" );
#endif
        src += 4;
        *(uint32_t *)(data + offset) = r2;
        offset += 2;
        *(uint32_t *)(dst) = r0;
        dst += 4;

        if (UNLIKELY_COND(offset >= length)) UNLIKELY_ATTR
        {
            offset = 0;
        }

        counter -= 4;
    } while (counter > 0);

    *poffs = offset;
}