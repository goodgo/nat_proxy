///*
// * CBuffer.cpp
// *
// *  Created on: Oct 25, 2018
// *      Author: root
// */
//
//#include "CBuffer.h"
//#include "CLog.h"
//#include "CInclude.h"
//#include <endian.h>
//
//CBuffer::CBuffer(size_t sz)
//: _buffer(sz)
//, _readerIndex(0)
//, _writerIndex(0)
//{
//
//}
//CBuffer::~CBuffer()
//{
//
//}
//
//
//bool CBuffer::onReadFd(int fd, int* savedErr)
//{
//	char extrabuf[65536];
//	struct iovec vec[2];
//	const size_t writable = writableBytes();
//	vec[0].iov_base = begin()+ _writerIndex;
//	vec[0].iov_len = writable;
//	vec[1].iov_base = extrabuf;
//	vec[1].iov_len = sizeof extrabuf;
//	const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
//	const ssize_t n = readv(fd, vec, iovcnt);
//	if (n < 0)
//	{
//		*savedErr = errno;
//	}
//	else if (n <= writable)
//	{
//		_writerIndex += n;
//	}
//	else
//	{
//		_writerIndex = _buffer.size();
//		//append(extrabuf, n - writable);
//	}
//	return n;
//}
//
//int8_t CBuffer::onReadInt8()
//{
//	int8_t res = *onPeek();
//	retrieve(sizeof(int8_t));
//	return res;
//}
//
//int16_t CBuffer::onReadInt16()
//{
//	int16_t res;
//	memcpy(&res, onPeek(), sizeof(int16_t));
//	res = be16toh(res);
//	retrieve(sizeof(int16_t));
//	return res;
//}
//
//int32_t CBuffer::onReadInt32()
//{
//	int32_t res;
//	memcpy(&res, onPeek(), sizeof(int32_t));
//	res = be32toh(res);
//	retrieve(sizeof(int32_t));
//	return res;
//}
//
//bool CBuffer::onReadString(char* res, size_t len)
//{
//	memset(res, 0, len);
//	if (readableBytes() >= len)
//	{
//		memcpy(res, onPeek(), len);
//		retrieve(len);
//		return true;
//	}
//	return false;
//}
//
//bool CBuffer::onWrite(char* ptr, size_t len)
//{
//	if (writableBytes() < len)
//	{
//		DEBUGINFO("buffer adjust..");
//		adjust();
//	}
//
//
//
//	return true;
//}
//
//void CBuffer::retrieve(size_t len)
//{
//	if (len < readableBytes())
//	{
//		_readerIndex += len;
//	}
//	else
//	{
//		_readerIndex = 0;
//		_writerIndex = 0;
//	}
//}
//
//const char* CBuffer::onPeek()
//{
//	return (begin() + _readerIndex);
//}
//
//
//char* CBuffer::begin()
//{
//	return &*_buffer.begin();
//}
//
//const char* CBuffer::begin() const
//{
//	return &*_buffer.begin();
//}
//
//size_t CBuffer::readableBytes()
//{
//	return (_writerIndex - _readerIndex);
//}
//
//size_t CBuffer::writableBytes()
//{
//	return (_buffer.size() - writableBytes);
//}
//
//bool CBuffer::append()
//{
//	return true;
//}
//
//bool CBuffer::adjust()
//{
//	return true;
//}
//
//
//
