#ifndef __I_HANDLER_HPP__
#define __I_HANDLER_HPP__

class IHandler
{
public:
	virtual ~IHandler();
	virtual int OnClientMsg(const void* pMsg) = 0;

};



#endif


