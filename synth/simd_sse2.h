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

#include <immintrin.h>


void CCALL simd_10007028_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint32_t mult4, uint32_t mult5, uint8_t * RESTRICT tmp, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    __m128i tmp0, tmp1;

    xmm0 = _mm_cvtsi32_si128(((uint16_t)mult2) | (mult3 << 16));    // xmm0 = 0000:0000:0000:0000:0000:0000:mul3:mul2
    xmm7 = _mm_cvtsi32_si128((uint16_t)mult1);                      // xmm7 = 0000:0000:0000:0000:0000:0000:0000:mul1
    xmm2 = _mm_cvtsi32_si128(((uint16_t)mult4) | (mult5 << 16));    // xmm2 = 0000:0000:0000:0000:0000:0000:mul5:mul4
    xmm0 = _mm_unpacklo_epi16(xmm0, xmm0);                          // xmm0 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                          // xmm7 = 0000:0000:0000:0000:0000:0000:mul1:mul1
    xmm1 = xmm0;                                                    // xmm1 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));        // xmm7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    xmm0 = _mm_shuffle_epi32(xmm0, _MM_SHUFFLE(0, 0, 0, 0));        // xmm0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm1 = _mm_shuffle_epi32(xmm1, _MM_SHUFFLE(1, 1, 1, 1));        // xmm1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    tmp0 = xmm0;                                                    // tmp0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm2 = _mm_unpacklo_epi16(xmm2, xmm2);                          // xmm2 = 0000:0000:0000:0000:mul5:mul5:mul4:mul4
    xmm3 = xmm2;                                                    // xmm3 = 0000:0000:0000:0000:mul5:mul5:mul4:mul4
    tmp1 = xmm1;                                                    // tmp1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    xmm2 = _mm_shuffle_epi32(xmm2, _MM_SHUFFLE(0, 0, 0, 0));        // xmm2 = mul4:mul4:mul4:mul4:mul4:mul4:mul4:mul4
    xmm3 = _mm_shuffle_epi32(xmm3, _MM_SHUFFLE(1, 1, 1, 1));        // xmm3 = mul5:mul5:mul5:mul5:mul5:mul5:mul5:mul5
    xmm5 = xmm2;

    do
    {
        xmm6 = _mm_load_si128((__m128i *)src);
        src += 16;
        xmm6 = _mm_mulhi_epi16(xmm6, xmm7);                         // xmm6 = [src] * mult1
        xmm1 = tmp1;                                                // tmp1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
        xmm0 = _mm_mulhi_epi16(xmm0, xmm6);                         // xmm0 = [src] * mult1 * mult2
        xmm1 = _mm_mulhi_epi16(xmm1, xmm6);                         // xmm1 = [src] * mult1 * mult3
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);                         // xmm2 = [src] * mult1 * mult4
        xmm6 = _mm_mulhi_epi16(xmm6, xmm3);                         // xmm6 = [src] * mult1 * mult5
        xmm0 = _mm_slli_epi16(xmm0, 1);                             // xmm0 = ([src] * mult1 * mult2) << 1
        xmm4 = xmm0;                                                // xmm4 = ([src] * mult1 * mult2) << 1
        xmm1 = _mm_slli_epi16(xmm1, 1);                             // xmm1 = ([src] * mult1 * mult3) << 1
        xmm2 = _mm_slli_epi16(xmm2, 1);                             // xmm2 = ([src] * mult1 * mult4) << 1
        xmm6 = _mm_slli_epi16(xmm6, 1);                             // xmm6 = ([src] * mult1 * mult5) << 1
        xmm2 = _mm_adds_epi16(xmm2, *(__m128i *)dst2);
        _mm_store_si128((__m128i *)dst2, xmm2);
        xmm0 = _mm_unpacklo_epi16(xmm0, xmm1);                      // xmm0 = m133:m123:m132:m122:m131:m121:m130:m120
        xmm6 = _mm_adds_epi16(xmm6, *(__m128i *)dst3);
        _mm_store_si128((__m128i *)dst3, xmm6);
        xmm4 = _mm_unpackhi_epi16(xmm4, xmm1);                      // xmm4 = m137:m127:m136:m126:m135:m125:m134:m124
        xmm2 = xmm5;
        xmm0 = _mm_adds_epi16(xmm0, *(__m128i *)dst1);
        _mm_store_si128((__m128i *)dst1, xmm0);
        xmm4 = _mm_adds_epi16(xmm4, *(__m128i *)(dst1 + 16));
        _mm_store_si128((__m128i *)(dst1 + 16), xmm4);
        dst1 += 32;
        dst2 += 16;
        dst3 += 16;
        xmm0 = tmp0;                                                // xmm0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007104_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint32_t mult4, uint32_t mult5, uint8_t * RESTRICT tmp, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    __m128i tmp0, tmp1;

    xmm0 = _mm_cvtsi32_si128(((uint16_t)mult2) | (mult3 << 16));    // xmm0 = 0000:0000:0000:0000:0000:0000:mul3:mul2
    xmm7 = _mm_cvtsi32_si128((uint16_t)mult1);                      // xmm7 = 0000:0000:0000:0000:0000:0000:0000:mul1
    xmm2 = _mm_cvtsi32_si128(((uint16_t)mult4) | (mult5 << 16));    // xmm2 = 0000:0000:0000:0000:0000:0000:mul5:mul4
    xmm0 = _mm_unpacklo_epi16(xmm0, xmm0);                          // xmm0 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                          // xmm7 = 0000:0000:0000:0000:0000:0000:mul1:mul1
    xmm1 = xmm0;                                                    // xmm1 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));        // xmm7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    xmm0 = _mm_shuffle_epi32(xmm0, _MM_SHUFFLE(0, 0, 0, 0));        // xmm0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm1 = _mm_shuffle_epi32(xmm1, _MM_SHUFFLE(1, 1, 1, 1));        // xmm1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    tmp0 = xmm0;                                                    // tmp0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm2 = _mm_unpacklo_epi16(xmm2, xmm2);                          // xmm2 = 0000:0000:0000:0000:mul5:mul5:mul4:mul4
    xmm3 = xmm2;                                                    // xmm3 = 0000:0000:0000:0000:mul5:mul5:mul4:mul4
    tmp1 = xmm1;                                                    // tmp1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    xmm2 = _mm_shuffle_epi32(xmm2, _MM_SHUFFLE(0, 0, 0, 0));        // xmm2 = mul4:mul4:mul4:mul4:mul4:mul4:mul4:mul4
    xmm3 = _mm_shuffle_epi32(xmm3, _MM_SHUFFLE(1, 1, 1, 1));        // xmm3 = mul5:mul5:mul5:mul5:mul5:mul5:mul5:mul5
    xmm5 = xmm2;

    do
    {
        xmm6 = _mm_load_si128((__m128i *)src);
        src += 16;
        xmm6 = _mm_mulhi_epi16(xmm6, xmm7);                         // xmm6 = [src] * mult1
        xmm1 = tmp1;                                                // tmp1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
        xmm0 = _mm_mulhi_epi16(xmm0, xmm6);                         // xmm0 = [src] * mult1 * mult2
        xmm1 = _mm_mulhi_epi16(xmm1, xmm6);                         // xmm1 = [src] * mult1 * mult3
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);                         // xmm2 = [src] * mult1 * mult4
        xmm6 = _mm_mulhi_epi16(xmm6, xmm3);                         // xmm6 = [src] * mult1 * mult5
        xmm0 = _mm_slli_epi16(xmm0, 1);                             // xmm0 = ([src] * mult1 * mult2) << 1
        xmm4 = xmm0;                                                // xmm4 = ([src] * mult1 * mult2) << 1
        xmm1 = _mm_slli_epi16(xmm1, 1);                             // xmm1 = ([src] * mult1 * mult3) << 1
        xmm2 = _mm_slli_epi16(xmm2, 1);                             // xmm2 = ([src] * mult1 * mult4) << 1
        xmm6 = _mm_slli_epi16(xmm6, 1);                             // xmm6 = ([src] * mult1 * mult5) << 1
        _mm_store_si128((__m128i *)dst2, xmm2);
        xmm0 = _mm_unpacklo_epi16(xmm0, xmm1);                      // xmm0 = m133:m123:m132:m122:m131:m121:m130:m120
        _mm_store_si128((__m128i *)dst3, xmm6);
        xmm4 = _mm_unpackhi_epi16(xmm4, xmm1);                      // xmm4 = m137:m127:m136:m126:m135:m125:m134:m124
        xmm2 = xmm5;
        _mm_store_si128((__m128i *)dst1, xmm0);
        _mm_store_si128((__m128i *)(dst1 + 16), xmm4);
        dst1 += 32;
        dst2 += 16;
        dst3 += 16;
        xmm0 = tmp0;                                                // xmm0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_100071D0_c(int32_t counter, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    __m128i xmm0;

    xmm0 = _mm_setzero_si128();

    do
    {
        _mm_store_si128((__m128i *)dst1, xmm0);
        _mm_store_si128((__m128i *)dst2, xmm0);
        dst1 += 16;
        dst2 += 16;
        _mm_store_si128((__m128i *)dst1, xmm0);
        _mm_store_si128((__m128i *)dst3, xmm0);
        dst1 += 16;
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_1000720B_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    __m128i xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;

    xmm7 = _mm_cvtsi32_si128((uint16_t)mult1);                      // xmm7 = 0000:0000:0000:0000:0000:0000:0000:mul1
    xmm2 = _mm_cvtsi32_si128(((uint16_t)mult2) | (mult3 << 16));    // xmm2 = 0000:0000:0000:0000:0000:0000:mul3:mul2
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                          // xmm7 = 0000:0000:0000:0000:0000:0000:mul1:mul1
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));        // xmm7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    xmm2 = _mm_unpacklo_epi16(xmm2, xmm2);                          // xmm2 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm3 = xmm2;                                                    // xmm3 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm2 = _mm_shuffle_epi32(xmm2, _MM_SHUFFLE(0, 0, 0, 0));        // xmm2 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm3 = _mm_shuffle_epi32(xmm3, _MM_SHUFFLE(1, 1, 1, 1));        // xmm3 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    xmm5 = xmm2;                                                    // xmm5 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm4 = xmm3;                                                    // xmm4 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3

    do
    {
        xmm6 = _mm_load_si128((__m128i *)src);
        src += 16;
        xmm6 = _mm_mulhi_epi16(xmm6, xmm7);                         // xmm6 = [src] * mult1
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);                         // xmm2 = [src] * mult1 * mult2
        xmm3 = _mm_mulhi_epi16(xmm3, xmm6);                         // xmm3 = [src] * mult1 * mult3
        xmm2 = _mm_slli_epi16(xmm2, 1);                             // xmm2 = ([src] * mult1 * mult2) << 1
        xmm2 = _mm_adds_epi16(xmm2, *(__m128i *)dst2);
        _mm_store_si128((__m128i *)dst2, xmm2);
        xmm3 = _mm_slli_epi16(xmm3, 1);                             // xmm3 = ([src] * mult1 * mult3) << 1
        xmm3 = _mm_adds_epi16(xmm3, *(__m128i *)dst3);
        _mm_store_si128((__m128i *)dst3, xmm3);
        xmm6 = _mm_adds_epi16(xmm6, *(__m128i *)dst1);
        xmm2 = xmm5;
        _mm_store_si128((__m128i *)dst1, xmm6);
        xmm3 = xmm4;
        dst1 += 16;
        dst2 += 16;
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_100072A3_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    __m128i xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;

    xmm7 = _mm_cvtsi32_si128((uint16_t)mult1);                      // xmm7 = 0000:0000:0000:0000:0000:0000:0000:mul1
    xmm2 = _mm_cvtsi32_si128(((uint16_t)mult2) | (mult3 << 16));    // xmm2 = 0000:0000:0000:0000:0000:0000:mul3:mul2
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                          // xmm7 = 0000:0000:0000:0000:0000:0000:mul1:mul1
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));        // xmm7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    xmm2 = _mm_unpacklo_epi16(xmm2, xmm2);                          // xmm2 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm3 = xmm2;                                                    // xmm3 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm2 = _mm_shuffle_epi32(xmm2, _MM_SHUFFLE(0, 0, 0, 0));        // xmm2 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm3 = _mm_shuffle_epi32(xmm3, _MM_SHUFFLE(1, 1, 1, 1));        // xmm3 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    xmm5 = xmm2;                                                    // xmm5 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm4 = xmm3;                                                    // xmm4 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3

    do
    {
        xmm6 = _mm_load_si128((__m128i *)src);
        src += 16;
        xmm6 = _mm_mulhi_epi16(xmm6, xmm7);                         // xmm6 = [src] * mult1
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);                         // xmm2 = [src] * mult1 * mult2
        xmm3 = _mm_mulhi_epi16(xmm3, xmm6);                         // xmm3 = [src] * mult1 * mult3
        xmm2 = _mm_slli_epi16(xmm2, 1);                             // xmm2 = ([src] * mult1 * mult2) << 1
        _mm_store_si128((__m128i *)dst2, xmm2);
        xmm3 = _mm_slli_epi16(xmm3, 1);                             // xmm3 = ([src] * mult1 * mult3) << 1
        _mm_store_si128((__m128i *)dst3, xmm3);
        xmm2 = xmm5;
        _mm_store_si128((__m128i *)dst1, xmm6);
        xmm3 = xmm4;
        dst1 += 16;
        dst2 += 16;
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_1000732F_c(int32_t counter, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    __m128i xmm0;

    xmm0 = _mm_setzero_si128();

    do
    {
        _mm_store_si128((__m128i *)dst1, xmm0);
        _mm_store_si128((__m128i *)dst2, xmm0);
        dst1 += 16;
        dst2 += 16;
        _mm_store_si128((__m128i *)dst3, xmm0);
        dst3 += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007364_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst)
{
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm6, xmm7;

    xmm0 = _mm_cvtsi32_si128(((uint16_t)mult2) | (mult3 << 16));    // xmm0 = 0000:0000:0000:0000:0000:0000:mul3:mul2
    xmm7 = _mm_cvtsi32_si128((uint16_t)mult1);                      // xmm7 = 0000:0000:0000:0000:0000:0000:0000:mul1
    xmm0 = _mm_unpacklo_epi16(xmm0, xmm0);                          // xmm0 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                          // xmm7 = 0000:0000:0000:0000:0000:0000:mul1:mul1
    xmm1 = xmm0;                                                    // xmm1 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));        // xmm7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    xmm0 = _mm_shuffle_epi32(xmm0, _MM_SHUFFLE(0, 0, 0, 0));        // xmm0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm1 = _mm_shuffle_epi32(xmm1, _MM_SHUFFLE(1, 1, 1, 1));        // xmm1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    xmm2 = xmm0;                                                    // xmm2 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm3 = xmm1;                                                    // xmm3 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3

    do
    {
        xmm6 = _mm_load_si128((__m128i *)src);
        src += 16;
        xmm6 = _mm_mulhi_epi16(xmm6, xmm7);                         // xmm6 = [src] * mult1
        xmm1 = xmm3;                                                // xmm1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
        xmm0 = _mm_mulhi_epi16(xmm0, xmm6);                         // xmm0 = [src] * mult1 * mult2
        xmm1 = _mm_mulhi_epi16(xmm1, xmm6);                         // xmm1 = [src] * mult1 * mult3
        xmm0 = _mm_slli_epi16(xmm0, 1);                             // xmm0 = ([src] * mult1 * mult2) << 1
        xmm4 = xmm0;                                                // xmm4 = ([src] * mult1 * mult2) << 1
        xmm1 = _mm_slli_epi16(xmm1, 1);                             // xmm1 = ([src] * mult1 * mult3) << 1
        xmm0 = _mm_unpacklo_epi16(xmm0, xmm1);                      // xmm0 = m133:m123:m132:m122:m131:m121:m130:m120
        xmm4 = _mm_unpackhi_epi16(xmm4, xmm1);                      // xmm4 = m137:m127:m136:m126:m135:m125:m134:m124
        xmm0 = _mm_adds_epi16(xmm0, *(__m128i *)dst);
        xmm4 = _mm_adds_epi16(xmm4, *(__m128i *)(dst + 16));
        _mm_store_si128((__m128i *)dst, xmm0);
        _mm_store_si128((__m128i *)(dst + 16), xmm4);
        dst += 32;
        xmm0 = xmm2;                                                // xmm0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_100073F1_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst)
{
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm6, xmm7;

    xmm0 = _mm_cvtsi32_si128(((uint16_t)mult2) | (mult3 << 16));    // xmm0 = 0000:0000:0000:0000:0000:0000:mul3:mul2
    xmm7 = _mm_cvtsi32_si128((uint16_t)mult1);                      // xmm7 = 0000:0000:0000:0000:0000:0000:0000:mul1
    xmm0 = _mm_unpacklo_epi16(xmm0, xmm0);                          // xmm0 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                          // xmm7 = 0000:0000:0000:0000:0000:0000:mul1:mul1
    xmm1 = xmm0;                                                    // xmm1 = 0000:0000:0000:0000:mul3:mul3:mul2:mul2
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));        // xmm7 = mul1:mul1:mul1:mul1:mul1:mul1:mul1:mul1
    xmm0 = _mm_shuffle_epi32(xmm0, _MM_SHUFFLE(0, 0, 0, 0));        // xmm0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm1 = _mm_shuffle_epi32(xmm1, _MM_SHUFFLE(1, 1, 1, 1));        // xmm1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
    xmm2 = xmm0;                                                    // xmm2 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
    xmm3 = xmm1;                                                    // xmm3 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3

    do
    {
        xmm6 = _mm_load_si128((__m128i *)src);
        src += 16;
        xmm6 = _mm_mulhi_epi16(xmm6, xmm7);                         // xmm6 = [src] * mult1
        xmm1 = xmm3;                                                // xmm1 = mul3:mul3:mul3:mul3:mul3:mul3:mul3:mul3
        xmm0 = _mm_mulhi_epi16(xmm0, xmm6);                         // xmm0 = [src] * mult1 * mult2
        xmm1 = _mm_mulhi_epi16(xmm1, xmm6);                         // xmm1 = [src] * mult1 * mult3
        xmm0 = _mm_slli_epi16(xmm0, 1);                             // xmm0 = ([src] * mult1 * mult2) << 1
        xmm4 = xmm0;                                                // xmm4 = ([src] * mult1 * mult2) << 1
        xmm1 = _mm_slli_epi16(xmm1, 1);                             // xmm1 = ([src] * mult1 * mult3) << 1
        xmm0 = _mm_unpacklo_epi16(xmm0, xmm1);                      // xmm0 = m133:m123:m132:m122:m131:m121:m130:m120
        xmm4 = _mm_unpackhi_epi16(xmm4, xmm1);                      // xmm4 = m137:m127:m136:m126:m135:m125:m134:m124
        _mm_store_si128((__m128i *)dst, xmm0);
        _mm_store_si128((__m128i *)(dst + 16), xmm4);
        dst += 32;
        xmm0 = xmm2;                                                // xmm0 = mul2:mul2:mul2:mul2:mul2:mul2:mul2:mul2
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007472_c(int32_t counter, uint8_t * RESTRICT dst)
{
    __m128i xmm0;

    xmm0 = _mm_setzero_si128();

    do
    {
        _mm_store_si128((__m128i *)dst, xmm0);
        dst += 16;
        counter -= 4;
    } while (counter > 0);
}

void CCALL simd_10007491_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult, uint8_t * RESTRICT dst)
{
    __m128i xmm0, xmm7;

    xmm7 = _mm_set1_epi16((uint16_t)mult);    // xmm7 = mult:mult:mult:mult:mult:mult:mult:mult

    do
    {
        xmm0 = _mm_load_si128((__m128i *)src);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm7);
        xmm0 = _mm_adds_epi16(xmm0, *(__m128i *)dst);
        _mm_store_si128((__m128i *)dst, xmm0);
        src += 16;
        dst += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_100074CD_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult, uint8_t * RESTRICT dst)
{
    __m128i xmm0, xmm7;

    xmm7 = _mm_set1_epi16((uint16_t)mult);    // xmm7 = mult:mult:mult:mult:mult:mult:mult:mult

    do
    {
        xmm0 = _mm_load_si128((__m128i *)src);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm7);
        _mm_store_si128((__m128i *)dst, xmm0);
        src += 16;
        dst += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007506_c(int32_t counter, uint8_t * RESTRICT dst)
{
    __m128i xmm0;

    xmm0 = _mm_setzero_si128();

    do
    {
        _mm_store_si128((__m128i *)dst, xmm0);
        dst += 16;
        counter -= 8;
    } while (counter > 0);
}

void CCALL simd_10007525_c(int32_t counter1, int32_t multadd2, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    __m128i xmm0, xmm1, xmm2;
    int32_t counter2;

    multadd2 = (multadd2 << 1) - 1;
    if (multadd2 >= 0)
    {
        do
        {
            xmm0 = _mm_load_si128((__m128i *)src);
            counter2 = multadd2;
            xmm2 = xmm0;
            src += 16;

            do
            {
                xmm0 = _mm_adds_epi16(xmm0, xmm2);
                counter2--;
            } while (counter2 > 0);

            xmm1 = xmm0;
            xmm0 = _mm_unpacklo_epi16(xmm0, xmm0);
            xmm0 = _mm_adds_epi16(xmm0, *(__m128i *)dst);
            _mm_store_si128((__m128i *)dst, xmm0);
            dst += 16;

            xmm1 = _mm_unpackhi_epi16(xmm1, xmm1);
            xmm1 = _mm_adds_epi16(xmm1, *(__m128i *)dst);
            _mm_store_si128((__m128i *)dst, xmm1);
            dst += 16;
            counter1 -= 8;
        } while (counter1 > 0);
    }
    else
    {
        do
        {
            xmm0 = _mm_load_si128((__m128i *)src);
            xmm1 = xmm0;
            xmm0 = _mm_unpacklo_epi16(xmm0, xmm0);
            xmm0 = _mm_adds_epi16(xmm0, *(__m128i *)dst);
            src += 16;
            _mm_store_si128((__m128i *)dst, xmm0);

            xmm1 = _mm_unpackhi_epi16(xmm1, xmm1);
            dst += 16;
            xmm1 = _mm_adds_epi16(xmm1, *(__m128i *)dst);
            _mm_store_si128((__m128i *)dst, xmm1);
            dst += 16;
            counter1 -= 8;
        } while (counter1 > 0);
    }
}

void CCALL simd_1000758D_c(int32_t counter1, int32_t multadd2, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    __m128i xmm0, xmm1;
    int32_t counter2;

    multadd2 = (multadd2 << 1) - 1;
    if (multadd2 >= 0)
    {
        do
        {
            xmm0 = _mm_load_si128((__m128i *)src);
            counter2 = multadd2;
            xmm1 = xmm0;
            src += 16;

            do
            {
                xmm0 = _mm_adds_epi16(xmm0, xmm1);
                counter2--;
            } while (counter2 > 0);

            xmm0 = _mm_adds_epi16(xmm0, *(__m128i *)dst);
            _mm_store_si128((__m128i *)dst, xmm0);
            dst += 16;
            counter1 -= 8;
        } while (counter1 > 0);
    }
    else
    {
        do
        {
            xmm0 = _mm_load_si128((__m128i *)src);
            src += 16;
            xmm0 = _mm_adds_epi16(xmm0, *(__m128i *)dst);
            _mm_store_si128((__m128i *)dst, xmm0);
            dst += 16;
            counter1 -= 8;
        } while (counter1 > 0);
    }
}

void CCALL simd_100075CC_c(int32_t counter, uint32_t shift, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    __m128i xmm0, xmm1, xmm2, xmm7;

    if (cs_volume_multiplier == 8192)
    {
        xmm7 = _mm_cvtsi32_si128((uint16_t)(shift - 1));

        do
        {
            xmm0 = _mm_load_si128((__m128i *)src);
            xmm0 = _mm_sra_epi16(xmm0, xmm7);
            _mm_storeu_si128((__m128i *)dst, xmm0);
            dst += 16;
            src += 16;
            counter -= 8;
        } while (counter > 0);
    }
    else
    {
        xmm7 = _mm_set1_epi16((uint16_t)(cs_volume_multiplier >> (uint16_t)(shift - 1)));

        do
        {
            xmm0 = _mm_load_si128((__m128i *)src);
            xmm1 = xmm0;
            xmm0 = _mm_mullo_epi16(xmm0, xmm7);
            xmm1 = _mm_mulhi_epi16(xmm1, xmm7);
            xmm2 = xmm0;
            xmm0 = _mm_unpacklo_epi16(xmm0, xmm1);
            xmm2 = _mm_unpackhi_epi16(xmm2, xmm1);
            xmm0 = _mm_srai_epi32(xmm0, 11);
            xmm2 = _mm_srai_epi32(xmm2, 11);
            xmm0 = _mm_packs_epi32(xmm0, xmm2);
            _mm_storeu_si128((__m128i *)dst, xmm0);
            dst += 16;
            src += 16;
            counter -= 8;
        } while (counter > 0);
    }
}

// reverb

void CCALL simd_1000A920_c(int32_t counter1, int16_t * RESTRICT params1, uint16_t * RESTRICT params2, uint16_t * RESTRICT data1, uint32_t * RESTRICT data2, uint32_t * RESTRICT data3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    uint32_t counter2, ofs1, ofs2;
    int32_t tmp1;
    int16_t * RESTRICT pval2;
    uint16_t * RESTRICT pval3;

    xmm7 = _mm_loadu_si64(params1);                             // xmm7 = 0000:0000:0000:0000:xxxx:p1_2:p1_1:p1_0
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                      // xmm7 = xxxx:xxxx:p1_2:p1_2:p1_1:p1_1:p1_0:p1_0
    xmm6 = xmm7;
    xmm6 = _mm_shuffle_epi32(xmm6, _MM_SHUFFLE(1, 1, 1, 1));    // xmm6 = p1_1:p1_1:p1_1:p1_1:p1_1:p1_1:p1_1:p1_1
    xmm5 = xmm7;
    xmm5 = _mm_shuffle_epi32(xmm5, _MM_SHUFFLE(2, 2, 2, 2));    // xmm5 = p1_2:p1_2:p1_2:p1_2:p1_2:p1_2:p1_2:p1_2
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));    // xmm7 = p1_0:p1_0:p1_0:p1_0:p1_0:p1_0:p1_0:p1_0

    do
    {
        xmm0 = _mm_loadu_si64(src);
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
                    _mm_storeu_si64(pval2 + ofs1, xmm0);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 > ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm0 = _mm_loadu_si64(pval2 + ofs1);
                    *pval3 = ofs1;
                }
                else
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 3);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 16);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 48);
                    _mm_storeu_si64(pval2 + ofs1, xmm2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm0 = _mm_loadu_si64(pval2 + ofs1);
                    _mm_storeu_si64(pval2 + ofs2 + 3, xmm1);
                    *pval3 = ofs1;
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 2);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 32);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 32);
                    _mm_storeu_si64(pval2 + ofs1, xmm2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm0 = _mm_loadu_si64(pval2 + ofs1);
                    _mm_storeu_si64(pval2 + ofs2 + 2, xmm1);
                    *pval3 = ofs1;
                }
                else
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 1);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 48);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 16);
                    _mm_storeu_si64(pval2 + ofs1, xmm2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm0 = _mm_loadu_si64(pval2 + ofs1);
                    _mm_storeu_si64(pval2 + ofs2 + 1, xmm1);
                    *pval3 = ofs1;
                }
            }

            counter2++;
        } while (counter2 < 1);

        tmp1 = *params2;
        xmm2 = xmm0;
        xmm4 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_subs_epi16(xmm2, xmm4);
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        tmp1 += (uint16_t)_mm_cvtsi128_si32(xmm2);
        xmm1 = _mm_cvtsi32_si128(tmp1);
        xmm0 = _mm_srli_epi64(xmm0, 16);
        xmm2 = xmm0;
        xmm1 = _mm_slli_epi64(xmm1, 48);
        xmm0 = _mm_add_epi16(xmm0, xmm1);
        xmm4 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_subs_epi16(xmm2, xmm4);
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        tmp1 += (uint16_t)_mm_cvtsi128_si32(xmm2);
        xmm1 = _mm_cvtsi32_si128(tmp1);
        xmm0 = _mm_srli_epi64(xmm0, 16);
        xmm2 = xmm0;
        xmm1 = _mm_slli_epi64(xmm1, 48);
        xmm0 = _mm_add_epi16(xmm0, xmm1);
        xmm4 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_subs_epi16(xmm2, xmm4);
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        tmp1 += (uint16_t)_mm_cvtsi128_si32(xmm2);
        xmm1 = _mm_cvtsi32_si128(tmp1);
        xmm0 = _mm_srli_epi64(xmm0, 16);
        xmm2 = xmm0;
        xmm1 = _mm_slli_epi64(xmm1, 48);
        xmm0 = _mm_add_epi16(xmm0, xmm1);
        xmm4 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_subs_epi16(xmm2, xmm4);
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        tmp1 += (uint16_t)_mm_cvtsi128_si32(xmm2);
        xmm1 = _mm_cvtsi32_si128(tmp1);
        xmm0 = _mm_srli_epi64(xmm0, 16);
        xmm1 = _mm_slli_epi64(xmm1, 48);
        xmm0 = _mm_add_epi16(xmm0, xmm1);
        *params2 = tmp1;

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
                    xmm3 = _mm_loadu_si64(pval2 + ofs1);
                    xmm4 = xmm3;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm0 = _mm_subs_epi16(xmm0, xmm4);
                    xmm2 = xmm0;
                    xmm4 = xmm0;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm3 = _mm_adds_epi16(xmm3, xmm4);
                    xmm0 = xmm3;
                    if (ofs1 != 0)
                    {
                        _mm_storeu_si64(pval2 + ofs1 - 4, xmm2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        _mm_storeu_si64(pval2 + ofs2, xmm2);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 3);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm3 = _mm_loadu_si64(pval2 + ofs1);
                    xmm4 = xmm3;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm0 = _mm_subs_epi16(xmm0, xmm4);
                    xmm4 = xmm0;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 16);
                    xmm3 = _mm_adds_epi16(xmm3, xmm4);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 48);
                    xmm0 = xmm3;
                    if (ofs1 != 0)
                    {
                        _mm_storeu_si64(pval2 + ofs1 - 4, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 3, xmm2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        _mm_storeu_si64(pval2 + ofs2 - 1, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 3, xmm2);
                        *pval3 = ofs1;
                    }
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm3 = _mm_loadu_si64(pval2 + ofs1);
                    xmm4 = xmm3;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm0 = _mm_subs_epi16(xmm0, xmm4);
                    xmm4 = xmm0;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 32);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm3 = _mm_adds_epi16(xmm3, xmm4);
                    xmm1 = _mm_srli_epi64(xmm1, 32);
                    xmm0 = xmm3;
                    if (ofs1 != 0)
                    {
                        _mm_storeu_si64(pval2 + ofs1 - 4, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 2, xmm2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        _mm_storeu_si64(pval2 + ofs2 - 2, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 2, xmm2);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 1);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm3 = _mm_loadu_si64(pval2 + ofs1);
                    xmm4 = xmm3;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm0 = _mm_subs_epi16(xmm0, xmm4);
                    xmm4 = xmm0;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm3 = _mm_adds_epi16(xmm3, xmm4);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 48);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 16);
                    xmm0 = xmm3;
                    if (ofs1 != 0)
                    {
                        _mm_storeu_si64(pval2 + ofs1 - 4, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 1, xmm2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        _mm_storeu_si64(pval2 + ofs2 - 3, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 1, xmm2);
                        *pval3 = ofs1;
                    }
                }
            }

            counter2++;
        } while (counter2 < 5);

        xmm2 = _mm_loadu_si64(dst);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm5);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_adds_epi16(xmm0, xmm2);
        _mm_storeu_si64(dst, xmm0);
        dst += 8;
        counter1 -= 4;
    } while (counter1 > 0);
}

void CCALL simd_1000AD02_c(int32_t counter1, int16_t * RESTRICT params1, uint16_t * RESTRICT params2, uint16_t * RESTRICT data1, uint32_t * RESTRICT data2, uint32_t * RESTRICT data3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    uint32_t counter2, ofs1, ofs2;
    int32_t tmp1;
    int16_t * RESTRICT pval2;
    uint16_t * RESTRICT pval3;

    xmm7 = _mm_loadu_si64(params1);                             // xmm7 = 0000:0000:0000:0000:xxxx:p1_2:p1_1:p1_0
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                      // xmm7 = xxxx:xxxx:p1_2:p1_2:p1_1:p1_1:p1_0:p1_0
    xmm6 = xmm7;
    xmm6 = _mm_shuffle_epi32(xmm6, _MM_SHUFFLE(1, 1, 1, 1));    // xmm6 = p1_1:p1_1:p1_1:p1_1:p1_1:p1_1:p1_1:p1_1
    xmm5 = xmm7;
    xmm5 = _mm_shuffle_epi32(xmm5, _MM_SHUFFLE(2, 2, 2, 2));    // xmm5 = p1_2:p1_2:p1_2:p1_2:p1_2:p1_2:p1_2:p1_2
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));    // xmm7 = p1_0:p1_0:p1_0:p1_0:p1_0:p1_0:p1_0:p1_0

    do
    {
        xmm0 = _mm_loadu_si64(src);
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
                    _mm_storeu_si64(pval2 + ofs1, xmm0);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 > ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm0 = _mm_loadu_si64(pval2 + ofs1);
                    *pval3 = ofs1;
                }
                else
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 3);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 16);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 48);
                    _mm_storeu_si64(pval2 + ofs1, xmm2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm0 = _mm_loadu_si64(pval2 + ofs1);
                    _mm_storeu_si64(pval2 + ofs2 + 3, xmm1);
                    *pval3 = ofs1;
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 2);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 32);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 32);
                    _mm_storeu_si64(pval2 + ofs1, xmm2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm0 = _mm_loadu_si64(pval2 + ofs1);
                    _mm_storeu_si64(pval2 + ofs2 + 2, xmm1);
                    *pval3 = ofs1;
                }
                else
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 1);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 48);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 16);
                    _mm_storeu_si64(pval2 + ofs1, xmm2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm0 = _mm_loadu_si64(pval2 + ofs1);
                    _mm_storeu_si64(pval2 + ofs2 + 1, xmm1);
                    *pval3 = ofs1;
                }
            }

            counter2++;
        } while (counter2 < 1);

        tmp1 = *params2;
        xmm2 = xmm0;
        xmm4 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_subs_epi16(xmm2, xmm4);
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        tmp1 += (uint16_t)_mm_cvtsi128_si32(xmm2);
        xmm1 = _mm_cvtsi32_si128(tmp1);
        xmm0 = _mm_srli_epi64(xmm0, 16);
        xmm2 = xmm0;
        xmm1 = _mm_slli_epi64(xmm1, 48);
        xmm0 = _mm_add_epi16(xmm0, xmm1);
        xmm4 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_subs_epi16(xmm2, xmm4);
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        tmp1 += (uint16_t)_mm_cvtsi128_si32(xmm2);
        xmm1 = _mm_cvtsi32_si128(tmp1);
        xmm0 = _mm_srli_epi64(xmm0, 16);
        xmm2 = xmm0;
        xmm1 = _mm_slli_epi64(xmm1, 48);
        xmm0 = _mm_add_epi16(xmm0, xmm1);
        xmm4 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_subs_epi16(xmm2, xmm4);
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        tmp1 += (uint16_t)_mm_cvtsi128_si32(xmm2);
        xmm1 = _mm_cvtsi32_si128(tmp1);
        xmm0 = _mm_srli_epi64(xmm0, 16);
        xmm2 = xmm0;
        xmm1 = _mm_slli_epi64(xmm1, 48);
        xmm0 = _mm_add_epi16(xmm0, xmm1);
        xmm4 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_subs_epi16(xmm2, xmm4);
        xmm2 = _mm_mulhi_epi16(xmm2, xmm6);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        tmp1 += (uint16_t)_mm_cvtsi128_si32(xmm2);
        xmm1 = _mm_cvtsi32_si128(tmp1);
        xmm0 = _mm_srli_epi64(xmm0, 16);
        xmm1 = _mm_slli_epi64(xmm1, 48);
        xmm0 = _mm_add_epi16(xmm0, xmm1);
        *params2 = tmp1;

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
                    xmm3 = _mm_loadu_si64(pval2 + ofs1);
                    xmm4 = xmm3;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm0 = _mm_subs_epi16(xmm0, xmm4);
                    xmm2 = xmm0;
                    xmm4 = xmm0;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm3 = _mm_adds_epi16(xmm3, xmm4);
                    xmm0 = xmm3;
                    if (ofs1 != 0)
                    {
                        _mm_storeu_si64(pval2 + ofs1 - 4, xmm2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        _mm_storeu_si64(pval2 + ofs2, xmm2);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 3);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm3 = _mm_loadu_si64(pval2 + ofs1);
                    xmm4 = xmm3;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm0 = _mm_subs_epi16(xmm0, xmm4);
                    xmm4 = xmm0;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 16);
                    xmm3 = _mm_adds_epi16(xmm3, xmm4);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 48);
                    xmm0 = xmm3;
                    if (ofs1 != 0)
                    {
                        _mm_storeu_si64(pval2 + ofs1 - 4, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 3, xmm2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        _mm_storeu_si64(pval2 + ofs2 - 1, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 3, xmm2);
                        *pval3 = ofs1;
                    }
                }
            }
            else
            {
                if ((ofs2 & 1) == 0)
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 2);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm3 = _mm_loadu_si64(pval2 + ofs1);
                    xmm4 = xmm3;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm0 = _mm_subs_epi16(xmm0, xmm4);
                    xmm4 = xmm0;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 32);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm3 = _mm_adds_epi16(xmm3, xmm4);
                    xmm1 = _mm_srli_epi64(xmm1, 32);
                    xmm0 = xmm3;
                    if (ofs1 != 0)
                    {
                        _mm_storeu_si64(pval2 + ofs1 - 4, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 2, xmm2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        _mm_storeu_si64(pval2 + ofs2 - 2, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 2, xmm2);
                        *pval3 = ofs1;
                    }
                }
                else
                {
                    xmm2 = _mm_loadu_si64(pval2 + ofs2 + 1);
                    ofs1 += 4;
                    if (UNLIKELY_COND(ofs1 >= ofs2)) UNLIKELY_ATTR
                    {
                        ofs1 = 0;
                    }
                    xmm3 = _mm_loadu_si64(pval2 + ofs1);
                    xmm4 = xmm3;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm0 = _mm_subs_epi16(xmm0, xmm4);
                    xmm4 = xmm0;
                    xmm4 = _mm_mulhi_epi16(xmm4, xmm7);
                    xmm4 = _mm_slli_epi16(xmm4, 1);
                    xmm3 = _mm_adds_epi16(xmm3, xmm4);
                    xmm1 = xmm0;
                    xmm0 = _mm_slli_epi64(xmm0, 48);
                    xmm2 = _mm_adds_epi16(xmm2, xmm0);
                    xmm1 = _mm_srli_epi64(xmm1, 16);
                    xmm0 = xmm3;
                    if (ofs1 != 0)
                    {
                        _mm_storeu_si64(pval2 + ofs1 - 4, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 1, xmm2);
                        *pval3 = ofs1;
                    }
                    else
                    {
                        _mm_storeu_si64(pval2 + ofs2 - 3, xmm2);
                        xmm2 = xmm1;
                        _mm_storeu_si64(pval2 + ofs2 + 1, xmm2);
                        *pval3 = ofs1;
                    }
                }
            }

            counter2++;
        } while (counter2 < 5);

        xmm0 = _mm_mulhi_epi16(xmm0, xmm5);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        _mm_storeu_si64(dst, xmm0);
        dst += 8;
        counter1 -= 4;
    } while (counter1 > 0);
}

// chorus

void CCALL simd_1000B2EE_c(uint8_t * RESTRICT params1, uint8_t * RESTRICT params2, uint16_t * RESTRICT data, uint8_t * RESTRICT params3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    int32_t counter;
    uint32_t length, offset, ofs2, tmp1;
    uint16_t * RESTRICT poffs, * RESTRICT pval;

    xmm7 = _mm_loadu_si64(params2);                             // xmm7 = 0000:0000:0000:0000:p2_6:p2_4:p2_2:p2_0
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                      // xmm7 = p2_6:p2_6:p2_4:p2_4:p2_2:p2_2:p2_0:p2_0
    xmm6 = xmm7;
    xmm6 = _mm_shuffle_epi32(xmm6, _MM_SHUFFLE(1, 1, 1, 1));    // xmm6 = p2_2:p2_2:p2_2:p2_2:p2_2:p2_2:p2_2:p2_2
    xmm5 = xmm7;
    xmm5 = _mm_shuffle_epi32(xmm5, _MM_SHUFFLE(2, 2, 2, 2));    // xmm5 = p2_4:p2_4:p2_4:p2_4:p2_4:p2_4:p2_4:p2_4
    xmm4 = xmm7;
    xmm4 = _mm_shuffle_epi32(xmm4, _MM_SHUFFLE(3, 3, 3, 3));    // xmm4 = p2_6:p2_6:p2_6:p2_6:p2_6:p2_6:p2_6:p2_6
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));    // xmm7 = p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0
    xmm0 = _mm_cvtsi32_si128(*(int32_t *)(params1 + 8));
    xmm7 = _mm_sra_epi16(xmm7, xmm0);                           // xmm7 = p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0 >> *(params1 + 8)
    xmm0 = xmm7;                                                // xmm0 = p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0 >> *(params1 + 8)
    xmm0 = _mm_add_epi16(xmm0, xmm7);                           // xmm0 = 2 * (p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0 >> *(params1 + 8))
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm0);                      // xmm7 = 2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8))
    xmm0 = _mm_add_epi16(xmm0, xmm7);                           // xmm0 = 4 * (p2_0 >> *(params1 + 8)):3 * (p2_0 >> *(params1 + 8)):4 * (p2_0 >> *(params1 + 8)): 3 * (p2_0 >> *(params1 + 8)):4 * (p2_0 >> *(params1 + 8)):3 * (p2_0 >> *(params1 + 8)):4 * (p2_0 >> *(params1 + 8)): 3 * (p2_0 >> *(params1 + 8))
    xmm7 = _mm_unpacklo_epi32(xmm7, xmm0);                      // xmm7 = 4 * (p2_0 >> *(params1 + 8)):3 * (p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8)):4 * (p2_0 >> *(params1 + 8)): 3 * (p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8))
    xmm3 = _mm_set1_epi16(*(uint16_t *)(params2 + 8));          // xmm3 = p2_8:p2_8:p2_8:p2_8:p2_8:p2_8:p2_8:p2_8
    length = *(uint32_t *)(params1 + 4);
    counter = *(int32_t *)params1;
    poffs = *(PTR32(uint16_t) *)params3;
    pval = *(PTR32(uint16_t) *)(params3 + 4);
    offset = *poffs;

    do
    {
        tmp1 = *pval;
        xmm2 = _mm_set1_epi16(tmp1);            // xmm2 = tmp1:tmp1:tmp1:tmp1:tmp1:tmp1:tmp1:tmp1
        xmm2 = _mm_add_epi16(xmm2, xmm7);
        xmm1 = xmm2;
        xmm1 = _mm_srli_epi64(xmm1, 48);
        xmm2 = _mm_slli_epi64(xmm2, 16);
        //xmm0 = _mm_setzero_si128();
        xmm0 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_add_epi16(xmm2, xmm0);
        tmp1 = _mm_cvtsi128_si32(xmm1);
        *pval = tmp1;
        xmm1 = xmm2;
        xmm2 = _mm_srai_epi16(xmm2, 15);
        xmm1 = _mm_xor_si128(xmm1, xmm2);
        xmm1 = _mm_sub_epi16(xmm1, xmm2);
        xmm1 = _mm_srai_epi16(xmm1, 1);
        xmm2 = _mm_cmpeq_epi16(xmm2, xmm2);     // xmm2 = ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff
        xmm2 = _mm_srli_epi16(xmm2, 2);
        xmm1 = _mm_sub_epi16(xmm1, xmm2);
        xmm1 = _mm_slli_epi16(xmm1, 1);
        xmm1 = _mm_mulhi_epi16(xmm1, xmm6);
        xmm1 = _mm_add_epi16(xmm1, xmm2);
        xmm2 = xmm1;
        xmm0 = xmm1;
        xmm1 = _mm_mulhi_epi16(xmm1, xmm5);
        xmm1 = _mm_slli_epi16(xmm1, 1);
        xmm0 = _mm_mullo_epi16(xmm0, xmm5);
        xmm0 = _mm_srli_epi16(xmm0, 15);
        xmm1 = _mm_or_si128(xmm1, xmm0);
        xmm0 = _mm_cmpeq_epi16(xmm0, xmm0);     // xmm0 = ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff
        xmm0 = _mm_srli_epi16(xmm0, 15);
        xmm0 = _mm_slli_epi16(xmm0, 2);
        xmm0 = _mm_cmpgt_epi16(xmm0, xmm1);
        xmm1 = _mm_or_si128(xmm1, xmm0);
        xmm1 = _mm_sub_epi16(xmm1, xmm0);
        xmm0 = _mm_slli_epi16(xmm0, 2);
        xmm1 = _mm_sub_epi16(xmm1, xmm0);
        xmm0 = xmm5;
        xmm2 = _mm_mullo_epi16(xmm2, xmm0);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        xmm2 = _mm_srli_epi16(xmm2, 1);

        ofs2 = offset;
        tmp1 = _mm_cvtsi128_si32(xmm1);
        xmm1 = _mm_srli_epi64(xmm1, 16);
        ofs2 = ofs2 - (uint16_t)tmp1;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);
        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
        xmm0 = _mm_srli_epi64(xmm0, 48);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm2);
        xmm2 = _mm_srli_epi64(xmm2, 16);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);

        ofs2 = offset;
        tmp1 = _mm_cvtsi128_si32(xmm1);
        xmm1 = _mm_srli_epi64(xmm1, 16);
        ofs2 = ofs2 + 1 - (uint16_t)tmp1;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);
        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
        xmm0 = _mm_srli_epi64(xmm0, 48);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm2);
        xmm2 = _mm_srli_epi64(xmm2, 16);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);

        ofs2 = offset;
        tmp1 = _mm_cvtsi128_si32(xmm1);
        xmm1 = _mm_srli_epi64(xmm1, 16);
        ofs2 = ofs2 + 2 - (uint16_t)tmp1;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);
        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
        xmm0 = _mm_srli_epi64(xmm0, 48);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm2);
        xmm2 = _mm_srli_epi64(xmm2, 16);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);

        ofs2 = offset;
        tmp1 = _mm_cvtsi128_si32(xmm1);
        xmm1 = _mm_srli_epi64(xmm1, 16);
        ofs2 = ofs2 + 3 - (uint16_t)tmp1;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);
        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
        xmm0 = _mm_srli_epi64(xmm0, 48);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm2);
        xmm2 = _mm_srli_epi64(xmm2, 16);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);

        xmm0 = _mm_loadu_si64(src);
        src += 8;
        xmm2 = xmm1;
        xmm2 = _mm_mulhi_epi16(xmm2, xmm4);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        xmm0 = _mm_adds_epi16(xmm0, xmm2);
        _mm_storeu_si64(data + offset, xmm0);
        offset += 4;
        xmm0 = xmm1;

        if (UNLIKELY_COND(offset >= length)) UNLIKELY_ATTR
        {
            offset = 0;
        }

        xmm2 = _mm_loadu_si64(dst);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm3);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_adds_epi16(xmm0, xmm2);
        _mm_storeu_si64(dst, xmm0);
        dst += 8;
        counter -= 4;
    } while (counter > 0);

    *poffs = offset;
}

void CCALL simd_1000B7C9_c(uint8_t * RESTRICT params1, uint8_t * RESTRICT params2, uint16_t * RESTRICT data, uint8_t * RESTRICT params3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    __m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
    int32_t counter;
    uint32_t length, offset, ofs2, tmp1;
    uint16_t * RESTRICT poffs, * RESTRICT pval;

    xmm7 = _mm_loadu_si64(params2);                             // xmm7 = 0000:0000:0000:0000:p2_6:p2_4:p2_2:p2_0
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm7);                      // xmm7 = p2_6:p2_6:p2_4:p2_4:p2_2:p2_2:p2_0:p2_0
    xmm6 = xmm7;
    xmm6 = _mm_shuffle_epi32(xmm6, _MM_SHUFFLE(1, 1, 1, 1));    // xmm6 = p2_2:p2_2:p2_2:p2_2:p2_2:p2_2:p2_2:p2_2
    xmm5 = xmm7;
    xmm5 = _mm_shuffle_epi32(xmm5, _MM_SHUFFLE(2, 2, 2, 2));    // xmm5 = p2_4:p2_4:p2_4:p2_4:p2_4:p2_4:p2_4:p2_4
    xmm4 = xmm7;
    xmm4 = _mm_shuffle_epi32(xmm4, _MM_SHUFFLE(3, 3, 3, 3));    // xmm4 = p2_6:p2_6:p2_6:p2_6:p2_6:p2_6:p2_6:p2_6
    xmm7 = _mm_shuffle_epi32(xmm7, _MM_SHUFFLE(0, 0, 0, 0));    // xmm7 = p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0
    xmm0 = _mm_cvtsi32_si128(*(int32_t *)(params1 + 8));
    xmm7 = _mm_sra_epi16(xmm7, xmm0);                           // xmm7 = p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0 >> *(params1 + 8)
    xmm0 = xmm7;                                                // xmm0 = p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0 >> *(params1 + 8)
    xmm0 = _mm_add_epi16(xmm0, xmm7);                           // xmm0 = 2 * (p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0:p2_0 >> *(params1 + 8))
    xmm7 = _mm_unpacklo_epi16(xmm7, xmm0);                      // xmm7 = 2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8))
    xmm0 = _mm_add_epi16(xmm0, xmm7);                           // xmm0 = 4 * (p2_0 >> *(params1 + 8)):3 * (p2_0 >> *(params1 + 8)):4 * (p2_0 >> *(params1 + 8)): 3 * (p2_0 >> *(params1 + 8)):4 * (p2_0 >> *(params1 + 8)):3 * (p2_0 >> *(params1 + 8)):4 * (p2_0 >> *(params1 + 8)): 3 * (p2_0 >> *(params1 + 8))
    xmm7 = _mm_unpacklo_epi32(xmm7, xmm0);                      // xmm7 = 4 * (p2_0 >> *(params1 + 8)):3 * (p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8)):4 * (p2_0 >> *(params1 + 8)): 3 * (p2_0 >> *(params1 + 8)):2 * (p2_0 >> *(params1 + 8)):(p2_0 >> *(params1 + 8))
    xmm3 = _mm_set1_epi16(*(uint16_t *)(params2 + 8));          // xmm3 = p2_8:p2_8:p2_8:p2_8:p2_8:p2_8:p2_8:p2_8
    length = *(uint32_t *)(params1 + 4);
    counter = *(int32_t *)params1;
    poffs = *(PTR32(uint16_t) *)params3;
    pval = *(PTR32(uint16_t) *)(params3 + 4);
    offset = *poffs;

    do
    {
        tmp1 = *pval;
        xmm2 = _mm_set1_epi16(tmp1);            // xmm2 = tmp1:tmp1:tmp1:tmp1:tmp1:tmp1:tmp1:tmp1
        xmm2 = _mm_add_epi16(xmm2, xmm7);
        xmm1 = xmm2;
        xmm1 = _mm_srli_epi64(xmm1, 48);
        xmm2 = _mm_slli_epi64(xmm2, 16);
        //xmm0 = _mm_setzero_si128();
        xmm0 = _mm_cvtsi32_si128(tmp1);
        xmm2 = _mm_add_epi16(xmm2, xmm0);
        tmp1 = _mm_cvtsi128_si32(xmm1);
        *pval = tmp1;
        xmm1 = xmm2;
        xmm2 = _mm_srai_epi16(xmm2, 15);
        xmm1 = _mm_xor_si128(xmm1, xmm2);
        xmm1 = _mm_sub_epi16(xmm1, xmm2);
        xmm1 = _mm_srai_epi16(xmm1, 1);
        xmm2 = _mm_cmpeq_epi16(xmm2, xmm2);     // xmm2 = ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff
        xmm2 = _mm_srli_epi16(xmm2, 2);
        xmm1 = _mm_sub_epi16(xmm1, xmm2);
        xmm1 = _mm_slli_epi16(xmm1, 1);
        xmm1 = _mm_mulhi_epi16(xmm1, xmm6);
        xmm1 = _mm_add_epi16(xmm1, xmm2);
        xmm2 = xmm1;
        xmm0 = xmm1;
        xmm1 = _mm_mulhi_epi16(xmm1, xmm5);
        xmm1 = _mm_slli_epi16(xmm1, 1);
        xmm0 = _mm_mullo_epi16(xmm0, xmm5);
        xmm0 = _mm_srli_epi16(xmm0, 15);
        xmm1 = _mm_or_si128(xmm1, xmm0);
        xmm0 = _mm_cmpeq_epi16(xmm0, xmm0);     // xmm0 = ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff
        xmm0 = _mm_srli_epi16(xmm0, 15);
        xmm0 = _mm_slli_epi16(xmm0, 2);
        xmm0 = _mm_cmpgt_epi16(xmm0, xmm1);
        xmm1 = _mm_or_si128(xmm1, xmm0);
        xmm1 = _mm_sub_epi16(xmm1, xmm0);
        xmm0 = _mm_slli_epi16(xmm0, 2);
        xmm1 = _mm_sub_epi16(xmm1, xmm0);
        xmm0 = xmm5;
        xmm2 = _mm_mullo_epi16(xmm2, xmm0);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        xmm2 = _mm_srli_epi16(xmm2, 1);

        ofs2 = offset;
        tmp1 = _mm_cvtsi128_si32(xmm1);
        xmm1 = _mm_srli_epi64(xmm1, 16);
        ofs2 = ofs2 - (uint16_t)tmp1;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);
        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
        xmm0 = _mm_srli_epi64(xmm0, 48);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm2);
        xmm2 = _mm_srli_epi64(xmm2, 16);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);

        ofs2 = offset;
        tmp1 = _mm_cvtsi128_si32(xmm1);
        xmm1 = _mm_srli_epi64(xmm1, 16);
        ofs2 = ofs2 + 1 - (uint16_t)tmp1;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);
        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
        xmm0 = _mm_srli_epi64(xmm0, 48);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm2);
        xmm2 = _mm_srli_epi64(xmm2, 16);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);

        ofs2 = offset;
        tmp1 = _mm_cvtsi128_si32(xmm1);
        xmm1 = _mm_srli_epi64(xmm1, 16);
        ofs2 = ofs2 + 2 - (uint16_t)tmp1;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);
        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
        xmm0 = _mm_srli_epi64(xmm0, 48);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm2);
        xmm2 = _mm_srli_epi64(xmm2, 16);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);

        ofs2 = offset;
        tmp1 = _mm_cvtsi128_si32(xmm1);
        xmm1 = _mm_srli_epi64(xmm1, 16);
        ofs2 = ofs2 + 3 - (uint16_t)tmp1;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);
        ofs2--;
        if (UNLIKELY_COND(((int32_t)ofs2) < 0)) UNLIKELY_ATTR
        {
            ofs2 += length;
        }

        xmm0 = _mm_loadu_si64(data + ofs2);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm0 = _mm_sub_epi16(xmm0, xmm1);
        xmm0 = _mm_srli_epi64(xmm0, 48);
        xmm0 = _mm_mulhi_epi16(xmm0, xmm2);
        xmm2 = _mm_srli_epi64(xmm2, 16);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        xmm0 = _mm_slli_epi64(xmm0, 48);
        xmm1 = _mm_add_epi16(xmm1, xmm0);

        xmm0 = _mm_loadu_si64(src);
        src += 8;
        xmm2 = xmm1;
        xmm2 = _mm_mulhi_epi16(xmm2, xmm4);
        xmm2 = _mm_slli_epi16(xmm2, 1);
        xmm0 = _mm_adds_epi16(xmm0, xmm2);
        _mm_storeu_si64(data + offset, xmm0);
        offset += 4;
        xmm0 = xmm1;

        if (UNLIKELY_COND(offset >= length)) UNLIKELY_ATTR
        {
            offset = 0;
        }

        xmm0 = _mm_mulhi_epi16(xmm0, xmm3);
        xmm0 = _mm_slli_epi16(xmm0, 1);
        _mm_storeu_si64(dst, xmm0);
        dst += 8;
        counter -= 4;
    } while (counter > 0);

    *poffs = offset;
}