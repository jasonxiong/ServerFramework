
#ifndef __HANDLER_FACTORY_HPP__
#define __HANDLER_FACTORY_HPP__

#include "GameConfigDefine.hpp"
#include "Handler.hpp"

class IHandler;

using namespace std;
using namespace GameConfig;

typedef enum tagenZoneMsgType
{
	EKMT_CLIENT = 1,    // 客户端消息
	EKMT_SERVER = 2,    // 服务器消息

}EZoneMsgType;

typedef enum tagEnumZoneMsgFlag
{
	// 不检查
	EKMF_CHECKNONE		= 0,

	// 如果不涉及复活和自动操作, 则必须检查死亡状态
	EKMF_CHECKDEAD		= 1,	

	// 如果涉及到任何物品, 则必须检查背包状态
	EKMF_CHECKBAGLOCK	= 2,	

	// 检查全部
	EKMF_CHECKALL		= EKMF_CHECKDEAD | EKMF_CHECKBAGLOCK,

}EZoneMsgFlag;

typedef struct tagZoneMsgConfig
{
	EZoneMsgType m_enMsgType;
	unsigned int m_uiMsgFlag;
	int m_iMsgInterval;
	bool m_bDisabled;
}TZoneMsgConfig;

class CHandlerFactory
{
public:
    // 获取消息处理函数
	static IHandler* GetHandler(const unsigned int uiMsgID, EZoneMsgType enMsgType = EKMT_SERVER);

	// 获取消息配置信息
	static TZoneMsgConfig* GetMsgConfig(const unsigned int uiMsgID);

    // 注册消息处理函数
	static int RegisterHandler(const unsigned int uiMsgID, const IHandler* pHandler, 
		EZoneMsgType enMsgType = EKMT_SERVER, 
		unsigned int uiHandlerFlag = EKMF_CHECKALL
		);

	static void DisabledMsg(const unsigned int uiMsgID);
	static bool IsMsgDisabled(const unsigned int uiMsgID);
	static void ClearAllDisableMsg();
protected:
	static IHandler* m_apHandler[MAX_HANDLER_NUMBER];
	static TZoneMsgConfig m_astMsgConfig[MAX_HANDLER_NUMBER];
};

#endif
