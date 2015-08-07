
#ifndef __HANDLER_FACTORY_HPP__
#define __HANDLER_FACTORY_HPP__

#include "GameConfigDefine.hpp"

using namespace GameConfig;

class IHandler;

class CHandlerFactory
{
public:
    CHandlerFactory();

public:
    static IHandler* GetHandler(const unsigned int uiMsgID);
    static int RegisterHandler(const unsigned int uiMsgID, const IHandler* pHandler);

protected:
    static IHandler* m_apHandler[MAX_HANDLER_NUMBER];
};

#endif
