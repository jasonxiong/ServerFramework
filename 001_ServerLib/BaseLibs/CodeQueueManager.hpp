#ifndef __CODE_QUEUE_MANAGER_HPP__
#define __CODE_QUEUE_MANAGER_HPP__

#include "CodeQueue.hpp"
using namespace ServerLib;

const int MAX_CQNAME_LENGTH = 32;
const int MAX_CODE_QUEUE_NUMBER = 100;


class CCodeQueueManager
{
public:
    //please call it before Initialize
    int LoadCodeQueueConfig(const char* szConfigFile, const char* szModuleName);

    int Initialize(bool bResumeMode);
    int GetCodeQueueNum() const
    {
        return m_iCodeQueueNum;
    }

    int SendOneMsg(const char* pszMsg, int iMsgLength, int iInstanceID = 0);
    int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, int iInstanceID = 0);
    // 读取CodeQueue配置

private:
    int m_iCodeQueueNum;
    // Lotus和Game之间通过CodeQueue通信
    CCodeQueue* m_apLotusCodeQueueIn[MAX_CODE_QUEUE_NUMBER];
    CCodeQueue* m_apLotusCodeQueueOut[MAX_CODE_QUEUE_NUMBER];

    char m_aszLotusToGameCQ[MAX_CODE_QUEUE_NUMBER][MAX_CQNAME_LENGTH];
    char m_aszGameToLotusCQ[MAX_CODE_QUEUE_NUMBER][MAX_CQNAME_LENGTH];

    char m_szCodeQueuePath[255];

};


#endif
