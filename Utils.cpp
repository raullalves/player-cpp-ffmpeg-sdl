#include "stdafx.h"

void Utils::display_exception(const char * message)
{
	throw std::exception(message);
}

void Utils::display_ffmpeg_exception(int error_code)
{
	char error_buf[ERROR_SIZE];
	av_strerror(error_code, error_buf, ERROR_SIZE);
	display_exception(error_buf);
}