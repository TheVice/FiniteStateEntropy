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

namespace FiniteStateEntropy
{
    public class Fse
    {
        public static int FSE_DTABLE_SIZE_U32(int maxTableLog)
        {
            return 1 + (1 << (maxTableLog));
        }

        public static readonly byte FSE_VERSION_MAJOR = 0;
        public static readonly byte FSE_VERSION_MINOR = 9;
        public static readonly byte FSE_VERSION_RELEASE = 0;
        public static readonly uint FSE_VERSION_NUMBER = (uint)(FSE_VERSION_MAJOR * 100 * 100 + FSE_VERSION_MINOR * 100 + FSE_VERSION_RELEASE);

        public static readonly byte FSE_MAX_MEMORY_USAGE = 14;
        public static readonly byte FSE_DEFAULT_MEMORY_USAGE = 13;
        public static readonly byte FSE_MAX_SYMBOL_VALUE = byte.MaxValue;
        public static readonly byte FSE_MAX_TABLELOG = (byte)(FSE_MAX_MEMORY_USAGE - 2);
        public static readonly ushort FSE_MAX_TABLESIZE = (ushort)(1U << FSE_MAX_TABLELOG);
        public static readonly ushort FSE_MAXTABLESIZE_MASK = (ushort)(FSE_MAX_TABLESIZE - 1);
        public static readonly byte FSE_DEFAULT_TABLELOG = (byte)(FSE_DEFAULT_MEMORY_USAGE - 2);
        public static readonly byte FSE_MIN_TABLELOG = 5;
        public static readonly byte FSE_TABLELOG_ABSOLUTE_MAX = 15;

        public static UInt64 FSE_TABLESTEP(UInt64 tableSize)
        {
            return ((tableSize) >> 1) + ((tableSize) >> 3) + 3;
        }
    }
}
