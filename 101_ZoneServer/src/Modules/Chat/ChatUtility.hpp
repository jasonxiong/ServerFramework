#ifndef __CHAT_UTILITY_HPP__
#define __CHAT_UTILITY_HPP__

class CGameRoleObj;
class CChatUtility
{
public:
    static int SendChatMsg(CGameRoleObj& stRoleObj, int iChannel, const char* pMsg);
};

#endif
