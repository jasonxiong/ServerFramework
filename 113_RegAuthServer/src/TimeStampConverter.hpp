#ifndef __TIME_STAMP_CONVERTER_HPP__
#define __TIME_STAMP_CONVERTER_HPP__

// TimeStamp的高16个bit存放session fd，低16个bit存放辅助的value
unsigned int GenerateTimeStamp(const unsigned int uiSessionFD, const unsigned short unValue);
// 从TimeStamp中解析出session fd
unsigned int TimeStampToSessionFD(const unsigned int uiTimeStamp);
// 从TimeStamp中解析出辅助value
unsigned short TimeStampToValue(const unsigned int uiTimeStamp);

#endif // __TIME_STAMP_CONVERTER_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
