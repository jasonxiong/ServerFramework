#ifndef __WORLD_GLOBAL_DEF_HPP__
#define __WORLD_GLOBAL_DEF_HPP__

#include "GameProtocol.hpp"
#include "WorldErrorNumDef.hpp"

typedef enum enWorldResultIDType
{
    RES_Success = 0,
    RES_Fail    = 1,

}EWorldResultIDType;

const int CODEQUEUE_SHM_SIZE = 16777216;

const int MAX_PATH_LEN=1024;

const int MAX_MSGBUFFER_SIZE = 2048000;

#endif
