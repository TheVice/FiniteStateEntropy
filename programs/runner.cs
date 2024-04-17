/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 TheVice
 *
 */

using System;

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

            return EXIT_SUCCESS;
        }
    }
}
