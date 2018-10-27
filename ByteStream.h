#pragma once

struct IByteStream
{
	virtual void write(unsigned char b) = 0;
	virtual void write(const char* str) = 0;
	virtual unsigned char read() = 0;
};
