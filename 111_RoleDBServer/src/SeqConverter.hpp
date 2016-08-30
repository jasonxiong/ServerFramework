#ifndef __SEQ_CONVERTER_HPP__
#define __SEQ_CONVERTER_HPP__

#define DAY_BASE 100000
#define WORLD_BASE 10

// 生成一个DBSeq，DBSeq = 100000 * unCreatedTime + 10 * nWorldID + nCacheSeq
unsigned int GenerateDBSeq(const unsigned short unCreatedTime,
                           const short nWorldID, const short nCacheSeq);
// 返回DBSeq中的CacheSeq的值
short DBSeqToCacheSeq(const unsigned int uiDBSeq);
// 返回DBSeq中的WorldID的值
short DBSeqToWorldID(const unsigned int uiDBSeq);
// 返回DBSeq中的CreatedTime的值，它表示自2011-1-1起的天数
unsigned short DBSeqToCreatedTime(const unsigned int uiDBSeq);
// 检查DBSeq的三个组成部分是否都在合法范围内
int CheckDBSeq(const unsigned int uiDBSeq);

#endif // __SEQ_CONVERTER_HPP__

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
