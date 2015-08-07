
#ifndef __CONFIGFILE_HPP__
#define __CONFIGFILE_HPP__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


class ConfigFile
{
private:
    char    *m_pszContent;        // 配置文件的原始内容
    char    *m_pszShadow;         // 配置文件的内容全部转换成小写
    size_t  m_nSize;              // 配置文件内容的长度，不包括最后的NULL

    unsigned int   m_bIsOpened;   // 配置文件是否打开成功的标志

public:
    char    *m_pszFilename;       // 存放需要读取的配置文件名

    ConfigFile();
    ~ConfigFile();

    int  OpenFile(const char* pszFilename);

    void CloseFile();

    unsigned int IsOpened();

    unsigned int GetItemValue( const char* pszSectionName,
                               const char* pszKeyName,
                               char* pszReturnedString,              //char*
                               unsigned int nSize );
    unsigned int GetItemValue( const char* pszSectionName,
                               const char* pszKeyName,
                               char* pszReturnedString,              //char* +default
                               unsigned int nSize,
                               const char* pszDefaultValue );
    unsigned int GetItemValue( const char* pszSectionName,
                               const char* pszKeyName,
                               int &lReturnedValue );                //int&
    unsigned int GetItemValue( const char* pszSectionName,
                               const char* pszKeyName,
                               int &lReturnedValue,                  //int& +default
                               int lDefaultValue );
    unsigned int GetItemValue( const char* pszSectionName,
                               const char* pszKeyName,
                               int64_t &lReturnedValue );            //int64_t&
    unsigned int GetItemValue( const char* pszSectionName,
                               const char* pszKeyName,
                               int64_t &lReturnedValue,              //int64_t& +default
                               int64_t lDefaultValue );
    unsigned int SetItemValue( const char* pszSectionName,
                               const char* pszKeyName,
                               int lKeyValue );                     //set int
    unsigned int SetItemValue( const char* pszSectionName,
                               const char* pszKeyName,
                               const char* pszKeyValue );           //set char*


private:
    unsigned int LocateSection( const char* pszSectionName,
                                char*  &pszSectionBegin,
                                char*  &pszSectionEnd );
    unsigned int LocateKey(     const char* pszKeyName,
                                const char* pszSectionBegin,
                                const char* pszSectionEnd,
                                char*  &pszKeyBegin,
                                char*  &pszKeyEnd );
    unsigned int LocateValue(   const char* pszKeyName,
                                const char* pszSectionBegin,
                                const char* pszSectionEnd,
                                char*  &pszValueBegin,
                                char*  &pszValueEnd );

    char* LocateStr(const char* pszStr, const char* pszBegin, const char* pszEnd);
    char* FindStr(const char* pszBegin, const char* pszStr);

    char* MapToContent(const char* p);
    char* MapToShadow(const char* p);

    void MyToLower(char*  pszSrc, size_t len);
};


#endif
