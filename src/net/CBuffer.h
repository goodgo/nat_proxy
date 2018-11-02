/*
 * CBuffer.h
 *
 *  Created on: Oct 25, 2018
 *      Author: root
 */

#ifndef SRC_NET_CBUFFER_H_
#define SRC_NET_CBUFFER_H_


//class CBuffer
//{
//public:
//	CBuffer(size_t sz);
//	~CBuffer();
//	bool onReadFd(int fd, int* savedErr);
//
//	int8_t onReadInt8();
//	int16_t onReadInt16();
//	int32_t onReadInt32();
//	bool onReadString(char* res, size_t len);
//
//	bool onWrite(char* ptr, size_t len);
//	const char* onPeek();
//	const char* onPeekInt8(uint8_t);
//
//private:
//	void retrieve(size_t len);
//	char* begin();
//	const char* begin() const;
//	size_t readableBytes();
//	size_t writableBytes();
//	bool append();
//	bool adjust();
//
//private:
//	size_t _readerIndex;
//	size_t _writerIndex;
//	std::vector<char> _buffer;
//};


#endif /* SRC_NET_CBUFFER_H_ */
