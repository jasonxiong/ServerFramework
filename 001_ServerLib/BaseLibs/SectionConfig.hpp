
#ifndef __SECTION_CONFIG_HPP__
#define __SECTION_CONFIG_HPP__

#include <stdint.h>

#include "ErrorDef.hpp"
//#define BYTE unsigned char

namespace ServerLib
{
class CSectionConfig
{
public:
    char    *m_pszFilename;     //!<存放需要读取的配置文件名

    //!构造函数，初始化类的成员，读取配置文件
    CSectionConfig();
    ~CSectionConfig();

    //!返回配置文件读取状态
    unsigned int IsOpen();

    //!取指定的键值
    unsigned int GetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               char *pszReturnedString,
                               unsigned int nSize );

    //!从内存缓冲区中找到KeyName，将值拷贝到指定的空间
    unsigned int SetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               const char *pszKeyValue );
    unsigned int GetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               int &lReturnedValue );
    unsigned int SetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               int lKeyValue );
    unsigned int GetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               int &lReturnedValue,
                               int lDefaultValue );
    unsigned int GetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               short &rshReturnedValue,
                               short shDefaultValue );
    unsigned int GetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               int64_t &lReturnedValue );
    unsigned int GetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               int64_t &lReturnedValue,
                               int64_t lDefaultValue );
    unsigned int GetItemValue( const char *pszSectionName,
                               const char *pszKeyName,
                               char *pszReturnedString,
                               unsigned int nSize,
                               const char *pszDefaultValue );

    //!加载配置文件到内存
    int  OpenFile(const char *pszFilename);

    //!释放配置文件加载到内存后占用的资源
    void CloseFile();

    //!在接口返回错误时，调用这个函数获取错误号
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:

    //!定位section的开始地址和结束地址
    unsigned int LocateSection(const char *pszSectionName,
                               char * &pszSectionBegin,
                               char * &pszSectionEnd);

    //!在指定的缓冲区范围中搜索Key，返回与Key匹配的值的开始地址和结束地址
    unsigned int LocateKeyRange(const char *pszKeyName,
                                const char *pszSectionBegin,
                                const char *pszSectionEnd,
                                char * &pszKeyBegin,
                                char * &pszKeyEnd);

    //!在指定的缓冲区范围中搜索Key，返回与Key匹配的值的开始地址和结束地址
    unsigned int LocateKeyValue(const char *pszKeyName,
                                const char *pszSectionBegin,
                                const char *pszSectionEnd,
                                char * &pszValueBegin,
                                char * &pszValueEnd);

    //!在一个字符串中搜索另一个字符串
    const char *LocateStr(    const char *pszCharSet,
                              const char *pszBegin,
                              const char *pszEnd );

    const char *SearchMarchStr(const char *pszBegin, const char *pszCharSet);

    //!将Shadow中的地址映射到Content中
    char *MapToContent(const char *p);

    //!将Content中的地址映射到Shadow中
    char *MapToShadow(const char *p);

    //!将字符串中的大写字母转换成小写字母
    void ToLower( char * pszSrc, size_t len);

    //!设置错误号
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    char    *m_pszContent;      //!<配置文件的原始内容
    char    *m_pszShadow;       //!<配置文件的内容全部转换成小写
    size_t  m_nSize;            //!<配置文件内容的长度，不包括最后的NULL
    short   m_bIsOpen;         //!<配置文件是否打开成功的标志
    int m_iErrorNO; //!错误码
};
}

#endif //__SECTION_CONFIG_HPP__
