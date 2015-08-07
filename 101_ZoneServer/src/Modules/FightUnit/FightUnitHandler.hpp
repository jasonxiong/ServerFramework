#ifndef __FIGHT_UNIT_HANDLER_HPP__
#define __FIGHT_UNIT_HANDLER_HPP__

#include "Handler.hpp"

class CFightUnitHandler : public IHandler
{
public:
	virtual int OnClientMsg();

private:

	void SendHandlerResponse(int iRespMsgID, int code);

	int securityCheck();

private:

	static GameProtocolMsg ms_stZoneMsg;
};

#endif
