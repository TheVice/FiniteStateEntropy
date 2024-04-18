/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 TheVice
 *
 */

#include "misc.h"

#include "buffer.h"
#include "common.h"
#include "file_system.h"
#include "path.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

uint8_t OpenTmpFile(void* tmp_file_name, void** file)
{
	ptrdiff_t size = buffer_size(tmp_file_name);

	if (!path_get_temp_file_name(tmp_file_name))
	{
		fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	if (!file_open(buffer_uint8_t_data(tmp_file_name, size), (const uint8_t*)"wb", file))
	{
		fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	return 1;
}

uint8_t GetPathToTheRunner(void* path)
{
	uint8_t current_dir[BUFFER_SIZE_OF];

	if (!buffer_init(current_dir, BUFFER_SIZE_OF))
	{
		fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	if (!path_get_directory_for_current_process(current_dir))
	{
		buffer_release(current_dir);
		fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	const uint8_t* path_start = buffer_uint8_t_data(current_dir, 0);
	const uint8_t* path_finish = path_start + buffer_size(current_dir);

	if (!path_get_directory_name(path_start, &path_finish))
	{
		buffer_release(current_dir);
		fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	const uint8_t* sub_path_start = (const uint8_t*)"/Visual/CSharp/Program/bin/Release/net8.0/Program.dll";
	const uint8_t* sub_path_finish = sub_path_start + common_count_bytes_until(sub_path_start, 0);

	if (!path_combine(path_start, path_finish, sub_path_start, sub_path_finish, path))
	{
		buffer_release(current_dir);
		fprintf(stderr, "FAILED at: %s %s %i\n", __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}

	buffer_release(current_dir);
	return 1;
}
