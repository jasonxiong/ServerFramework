#ifndef __MISCDATA_STORE_HPP__
#define __MISCDATA_STORE_HPP__

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "CommonDef.hpp"

using namespace ServerLib;

//该类对文件操作做了简单封装
//因为每次都是全量写文件，只适合数据比较小的文件,大约4k一下
//4k write 一次大约是4us
//大的存储还是存数据库吧
class CMiscDataStore
{
public:
    CMiscDataStore() {}
    virtual ~CMiscDataStore();

//	static bool IsExsist(const char *szFileName, int iWorldID, int iZoneID);
//	static int *GetFileFP(const char *szFileName, int iWorldID, int iZoneID, FILE *&fp);
//
    // 初始化文件, WorldID和ZoneID是为了避免文件重复
    virtual int Init(const char *szFileName, int iWorldID, int iZoneID);

    //写文件， 一般不用重载，
    //具体写数据时，请重载WritePrivateData
    virtual int WriteData();
private:
    //读文件， 在Init时调用
    virtual int ReadData() = 0;

    //写文件， 写类中需要存储的数据
    virtual int WritePrivateData() = 0;

private:
    //得到文件对应的真实文件路径
    static int GetRealFilePath(char (&szFilePath)[MAX_FILENAME_LENGTH], const char* pFileName, int iWorldID, int iZoneID);

public:
    //文件是否存在
    static bool Exists(const char* pFileName, int iWorldID, int iZoneID);

    //读取文件内容, 返回真实读取的字节数
    static int ReadFile(const char* pFileName, int iWorldID, int iZoneID, char* pData, int iDataLen);

    //写文件内容, 返回真实写入的字节数
    static int WriteFile(const char* pFileName, int iWorldID, int iZoneID, const char* pData, int iDataLen);

protected:
    //正在打开的文件指针
    FILE* m_fp;
    //文件名称
    char m_szDataStoreFileName[MAX_FILENAME_LENGTH];
};

#endif
