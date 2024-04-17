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

namespace FiniteStateEntropy
{
    public class Error_Public
    {
        public static bool ERR_isError(ulong code)
        {
            return code > FSE_Error(FSE_ErrorCode.FSE_error_maxCode);
        }

        public enum FSE_ErrorCode
        {
            FSE_error_no_error,
            FSE_error_GENERIC,
            FSE_error_dstSize_tooSmall,
            FSE_error_srcSize_wrong,
            FSE_error_corruption_detected,
            FSE_error_tableLog_tooLarge,
            FSE_error_maxSymbolValue_tooLarge,
            FSE_error_maxSymbolValue_tooSmall,
            FSE_error_workSpace_tooSmall,
            FSE_error_maxCode
        };

        public static ulong FSE_Error(FSE_ErrorCode code)
        {
            return ulong.MaxValue - (ulong)code + 1;
        }
    }
}
