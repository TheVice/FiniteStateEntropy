/* ******************************************************************
 * Common functions of New Generation Entropy library
 * Copyright (c) 2016-2020, Yann Collet, Facebook, Inc.
 *
 *  You can contact the author at :
 *  - FSE+HUF source repository : https://github.com/Cyan4973/FiniteStateEntropy
 *  - Public forum : https://groups.google.com/forum/#!forum/lz4c
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
****************************************************************** */

/* *************************************
*  Dependencies
***************************************/
#include "mem.h"
#include "error_private.h"       /* ERR_*, ERROR */
#define FSE_STATIC_LINKING_ONLY  /* FSE_MIN_TABLELOG */
#include "fse.h"
#define HUF_STATIC_LINKING_ONLY  /* HUF_TABLELOG_ABSOLUTEMAX */
#include "huf.h"

#ifndef ORIGINAL

#include "buffer.h"
#include "common.h"
#include "file_system.h"
#include "path.h"
#include "string_unit.h"
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#endif

/*===   Version   ===*/
unsigned FSE_versionNumber(void) { return FSE_VERSION_NUMBER; }


/*===   Error Management   ===*/
unsigned FSE_isError(size_t code) { return ERR_isError(code); }
const char* FSE_getErrorName(size_t code) { return ERR_getErrorName(code); }

unsigned HUF_isError(size_t code) { return ERR_isError(code); }
const char* HUF_getErrorName(size_t code) { return ERR_getErrorName(code); }


/*-**************************************************************
*  FSE NCount encoding-decoding
****************************************************************/
#ifndef ORIGINAL
size_t FSE_readNCount(short* normalizedCounter, unsigned* maxSVPtr, unsigned* tableLogPtr,
    const void* headerBuffer, size_t hbSize)
{
    uint8_t command_path[BUFFER_SIZE_OF];

    if (!buffer_init(command_path, BUFFER_SIZE_OF))
    {
        buffer_release(command_path);
        fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
        return ERROR(GENERIC);
    }

    const uint8_t* tmp = (const uint8_t*)"dotnet \"";

    if (!buffer_append(command_path, tmp, common_count_bytes_until(tmp, 0)))
    {
        buffer_release(command_path);
        fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
        return ERROR(GENERIC);
    }

    if (!GetPathToTheRunner(command_path))
    {
        buffer_release(command_path);
        fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
        return ERROR(GENERIC);
    }

    tmp = (const uint8_t*)"\" ";

    if (!buffer_append(command_path, tmp, common_count_bytes_until(tmp, 0)))
    {
        buffer_release(command_path);
        fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
        return ERROR(GENERIC);
    }

    tmp = (const uint8_t*)__FUNCTION__;

    if (!buffer_append(command_path, tmp, common_count_bytes_until(tmp, 0)))
    {
        buffer_release(command_path);
        fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
        return ERROR(GENERIC);
    }

    if (!buffer_push_back(command_path, ' '))
    {
        buffer_release(command_path);
        fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
        return ERROR(GENERIC);
    }

    for (uint8_t i = 0; i < 6; ++i)
    {
        tmp = (const uint8_t*)"\"";

        if (!buffer_append(command_path, tmp, common_count_bytes_until(tmp, 0)))
        {
            buffer_release(command_path);
            fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
            return ERROR(GENERIC);
        }

        void* file = NULL;

        if (!OpenTmpFile(command_path, &file))
        {
            file_close(file);
            buffer_release(command_path);
            fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
            return ERROR(GENERIC);
        }

        switch (i)
        {
            case 0:
                for (uint32_t index = 0, count = (*maxSVPtr + 1); index < count; ++index)
                {
                    fwrite(&normalizedCounter[index], sizeof(short), 1, (FILE*)file);
                }
                break;
            case 1:
                fwrite(&maxSVPtr[0], sizeof(unsigned), 1, (FILE*)file);
                break;
            case 2:
                fwrite(&tableLogPtr[0], sizeof(unsigned), 1, (FILE*)file);
                break;
            case 3:
                for (size_t index = 0; index < hbSize; ++index)
                {
                    uint8_t item = ((const uint8_t*)headerBuffer)[index];
                    fwrite(&item, sizeof(uint8_t), 1, (FILE*)file);
                }
                break;
            case 4:
                fwrite(&hbSize, sizeof(size_t), 1, (FILE*)file);
                break;
            case 5:
            default:
                break;
        }

        file_close(file);
        tmp = (const uint8_t*)"\" ";

        if (!buffer_append(command_path, tmp, common_count_bytes_until(tmp, 0)))
        {
            buffer_release(command_path);
            fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
            return ERROR(GENERIC);
        }
    }

    if (!buffer_push_back(command_path, 0))
    {
        buffer_release(command_path);
        fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
        return ERROR(GENERIC);
    }

    if (EXIT_SUCCESS != system(buffer_char_data(command_path, 0)))
    {
        buffer_release(command_path);
        fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
        return ERROR(GENERIC);
    }

    size_t returned = ERROR(GENERIC);

    {
        uint8_t i = 0;
        const uint8_t* finish = (const uint8_t*)buffer_data_finish(&command_path);
        const uint8_t* pos = buffer_uint8_t_data(&command_path, 0);
        const uint8_t* start = NULL;
        uint32_t char_set;
        //
        const uint8_t* func = (const uint8_t*)__FUNCTION__;
        char_set = (uint32_t)string_index_of(pos, finish, func, func + common_count_bytes_until(func, 0));
        pos += char_set;
        pos += common_count_bytes_until(func, 0);

        while (NULL != (pos = string_enumerate(pos, finish, &char_set)))
        {
            if ('\"' != char_set)
            {
                continue;
            }

            if (NULL == start)
            {
                start = pos;
            }
            else
            {
                if (3 == i || 4 == i)
                {
                    start = NULL;
                    ++i;
                    continue;
                }
                else if (5 < i)
                {
                    break;
                }

                uint8_t* ptr = buffer_uint8_t_data(&command_path, 0);
                ptr += pos - ptr - 1;
                *ptr = 0;
                void* file;

                if (!file_open(start, (const uint8_t*)"rb", &file))
                {
                    fprintf(stderr, "%s\n", (const char*)start);
                    buffer_release(command_path);
                    fprintf(stderr, "FAILED to open file: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
                    return ERROR(GENERIC);
                }

                switch (i)
                {
                    case 0:
                        for (uint32_t index = 0, count = (*maxSVPtr + 1); index < count; ++index)
                        {
                            if (1 != fread(&normalizedCounter[index], sizeof(short), 1, (FILE*)file))
                            {
                                file_close(file);
                                fprintf(stderr, "%s\n", (const char*)start);
                                buffer_release(command_path);
                                fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
                                return ERROR(GENERIC);
                            }
                        }
                        break;
                    case 1:
                        if (1 != fread(&maxSVPtr[0], sizeof(unsigned), 1, (FILE*)file))
                        {
                            file_close(file);
                            fprintf(stderr, "%s\n", (const char*)start);
                            buffer_release(command_path);
                            fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
                            return ERROR(GENERIC);
                        }
                        break;
                    case 2:
                        if (1 != fread(&tableLogPtr[0], sizeof(unsigned), 1, (FILE*)file))
                        {
                            file_close(file);
                            fprintf(stderr, "%s\n", (const char*)start);
                            buffer_release(command_path);
                            fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
                            return ERROR(GENERIC);
                        }
                        break;
                    case 5:
                        if (1 != fread(&returned, sizeof(size_t), 1, (FILE*)file))
                        {
                            file_close(file);
                            fprintf(stderr, "%s\n", (const char*)start);
                            buffer_release(command_path);
                            fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
                            return ERROR(GENERIC);
                        }
                        break;
                    case 3:
                    case 4:
                    default:
                        break;
                }

                file_close(file);
                file_delete(start);
                start = NULL;
                ++i;
            }
        }
    }

    buffer_release(command_path);
    //
    return returned;
}
#else
size_t FSE_readNCount (short* normalizedCounter, unsigned* maxSVPtr, unsigned* tableLogPtr,
                 const void* headerBuffer, size_t hbSize)
{
    const BYTE* const istart = (const BYTE*) headerBuffer;
    const BYTE* const iend = istart + hbSize;
    const BYTE* ip = istart;
    int nbBits;
    int remaining;
    int threshold;
    U32 bitStream;
    int bitCount;
    unsigned charnum = 0;
    int previous0 = 0;

    if (hbSize < 4) {
        /* This function only works when hbSize >= 4 */
        char buffer[4] = {0};
        memcpy(buffer, headerBuffer, hbSize);
        {   size_t const countSize = FSE_readNCount(normalizedCounter, maxSVPtr, tableLogPtr,
                                                    buffer, sizeof(buffer));
            if (FSE_isError(countSize)) return countSize;
            if (countSize > hbSize) return ERROR(corruption_detected);
            return countSize;
    }   }
    assert(hbSize >= 4);

    /* init */
    memset(normalizedCounter, 0, (*maxSVPtr+1) * sizeof(normalizedCounter[0]));   /* all symbols not present in NCount have a frequency of 0 */
    bitStream = MEM_readLE32(ip);
    nbBits = (bitStream & 0xF) + FSE_MIN_TABLELOG;   /* extract tableLog */
    if (nbBits > FSE_TABLELOG_ABSOLUTE_MAX) return ERROR(tableLog_tooLarge);
    bitStream >>= 4;
    bitCount = 4;
    *tableLogPtr = nbBits;
    remaining = (1<<nbBits)+1;
    threshold = 1<<nbBits;
    nbBits++;

    while ((remaining>1) & (charnum<=*maxSVPtr)) {
        if (previous0) {
            unsigned n0 = charnum;
            while ((bitStream & 0xFFFF) == 0xFFFF) {
                n0 += 24;
                if (ip < iend-5) {
                    ip += 2;
                    bitStream = MEM_readLE32(ip) >> bitCount;
                } else {
                    bitStream >>= 16;
                    bitCount   += 16;
            }   }
            while ((bitStream & 3) == 3) {
                n0 += 3;
                bitStream >>= 2;
                bitCount += 2;
            }
            n0 += bitStream & 3;
            bitCount += 2;
            if (n0 > *maxSVPtr) return ERROR(maxSymbolValue_tooSmall);
            while (charnum < n0) normalizedCounter[charnum++] = 0;
            if ((ip <= iend-7) || (ip + (bitCount>>3) <= iend-4)) {
                assert((bitCount >> 3) <= 3); /* For first condition to work */
                ip += bitCount>>3;
                bitCount &= 7;
                bitStream = MEM_readLE32(ip) >> bitCount;
            } else {
                bitStream >>= 2;
        }   }
        {   int const max = (2*threshold-1) - remaining;
            int count;

            if ((bitStream & (threshold-1)) < (U32)max) {
                count = bitStream & (threshold-1);
                bitCount += nbBits-1;
            } else {
                count = bitStream & (2*threshold-1);
                if (count >= threshold) count -= max;
                bitCount += nbBits;
            }

            count--;   /* extra accuracy */
            remaining -= count < 0 ? -count : count;   /* -1 means +1 */
            normalizedCounter[charnum++] = (short)count;
            previous0 = !count;
            while (remaining < threshold) {
                nbBits--;
                threshold >>= 1;
            }

            if ((ip <= iend-7) || (ip + (bitCount>>3) <= iend-4)) {
                ip += bitCount>>3;
                bitCount &= 7;
            } else {
                bitCount -= (int)(8 * (iend - 4 - ip));
                ip = iend - 4;
            }
            bitStream = MEM_readLE32(ip) >> (bitCount & 31);
    }   }   /* while ((remaining>1) & (charnum<=*maxSVPtr)) */
    if (remaining != 1) return ERROR(corruption_detected);
    if (bitCount > 32) return ERROR(corruption_detected);
    *maxSVPtr = charnum-1;

    ip += (bitCount+7)>>3;
    return ip-istart;
}
#endif


/*! HUF_readStats() :
    Read compact Huffman tree, saved by HUF_writeCTable().
    `huffWeight` is destination buffer.
    `rankStats` is assumed to be a table of at least HUF_TABLELOG_MAX U32.
    @return : size read from `src` , or an error Code .
    Note : Needed by HUF_readCTable() and HUF_readDTableX?() .
*/
size_t HUF_readStats(BYTE* huffWeight, size_t hwSize, U32* rankStats,
                     U32* nbSymbolsPtr, U32* tableLogPtr,
                     const void* src, size_t srcSize)
{
    U32 weightTotal;
    const BYTE* ip = (const BYTE*) src;
    size_t iSize;
    size_t oSize;

    if (!srcSize) return ERROR(srcSize_wrong);
    iSize = ip[0];
    /* memset(huffWeight, 0, hwSize);   *//* is not necessary, even though some analyzer complain ... */

    if (iSize >= 128) {  /* special header */
        oSize = iSize - 127;
        iSize = ((oSize+1)/2);
        if (iSize+1 > srcSize) return ERROR(srcSize_wrong);
        if (oSize >= hwSize) return ERROR(corruption_detected);
        ip += 1;
        {   U32 n;
            for (n=0; n<oSize; n+=2) {
                huffWeight[n]   = ip[n/2] >> 4;
                huffWeight[n+1] = ip[n/2] & 15;
    }   }   }
    else  {   /* header compressed with FSE (normal case) */
        FSE_DTable fseWorkspace[FSE_DTABLE_SIZE_U32(6)];  /* 6 is max possible tableLog for HUF header (maybe even 5, to be tested) */
        if (iSize+1 > srcSize) return ERROR(srcSize_wrong);
        oSize = FSE_decompress_wksp(huffWeight, hwSize-1, ip+1, iSize, fseWorkspace, 6);   /* max (hwSize-1) values decoded, as last one is implied */
        if (FSE_isError(oSize)) return oSize;
    }

    /* collect weight stats */
    memset(rankStats, 0, (HUF_TABLELOG_MAX + 1) * sizeof(U32));
    weightTotal = 0;
    {   U32 n; for (n=0; n<oSize; n++) {
            if (huffWeight[n] >= HUF_TABLELOG_MAX) return ERROR(corruption_detected);
            rankStats[huffWeight[n]]++;
            weightTotal += (1 << huffWeight[n]) >> 1;
    }   }
    if (weightTotal == 0) return ERROR(corruption_detected);

    /* get last non-null symbol weight (implied, total must be 2^n) */
    {   U32 const tableLog = BIT_highbit32(weightTotal) + 1;
        if (tableLog > HUF_TABLELOG_MAX) return ERROR(corruption_detected);
        *tableLogPtr = tableLog;
        /* determine last weight */
        {   U32 const total = 1 << tableLog;
            U32 const rest = total - weightTotal;
            U32 const verif = 1 << BIT_highbit32(rest);
            U32 const lastWeight = BIT_highbit32(rest) + 1;
            if (verif != rest) return ERROR(corruption_detected);    /* last value must be a clean power of 2 */
            huffWeight[oSize] = (BYTE)lastWeight;
            rankStats[lastWeight]++;
    }   }

    /* check tree construction validity */
    if ((rankStats[1] < 2) || (rankStats[1] & 1)) return ERROR(corruption_detected);   /* by construction : at least 2 elts of rank 1, must be even */

    /* results */
    *nbSymbolsPtr = (U32)(oSize+1);
    return iSize+1;
}
