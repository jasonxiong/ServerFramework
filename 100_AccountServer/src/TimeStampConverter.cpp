#include "LogAdapter.hpp"
#include "TimeStampConverter.hpp"

using namespace ServerLib;

unsigned int GenerateTimeStamp(const unsigned int uiSessionFD, const unsigned short unValue)
{
 //   if (uiSessionFD > 0xffff)
 //   {
 //       TRACESVR("Session fd too big: %u", uiSessionFD);
 //   }

 //   return ((uiSessionFD << 16) + unValue);
	return uiSessionFD;
}

unsigned int TimeStampToSessionFD(const unsigned int uiTimeStamp)
{
	return uiTimeStamp;
    //return ((uiTimeStamp & 0xffff0000) >> 16);
}

unsigned short TimeStampToValue(const unsigned int uiTimeStamp)
{
    return 0;
    //return (unsigned short)(uiTimeStamp & 0xffff);
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
