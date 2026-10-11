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

#define CLAMP16(x) if ((x) >= 32768) (x) = 32767; else if ((x) <= -32768) (x) = -32768;


void CCALL simd_10007028_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint32_t mult4, uint32_t mult5, uint8_t * RESTRICT tmp, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int r0, r1, r2, r3, r5, r6, r7, r8, r9, r10;
    int tmp0, tmp1;

    r7 = (int16_t)mult1;
    tmp0 = (int16_t)mult2;
    tmp1 = (int16_t)mult3;
    r5 = (int16_t)mult4;
    r3 = (int16_t)mult5;

    do
    {
        r6 = *(int16_t *)src;
        src += 2;
        r8 = *(int16_t *)dst2;
        r6 = (r6 * r7) >> 16;   // r6 = [src] * mult1
        r9 = *(int16_t *)dst3;
        r0 = (tmp0 * r6) >> 16; // r0 = [src] * mult1 * mult2
        r10 = *(int32_t *)dst1;
        r1 = (tmp1 * r6) >> 16; // r1 = [src] * mult1 * mult3
        r2 = (r5 * r6) >> 16;   // r2 = [src] * mult1 * mult4
        r6 = (r6 * r3) >> 16;   // r6 = [src] * mult1 * mult5
        r0 += r0;               // r0 = ([src] * mult1 * mult2) << 1
        r1 += r1;               // r1 = ([src] * mult1 * mult3) << 1
        r2 += r2;               // r2 = ([src] * mult1 * mult4) << 1
        r6 += r6;               // r6 = ([src] * mult1 * mult5) << 1
        r2 += r8;
        CLAMP16(r2)
        *(int16_t *)dst2 = r2;
        r6 += r9;
        CLAMP16(r6)
        *(int16_t *)dst3 = r6;
        r0 += (int16_t)r10;
        CLAMP16(r0)
        *(int16_t *)dst1 = r0;
        r1 += (int16_t)(r10 >> 16);
        CLAMP16(r1)
        *(int16_t *)(dst1 + 2) = r1;
        dst1 += 4;
        dst2 += 2;
        dst3 += 2;
        counter--;
    } while (counter > 0);
}

void CCALL simd_10007104_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint32_t mult4, uint32_t mult5, uint8_t * RESTRICT tmp, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int r0, r1, r2, r3, r5, r6, r7;
    int tmp0, tmp1;

    r7 = (int16_t)mult1;
    tmp0 = (int16_t)mult2;
    tmp1 = (int16_t)mult3;
    r5 = (int16_t)mult4;
    r3 = (int16_t)mult5;

    do
    {
        r6 = *(int16_t *)src;
        src += 2;
        r6 = (r6 * r7) >> 16;   // r6 = [src] * mult1
        r0 = (tmp0 * r6) >> 16; // r0 = [src] * mult1 * mult2
        r1 = (tmp1 * r6) >> 16; // r1 = [src] * mult1 * mult3
        r2 = (r5 * r6) >> 16;   // r2 = [src] * mult1 * mult4
        r6 = (r6 * r3) >> 16;   // r6 = [src] * mult1 * mult5
        r0 += r0;               // r0 = ([src] * mult1 * mult2) << 1
        r1 += r1;               // r1 = ([src] * mult1 * mult3) << 1
        r2 += r2;               // r2 = ([src] * mult1 * mult4) << 1
        r6 += r6;               // r6 = ([src] * mult1 * mult5) << 1
        *(int16_t *)dst2 = r2;
        *(int16_t *)dst3 = r6;
        *(int16_t *)dst1 = r0;
        *(int16_t *)(dst1 + 2) = r1;
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
    int r2, r3, r4, r5, r6, r7, r8, r9, r10;

    r7 = (int16_t)mult1;
    r5 = (int16_t)mult2;
    r4 = (int16_t)mult3;

    do
    {
        r6 = *(int16_t *)src;
        src += 2;
        r8 = *(int16_t *)dst2;
        r6 = (r6 * r7) >> 16;   // r6 = [src] * mult1
        r9 = *(int16_t *)dst3;
        r2 = (r5 * r6) >> 16;   // r2 = [src] * mult1 * mult2
        r10 = *(int16_t *)dst1;
        r3 = (r4 * r6) >> 16;   // r3 = [src] * mult1 * mult3
        r2 += r2;               // r2 = ([src] * mult1 * mult2) << 1
        r2 += r8;
        CLAMP16(r2)
        *(int16_t *)dst2 = r2;
        r3 += r3;               // r3 = ([src] * mult1 * mult3) << 1
        r3 += r9;
        CLAMP16(r3)
        *(int16_t *)dst3 = r3;
        r6 += r10;
        CLAMP16(r6)
        *(int16_t *)dst1 = r6;
        dst1 += 2;
        dst2 += 2;
        dst3 += 2;
        counter--;
    } while (counter > 0);
}

void CCALL simd_100072A3_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst1, uint8_t * RESTRICT dst2, uint8_t * RESTRICT dst3)
{
    int r2, r3, r4, r5, r6, r7;

    r7 = (int16_t)mult1;
    r5 = (int16_t)mult2;
    r4 = (int16_t)mult3;

    do
    {
        r6 = *(int16_t *)src;
        src += 2;
        r6 = (r6 * r7) >> 16;   // r6 = [src] * mult1
        r2 = (r5 * r6) >> 16;   // r2 = [src] * mult1 * mult2
        r3 = (r4 * r6) >> 16;   // r3 = [src] * mult1 * mult3
        r2 += r2;               // r2 = ([src] * mult1 * mult2) << 1
        *(int16_t *)dst2 = r2;
        r3 += r3;               // r3 = ([src] * mult1 * mult3) << 1
        *(int16_t *)dst3 = r3;
        *(int16_t *)dst1 = r6;
        dst1 += 2;
        dst2 += 2;
        dst3 += 2;
        counter--;
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
    int r0, r1, r2, r3, r6, r7, r8;

    r7 = (int16_t)mult1;
    r2 = (int16_t)mult2;
    r3 = (int16_t)mult3;

    do
    {
        r6 = *(int16_t *)src;
        r8 = *(int32_t *)dst;
        src += 2;
        r6 = (r6 * r7) >> 16;   // r6 = [src] * mult1
        r0 = (r2 * r6) >> 16;   // r0 = [src] * mult1 * mult2
        r1 = (r3 * r6) >> 16;   // r1 = [src] * mult1 * mult3
        r0 += r0;               // r0 = ([src] * mult1 * mult2) << 1
        r1 += r1;               // r1 = ([src] * mult1 * mult3) << 1
        r0 += (int16_t)r8;
        r1 += (int16_t)(r8 >> 16);
        CLAMP16(r0)
        CLAMP16(r1)
        *(int16_t *)dst = r0;
        *(int16_t *)(dst + 2) = r1;
        dst += 4;
        counter--;
    } while (counter > 0);
}

void CCALL simd_100073F1_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult1, uint32_t mult2, uint32_t mult3, uint8_t * RESTRICT dst)
{
    int r0, r1, r2, r3, r6, r7;

    r7 = (int16_t)mult1;
    r2 = (int16_t)mult2;
    r3 = (int16_t)mult3;

    do
    {
        r6 = *(int16_t *)src;
        src += 2;
        r6 = (r6 * r7) >> 16;   // r6 = [src] * mult1
        r0 = (r2 * r6) >> 16;   // r0 = [src] * mult1 * mult2
        r1 = (r3 * r6) >> 16;   // r1 = [src] * mult1 * mult3
        r0 += r0;               // r0 = ([src] * mult1 * mult2) << 1
        r1 += r1;               // r1 = ([src] * mult1 * mult3) << 1
        *(int16_t *)dst = r0;
        *(int16_t *)(dst + 2) = r1;
        dst += 4;
        counter--;
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
    int r0, r7;

    r7 = (int16_t)mult;

    do
    {
        r0 = ((*(int16_t *)src * r7) >> 16) + *(int16_t *)dst;
        CLAMP16(r0)
        *(int16_t *)dst = r0;
        src += 2;
        dst += 2;
        counter--;
    } while (counter > 0);
}

void CCALL simd_100074CD_c(uint8_t * RESTRICT src, int32_t counter, uint32_t mult, uint8_t * RESTRICT dst)
{
    int r7;

    r7 = (int16_t)mult;

    do
    {
        *(int16_t *)dst = (*(int16_t *)src * r7) >> 16;
        src += 2;
        dst += 2;
        counter--;
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
    int r0, r2, r8, r9;
    int32_t counter2;

    multadd2 = (multadd2 << 1) - 1;
    if (multadd2 >= 0)
    {
        do
        {
            r0 = *(int16_t *)src;
            r8 = *(int32_t *)dst;
            counter2 = multadd2;
            r2 = r0;
            src += 2;

            do
            {
                r0 += r2;
                counter2--;
            } while (counter2 > 0);
            CLAMP16(r0)

            r9 = r0 + (int16_t)(r8 >> 16);
            r8 = r0 + (int16_t)r8;
            CLAMP16(r8)
            *(int16_t *)dst = r8;
            CLAMP16(r9)
            *(int16_t *)(dst + 2) = r9;
            dst += 4;
            counter1--;
        } while (counter1 > 0);
    }
    else
    {
        do
        {
            r0 = *(int16_t *)src;
            r8 = *(int32_t *)dst;
            src += 2;
            r9 = r0 + (int16_t)(r8 >> 16);
            r8 = r0 + (int16_t)r8;
            CLAMP16(r8)
            *(int16_t *)dst = r8;
            CLAMP16(r9)
            *(int16_t *)(dst + 2) = r9;
            dst += 4;
            counter1--;
        } while (counter1 > 0);
    }
}

void CCALL simd_1000758D_c(int32_t counter1, int32_t multadd2, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int r0, r1, r8;
    int32_t counter2;

    multadd2 = (multadd2 << 1) - 1;
    if (multadd2 >= 0)
    {
        do
        {
            r0 = *(int16_t *)src;
            r8 = *(int16_t *)dst;
            counter2 = multadd2;
            r1 = r0;
            src += 2;

            do
            {
                r0 += r1;
                counter2--;
            } while (counter2 > 0);
            CLAMP16(r0)

            r0 += r8;
            CLAMP16(r0)
            *(int16_t *)dst = r0;
            dst += 2;
            counter1--;
        } while (counter1 > 0);
    }
    else
    {
        do
        {
            r0 = *(int16_t *)src + *(int16_t *)dst;
            CLAMP16(r0)
            src += 2;
            *(int16_t *)dst = r0;
            dst += 2;
            counter1--;
        } while (counter1 > 0);
    }
}

void CCALL simd_100075CC_c(int32_t counter, uint32_t shift, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int r7, mult;

    r7 = (int16_t)shift - 1;

    if (cs_volume_multiplier == 8192)
    {
        do
        {
            *(int16_t *)dst = *(int16_t *)src >> r7;
            dst += 2;
            src += 2;
            counter--;
        } while (counter > 0);
    }
    else
    {
        mult = cs_volume_multiplier >> r7;
        do
        {
            r7 = (*(int16_t *)src * mult) >> 11;
            CLAMP16(r7)
            *(int16_t *)dst = r7;
            dst += 2;
            src += 2;
            counter--;
        } while (counter > 0);
    }
}

// reverb

void CCALL simd_1000A920_c(int32_t counter1, int16_t * RESTRICT params1, uint16_t * RESTRICT params2, uint16_t * RESTRICT data1, uint32_t * RESTRICT data2, uint32_t * RESTRICT data3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    union {
        int16_t h[4];
        uint64_t d;
    } r0, r1, r2, r3;
    uint32_t counter2, ofs1, ofs2, index;
    int32_t tmp1, tmp2, p1_0, p1_1, p1_2;
    int16_t * RESTRICT pval2;
    uint16_t * RESTRICT pval3;

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
            pval2 = ((PTR32(int16_t) *)data2)[counter2];
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
                    tmp2 = (int32_t)r2.h[1] + (int32_t)r0.h[0];
                    CLAMP16(tmp2)
                    r2.h[1] = tmp2;
                    tmp2 = (int32_t)r2.h[2] + (int32_t)r0.h[1];
                    CLAMP16(tmp2)
                    r2.h[2] = tmp2;
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r0.h[2];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
                    r1.d = r0.d >> 48;
                    unaligned_write_64(pval2 + ofs1, r2.d);
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
                    tmp2 = (int32_t)r2.h[2] + (int32_t)r0.h[0];
                    CLAMP16(tmp2)
                    r2.h[2] = tmp2;
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r0.h[1];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
                    r1.d = r0.d >> 32;
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
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r0.h[0];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
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

        tmp1 = *(int16_t *)params2;
        tmp2 = (int32_t)r0.h[0] - tmp1;
        CLAMP16(tmp2)
        tmp1 += ((tmp2 * p1_1) >> 16) << 1;
        r0.h[0] = tmp1;
        tmp2 = (int32_t)r0.h[1] - (int32_t)tmp1;
        CLAMP16(tmp2)
        tmp1 += ((tmp2 * p1_1) >> 16) << 1;
        r0.h[1] = tmp1;
        tmp2 = (int32_t)r0.h[2] - (int32_t)tmp1;
        CLAMP16(tmp2)
        tmp1 += ((tmp2 * p1_1) >> 16) << 1;
        r0.h[2] = tmp1;
        tmp2 = (int32_t)r0.h[3] - (int32_t)tmp1;
        CLAMP16(tmp2)
        tmp1 += ((tmp2 * p1_1) >> 16) << 1;
        r0.h[3] = tmp1;
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
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 4; index++)
                    {
                        tmp2 = (int32_t)r0.h[index] - (((r3.h[index] * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r2.h[index] = tmp2;
                        tmp2 = (int32_t)r3.h[index] + (((tmp2 * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r0.h[index] = tmp2;
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

                    for (index = 0; index < 4; index++)
                    {
                        tmp2 = (int32_t)r0.h[index] - (((r3.h[index] * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r1.h[index] = tmp2;
                        tmp2 = (int32_t)r3.h[index] + (((tmp2 * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r0.h[index] = tmp2;
                    }

                    tmp2 = (int32_t)r2.h[1] + (int32_t)r1.h[0];
                    CLAMP16(tmp2)
                    r2.h[1] = tmp2;
                    tmp2 = (int32_t)r2.h[2] + (int32_t)r1.h[1];
                    CLAMP16(tmp2)
                    r2.h[2] = tmp2;
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r1.h[2];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
                    r1.d = r1.d >> 48;

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

                    for (index = 0; index < 4; index++)
                    {
                        tmp2 = (int32_t)r0.h[index] - (((r3.h[index] * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r1.h[index] = tmp2;
                        tmp2 = (int32_t)r3.h[index] + (((tmp2 * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r0.h[index] = tmp2;
                    }

                    tmp2 = (int32_t)r2.h[2] + (int32_t)r1.h[0];
                    CLAMP16(tmp2)
                    r2.h[2] = tmp2;
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r1.h[1];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
                    r1.d = r1.d >> 32;

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

                    for (index = 0; index < 4; index++)
                    {
                        tmp2 = (int32_t)r0.h[index] - (((r3.h[index] * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r1.h[index] = tmp2;
                        tmp2 = (int32_t)r3.h[index] + (((tmp2 * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r0.h[index] = tmp2;
                    }

                    tmp2 = (int32_t)r2.h[3] + (int32_t)r1.h[0];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
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
        tmp2 = (int32_t)r3.h[0] + (int16_t)(((r0.h[0] * p1_2) >> 16) << 1);
        CLAMP16(tmp2)
        r0.h[0] = tmp2;
        tmp2 = (int32_t)r3.h[1] + (int16_t)(((r0.h[1] * p1_2) >> 16) << 1);
        CLAMP16(tmp2)
        r0.h[1] = tmp2;
        tmp2 = (int32_t)r3.h[2] + (int16_t)(((r0.h[2] * p1_2) >> 16) << 1);
        CLAMP16(tmp2)
        r0.h[2] = tmp2;
        tmp2 = (int32_t)r3.h[3] + (int16_t)(((r0.h[3] * p1_2) >> 16) << 1);
        CLAMP16(tmp2)
        r0.h[3] = tmp2;
        *(uint64_t *)dst = r0.d;
        dst += 8;
        counter1 -= 4;
    } while (counter1 > 0);
}

void CCALL simd_1000AD02_c(int32_t counter1, int16_t * RESTRICT params1, uint16_t * RESTRICT params2, uint16_t * RESTRICT data1, uint32_t * RESTRICT data2, uint32_t * RESTRICT data3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    union {
        int16_t h[4];
        uint64_t d;
    } r0, r1, r2, r3;
    uint32_t counter2, ofs1, ofs2, index;
    int32_t tmp1, tmp2, p1_0, p1_1, p1_2;
    int16_t * RESTRICT pval2;
    uint16_t * RESTRICT pval3;

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
            pval2 = ((PTR32(int16_t) *)data2)[counter2];
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
                    tmp2 = (int32_t)r2.h[1] + (int32_t)r0.h[0];
                    CLAMP16(tmp2)
                    r2.h[1] = tmp2;
                    tmp2 = (int32_t)r2.h[2] + (int32_t)r0.h[1];
                    CLAMP16(tmp2)
                    r2.h[2] = tmp2;
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r0.h[2];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
                    r1.d = r0.d >> 48;
                    unaligned_write_64(pval2 + ofs1, r2.d);
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
                    tmp2 = (int32_t)r2.h[2] + (int32_t)r0.h[0];
                    CLAMP16(tmp2)
                    r2.h[2] = tmp2;
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r0.h[1];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
                    r1.d = r0.d >> 32;
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
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r0.h[0];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
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

        tmp1 = *(int16_t *)params2;
        tmp2 = (int32_t)r0.h[0] - tmp1;
        CLAMP16(tmp2)
        tmp1 += ((tmp2 * p1_1) >> 16) << 1;
        r0.h[0] = tmp1;
        tmp2 = (int32_t)r0.h[1] - (int32_t)tmp1;
        CLAMP16(tmp2)
        tmp1 += ((tmp2 * p1_1) >> 16) << 1;
        r0.h[1] = tmp1;
        tmp2 = (int32_t)r0.h[2] - (int32_t)tmp1;
        CLAMP16(tmp2)
        tmp1 += ((tmp2 * p1_1) >> 16) << 1;
        r0.h[2] = tmp1;
        tmp2 = (int32_t)r0.h[3] - (int32_t)tmp1;
        CLAMP16(tmp2)
        tmp1 += ((tmp2 * p1_1) >> 16) << 1;
        r0.h[3] = tmp1;
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
                    r3.d = unaligned_read_64(pval2 + ofs1);

                    for (index = 0; index < 4; index++)
                    {
                        tmp2 = (int32_t)r0.h[index] - (((r3.h[index] * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r2.h[index] = tmp2;
                        tmp2 = (int32_t)r3.h[index] + (((tmp2 * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r0.h[index] = tmp2;
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

                    for (index = 0; index < 4; index++)
                    {
                        tmp2 = (int32_t)r0.h[index] - (((r3.h[index] * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r1.h[index] = tmp2;
                        tmp2 = (int32_t)r3.h[index] + (((tmp2 * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r0.h[index] = tmp2;
                    }

                    tmp2 = (int32_t)r2.h[1] + (int32_t)r1.h[0];
                    CLAMP16(tmp2)
                    r2.h[1] = tmp2;
                    tmp2 = (int32_t)r2.h[2] + (int32_t)r1.h[1];
                    CLAMP16(tmp2)
                    r2.h[2] = tmp2;
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r1.h[2];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
                    r1.d = r1.d >> 48;

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

                    for (index = 0; index < 4; index++)
                    {
                        tmp2 = (int32_t)r0.h[index] - (((r3.h[index] * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r1.h[index] = tmp2;
                        tmp2 = (int32_t)r3.h[index] + (((tmp2 * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r0.h[index] = tmp2;
                    }

                    tmp2 = (int32_t)r2.h[2] + (int32_t)r1.h[0];
                    CLAMP16(tmp2)
                    r2.h[2] = tmp2;
                    tmp2 = (int32_t)r2.h[3] + (int32_t)r1.h[1];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
                    r1.d = r1.d >> 32;

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

                    for (index = 0; index < 4; index++)
                    {
                        tmp2 = (int32_t)r0.h[index] - (((r3.h[index] * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r1.h[index] = tmp2;
                        tmp2 = (int32_t)r3.h[index] + (((tmp2 * p1_0) >> 16) << 1);
                        CLAMP16(tmp2)
                        r0.h[index] = tmp2;
                    }

                    tmp2 = (int32_t)r2.h[3] + (int32_t)r1.h[0];
                    CLAMP16(tmp2)
                    r2.h[3] = tmp2;
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

        r0.h[0] = ((r0.h[0] * p1_2) >> 16) << 1;
        r0.h[1] = ((r0.h[1] * p1_2) >> 16) << 1;
        r0.h[2] = ((r0.h[2] * p1_2) >> 16) << 1;
        r0.h[3] = ((r0.h[3] * p1_2) >> 16) << 1;
        *(uint64_t *)dst = r0.d;
        dst += 8;
        counter1 -= 4;
    } while (counter1 > 0);
}

// chorus

void CCALL simd_1000B2EE_c(uint8_t * RESTRICT params1, uint8_t * RESTRICT params2, uint16_t * RESTRICT data, uint8_t * RESTRICT params3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16_t r0, r1[4], p2[4];
    int32_t counter, tmp1, r2, r7;
    uint32_t length, offset, ofs2, index;
    uint16_t * RESTRICT poffs, * RESTRICT pval;

    r7 = (*(int16_t *)params2) >> (*(uint16_t *)(params1 + 8));
    p2[0] = *(int16_t *)(params2 + 2);
    p2[1] = *(int16_t *)(params2 + 4);
    p2[2] = *(int16_t *)(params2 + 6);
    p2[3] = *(int16_t *)(params2 + 8);
    length = *(uint32_t *)(params1 + 4);
    counter = *(int32_t *)params1;
    poffs = *(PTR32(uint16_t) *)params3;
    pval = *(PTR32(uint16_t) *)(params3 + 4);
    offset = *poffs;

    do
    {
        r1[0] = *pval;
        r1[1] = r1[0] + r7;
        r1[2] = r1[0] + 2 * r7;
        r1[3] = r1[0] + 3 * r7;
        *pval = r1[0] + 4 * r7;

        for (index = 0; index < 4; index++)
        {
            if (r1[index] < 0) r1[index] = -r1[index];
            r1[index] = ((r1[index] >> 1) - 0x3fff) << 1;
            r1[index] = ((r1[index] * (int32_t)p2[0]) >> 16) + 0x3fff;
            r2 = (r1[index] * (int32_t)p2[1]) & 0x7fff;
            r1[index] = (r1[index] * (int32_t)p2[1]) >> 15;
            if (r1[index] < 4) r1[index] = 4;

            ofs2 = offset + index - r1[index];
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

            r1[index] = data[ofs2] - tmp1;

            r1[index] = (((r1[index] * r2) >> 16) << 1) + tmp1;
        }

        for (index = 0; index < 4; index++)
        {
            r2 = ((int16_t *)src)[index];

            r0 = ((r1[index] * (int32_t)p2[2]) >> 16) << 1;
            r2 += r0;
            CLAMP16(r2)
            data[offset + index] = r2;

            r2 = ((int16_t *)dst)[index];
            r0 = ((r1[index] * (int32_t)p2[3]) >> 16) << 1;
            r2 += r0;
            CLAMP16(r2)
            ((int16_t *)dst)[index] = r2;
        }

        src += 8;
        offset += 4;

        if (UNLIKELY_COND(offset >= length)) UNLIKELY_ATTR
        {
            offset = 0;
        }

        dst += 8;
        counter -= 4;
    } while (counter > 0);

    *poffs = offset;
}

void CCALL simd_1000B7C9_c(uint8_t * RESTRICT params1, uint8_t * RESTRICT params2, uint16_t * RESTRICT data, uint8_t * RESTRICT params3, uint8_t * RESTRICT dst, uint8_t * RESTRICT src)
{
    int16_t r0, r1[4], p2[4];
    int32_t counter, tmp1, r2, r7;
    uint32_t length, offset, ofs2, index;
    uint16_t * RESTRICT poffs, * RESTRICT pval;

    r7 = (*(int16_t *)params2) >> (*(uint16_t *)(params1 + 8));
    p2[0] = *(int16_t *)(params2 + 2);
    p2[1] = *(int16_t *)(params2 + 4);
    p2[2] = *(int16_t *)(params2 + 6);
    p2[3] = *(int16_t *)(params2 + 8);
    length = *(uint32_t *)(params1 + 4);
    counter = *(int32_t *)params1;
    poffs = *(PTR32(uint16_t) *)params3;
    pval = *(PTR32(uint16_t) *)(params3 + 4);
    offset = *poffs;

    do
    {
        r1[0] = *pval;
        r1[1] = r1[0] + r7;
        r1[2] = r1[0] + 2 * r7;
        r1[3] = r1[0] + 3 * r7;
        *pval = r1[0] + 4 * r7;

        for (index = 0; index < 4; index++)
        {
            if (r1[index] < 0) r1[index] = -r1[index];
            r1[index] = ((r1[index] >> 1) - 0x3fff) << 1;
            r1[index] = ((r1[index] * (int32_t)p2[0]) >> 16) + 0x3fff;
            r2 = (r1[index] * (int32_t)p2[1]) & 0x7fff;
            r1[index] = (r1[index] * (int32_t)p2[1]) >> 15;
            if (r1[index] < 4) r1[index] = 4;

            ofs2 = offset + index - r1[index];
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

            r1[index] = data[ofs2] - tmp1;

            r1[index] = (((r1[index] * r2) >> 16) << 1) + tmp1;
        }

        for (index = 0; index < 4; index++)
        {
            r2 = ((int16_t *)src)[index];

            r0 = ((r1[index] * (int32_t)p2[2]) >> 16) << 1;
            r2 += r0;
            CLAMP16(r2)
            data[offset + index] = r2;

            r0 = ((r1[index] * (int32_t)p2[3]) >> 16) << 1;
            ((int16_t *)dst)[index] = r0;
        }

        src += 8;
        offset += 4;

        if (UNLIKELY_COND(offset >= length)) UNLIKELY_ATTR
        {
            offset = 0;
        }

        dst += 8;
        counter -= 4;
    } while (counter > 0);

    *poffs = offset;
}