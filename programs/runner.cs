/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 TheVice
 *
 */

using System;
using System.IO;
using System.Collections.Generic;

namespace FiniteStateEntropy
{
    public class Runner
    {
        static readonly int EXIT_SUCCESS = 0;
        static readonly int EXIT_FAILURE = 1;

        public static int Main(string[] args)
        {
            if (args.Length < 1)
            {
                return EXIT_FAILURE;
            }

            switch (args[0])
            {
                case "FSE_readNCount":
                    {
                        if (args.Length < 7)
                        {
                            return EXIT_FAILURE;
                        }

                        var shorts = new List<short>();
                        var tmp = File.ReadAllBytes(args[1]);

                        for (var index = 0; index < tmp.Length; index += sizeof(short))
                        {
                            shorts.Add(BitConverter.ToInt16(tmp, index));
                        }

                        var normalizedCounter = shorts.ToArray();
                        shorts.Clear();
                        //
                        tmp = File.ReadAllBytes(args[2]);
                        var uints = new List<uint>();

                        for (var index = 0; index < tmp.Length; index += sizeof(uint))
                        {
                            uints.Add(BitConverter.ToUInt32(tmp, index));
                        }

                        var maxSVPtr = uints.ToArray();
                        uints.Clear();
                        //
                        tmp = File.ReadAllBytes(args[3]);
                        var ints = new List<int>();

                        for (var index = 0; index < tmp.Length; index += sizeof(int))
                        {
                            ints.Add(BitConverter.ToInt32(tmp, index));
                        }

                        var tableLogPtr = ints.ToArray();
                        ints.Clear();
                        //
                        var headerBuffer = File.ReadAllBytes(args[4]);
                        //
                        tmp = File.ReadAllBytes(args[5]);

                        if (null == tmp || tmp.Length < sizeof(ulong))
                        {
                            tmp = new byte[sizeof(ulong)];
                        }

                        var hbSize = BitConverter.ToUInt64(tmp, 0);
                        //
                        var returned = Entropy_Common.FSE_readNCount(normalizedCounter, maxSVPtr, tableLogPtr, headerBuffer, hbSize);
                        //
                        File.WriteAllBytes(args[6], BitConverter.GetBytes(returned));
                        var bytes = new List<byte>();

                        foreach (var item in tableLogPtr)
                        {
                            bytes.AddRange(BitConverter.GetBytes(item));
                        }

                        File.WriteAllBytes(args[3], bytes.ToArray());
                        bytes.Clear();

                        foreach (var item in maxSVPtr)
                        {
                            bytes.AddRange(BitConverter.GetBytes(item));
                        }

                        File.WriteAllBytes(args[2], bytes.ToArray());
                        bytes.Clear();

                        foreach (var item in normalizedCounter)
                        {
                            bytes.AddRange(BitConverter.GetBytes(item));
                        }

                        File.WriteAllBytes(args[1], bytes.ToArray());
                        bytes.Clear();
                    }
                    break;
                default:
                    return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }
    }
}
