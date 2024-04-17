/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 TheVice
 *
 * C# port of Finite State Entropy codec (https://github.com/Cyan4973/FiniteStateEntropy).
 *
 * Original project copyright is next:
 * Copyright (c) 2013-2020, Yann Collet, Facebook, Inc.
 *
 * You can contact the author at :
 * - Source repository : https://github.com/Cyan4973/FiniteStateEntropy
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 *
 */

using System;
using System.IO;

namespace FiniteStateEntropy
{

    public class Entropy_Common
    {
        public static bool FSE_isError(ulong code)
        {
            return Error_Public.ERR_isError(code);
        }

        public static ulong FSE_readNCount(
            short[] normalizedCounter,
            uint[] maxSVPtr,
            int[] tableLogPtr,
            byte[] headerBuffer,
            ulong hbSize)
        {
            int istart = 0;
            int iend = istart + (int)hbSize;
            int ip = istart;
            
            if (hbSize < 4)
            {
                var buffer = new byte[4];
                Array.Clear(buffer, 0, buffer.Length);
                Array.Copy(headerBuffer, buffer, (byte)hbSize);
                //
                var countSize = FSE_readNCount(normalizedCounter, maxSVPtr, tableLogPtr, buffer, (byte)buffer.Length);

                if (countSize > hbSize)
                {
                    return Error_Public.FSE_Error(Error_Public.FSE_ErrorCode.FSE_error_corruption_detected);
                }

                return countSize;
            }

            Array.Clear(normalizedCounter, 0, (int)(maxSVPtr[0] + 1));
            var bitStream = Mem.MEM_readLE32(headerBuffer, ip);
            var nbBits = (int)((bitStream & 0xF) + Fse.FSE_MIN_TABLELOG);

            if (nbBits > Fse.FSE_TABLELOG_ABSOLUTE_MAX)
            {
                return Error_Public.FSE_Error(Error_Public.FSE_ErrorCode.FSE_error_tableLog_tooLarge);
            }

            bitStream >>= 4;
            int bitCount = 4;
            tableLogPtr[0] = nbBits;
            int remaining = (1 << nbBits) + 1;
            int threshold = 1 << nbBits;
            nbBits++;
            uint charnum = 0;
            bool previous0 = false;

            while ((remaining > 1) && (charnum <= maxSVPtr[0]))
            {
                if (previous0)
                {
                    var n0 = charnum;

                    while ((bitStream & 0xFFFF) == 0xFFFF)
                    {
                        n0 += 24;

                        if (ip < iend - 5)
                        {
                            ip += 2;
                            bitStream = Mem.MEM_readLE32(headerBuffer, ip) >> bitCount;
                        }
                        else
                        {
                            bitStream >>= 16;
                            bitCount += 16;
                        }
                    }

                    while ((bitStream & 3) == 3)
                    {
                        n0 += 3;
                        bitStream >>= 2;
                        bitCount += 2;
                    }

                    n0 += bitStream & 3;
                    bitCount += 2;

                    if (n0 > maxSVPtr[0])
                    {
                        return Error_Public.FSE_Error(Error_Public.FSE_ErrorCode.FSE_error_maxSymbolValue_tooSmall);
                    }

                    while (charnum < n0)
                    {
                        normalizedCounter[charnum++] = 0;
                    }

                    if ((ip <= iend - 7) || (ip + (bitCount >> 3) <= iend - 4))
                    {
                        if ((bitCount >> 3) > 3)
                        {
                            throw new ArgumentOutOfRangeException("bitCount");
                        }

                        ip += bitCount >> 3;
                        bitCount &= 7;
                        bitStream = Mem.MEM_readLE32(headerBuffer, ip) >> bitCount;
                    }
                    else
                    {
                        bitStream >>= 2;
                    }
                }
                {
                    int max = (2 * threshold - 1) - remaining;
                    int count;

                    if ((bitStream & (threshold - 1)) < (uint)max)
                    {
                        count = (int)(bitStream & (threshold - 1));
                        bitCount += nbBits - 1;
                    }
                    else
                    {
                        count = (int)(bitStream & (2 * threshold - 1));

                        if (count >= threshold)
                        {
                            count -= max;
                        }

                        bitCount += nbBits;
                    }

                    count--;
                    remaining -= count < 0 ? -count : count;
                    normalizedCounter[charnum++] = (short)count;
                    previous0 = count == 0;

                    while (remaining < threshold)
                    {
                        nbBits--;
                        threshold >>= 1;
                    }

                    if ((ip <= iend - 7) || (ip + (bitCount >> 3) <= iend - 4))
                    {
                        ip += bitCount >> 3;
                        bitCount &= 7;
                    }
                    else
                    {
                        bitCount -= (int)(8 * (iend - 4 - ip));
                        ip = iend - 4;
                    }

                    bitStream = Mem.MEM_readLE32(headerBuffer, ip) >> (bitCount & 31);
                }
            }


            if (remaining != 1)
            {
                return Error_Public.FSE_Error(Error_Public.FSE_ErrorCode.FSE_error_corruption_detected);
            }

            if (32 < bitCount)
            {
                return Error_Public.FSE_Error(Error_Public.FSE_ErrorCode.FSE_error_corruption_detected);
            }

            maxSVPtr[0] = charnum - 1;
            ip += (bitCount + 7) >> 3;

            return (ulong)(ip - istart);
        }
    }
}
