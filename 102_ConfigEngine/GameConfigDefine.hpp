#ifndef __GAME_PROTOCOL_DEF_HPP__
#define __GAME_PROTOCOL_DEF_HPP__

#include "TdrKeywords.h"
#include "S_GameConfig.h"

//战斗单位路径上最多可以有30个点
const int MAX_POSITION_NUMBER_IN_PATH = 30;

//战场上单位的位置坐标信息
struct TUNITPOSITION
{
    int iPosX;          //战场上单位的X坐标
    int iPosY;          //战场上单位的Y坐标

    TUNITPOSITION()
    {
        memset(this, 0, sizeof(*this));
    };

    TUNITPOSITION(int iX, int iY)
    {
        iPosX = iX;
        iPosY = iY;
    };

    TUNITPOSITION(const TUNITPOSITION& stPos)
    {
        iPosX = stPos.iPosX;
        iPosY = stPos.iPosY;
    };
};

//战场上单位的路径信息
struct TUNITPATH
{
    int iPosNum;                                        //路径上的点的数目
    TUNITPOSITION astPos[MAX_POSITION_NUMBER_IN_PATH];  //单位路径上的点信息

    TUNITPATH()
    {
        memset(this, 0, sizeof(*this));
    };
};

#endif

