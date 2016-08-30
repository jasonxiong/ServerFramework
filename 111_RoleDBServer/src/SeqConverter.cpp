#include "UnixTime.hpp"
#include "NowTime.hpp"
#include "Public.hpp"
#include "SeqConverter.hpp"

using namespace ServerLib;

unsigned int GenerateDBSeq(const unsigned short unCreatedTime, const short nWorldID,
                           const short nCacheSeq)
{
    unsigned int uiDBSeq = DAY_BASE * unCreatedTime + WORLD_BASE * nWorldID + nCacheSeq;

    return uiDBSeq;
}

short DBSeqToCacheSeq(const unsigned int uiDBSeq)
{
    short nWorldIDPlusCacheSeq = uiDBSeq % DAY_BASE;
    short nCacheSeq = nWorldIDPlusCacheSeq % WORLD_BASE;

    return nCacheSeq;
}

short DBSeqToWorldID(const unsigned int uiDBSeq)
{
    short nWorldIDPlusCacheSeq = uiDBSeq % DAY_BASE;
    short nWorldID = nWorldIDPlusCacheSeq / WORLD_BASE;

    return nWorldID;
}

unsigned short DBSeqToCreatedTime(const unsigned int uiDBSeq)
{
    unsigned short unCreatedTime = uiDBSeq / DAY_BASE;

    return unCreatedTime;
}

int CheckDBSeq(const unsigned int uiDBSeq)
{
    short nCacheSeq = DBSeqToCacheSeq(uiDBSeq);
    if ((nCacheSeq < 0) || (nCacheSeq > 1))
    {
        return -1;
    }

    short nWorldID = DBSeqToWorldID(uiDBSeq);
    if ((nWorldID < 0) || (nWorldID > 1))
    {
        return -1;
    }

    //todo jasonxiong 暂时不需要，等后面再确认是否需要修改
    /* 
    CUnixTime now(time(NULL));
    unsigned short unNow;
    now.GetDaysAfterYear(START_YEAR, unNow);

    unsigned short unCreatedTime = DBSeqToCreatedTime(uiDBSeq);
    if (unCreatedTime > unNow)
    {
        return -1;
    }
    */

    return 0;
}

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
