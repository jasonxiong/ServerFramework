#include "HandlerSet.hpp"

CHandlerSet::CHandlerSet()
{
    memset(m_apHandler, 0, sizeof(m_apHandler));
}

CHandler* CHandlerSet::GetHandler(const unsigned int uiMsgID)
{
    if (uiMsgID >= MAX_HANDLER_NUMBER)
    {
        return NULL;
    }

    return m_apHandler[uiMsgID];
}

int CHandlerSet::RegisterHandler(const unsigned int uiMsgID, CHandler* pHandler)
{
    if (uiMsgID >= MAX_HANDLER_NUMBER)
    {
        return -1;
    }

    m_apHandler[uiMsgID] = pHandler;
    return 0;
}

int CHandlerSet::Initialize()
{
    return 0; // »± ° µœ÷
}

