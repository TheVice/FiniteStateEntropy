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
    public class Mem
    {
        public static bool MEM_isLittleEndian()
        {
            return BitConverter.IsLittleEndian;
	}

        public static uint MEM_read32(byte[] memPtr)
        {
            var val = BitConverter.ToUInt32(memPtr, 0);
            return val;
        }

        public static uint MEM_swap32(uint inValue)
        {
            return
              ((inValue << 24) & 0xff000000) |
              ((inValue << 8) & 0x00ff0000) |
              ((inValue >> 8) & 0x0000ff00) |
              ((inValue >> 24) & 0x000000ff);
        }

        public static UInt32 MEM_readLE32(byte[] memPtr)
        {
            if (MEM_isLittleEndian())
            {
                return MEM_read32(memPtr);
            }
            
            return MEM_swap32(MEM_read32(memPtr));
        }

        public static UInt32 MEM_readLE32(byte[] memPtr, int index)
        {
            byte i = 0;
            var buffer = new byte[4];
            Array.Clear(buffer, 0, buffer.Length);

            while (index < memPtr.Length && i < buffer.Length)
            {
                buffer[i++] = memPtr[index++];
            }

            return MEM_readLE32(buffer);
        }
    }
}
