#ifndef __PROTO_DATA_UTILITY_HPP__
#define __PROTO_DATA_UTILITY_HPP__

//用于对DB中存储的protobuf数据的处理

#include <string>

#include "lz4.hpp"
#include "google/protobuf/message.h"

//压缩处理步骤为：
//      1.先将protobuf结构进行序列化；
//      2.将序列化后的二进制数据通过lz4进行压缩;

//解压处理步骤为：
//      1.先通过lz4解压，获取二进制数据；
//      2.将二进制数据反序列化到protobuf结构中

namespace ServerLib
{

#define LZ4_COMPRESS_BUFF_LEN 50*1024     //压缩数据的缓冲区大小为50K

extern inline bool CompressData(const std::string& strSource, std::string& strDest)
{
    if(LZ4_compressBound(strSource.size()) > LZ4_COMPRESS_BUFF_LEN)
	{
		return false;
	}

    //压缩数据
    static char szCompressedBuff[LZ4_COMPRESS_BUFF_LEN];
    int iCompressedLen = LZ4_compress(strSource.c_str(), szCompressedBuff, strSource.size());
    if(iCompressedLen == 0)
    {
        return false;
    }

    strDest.assign(szCompressedBuff, iCompressedLen);

    return true;
}

extern inline bool UnCompressData(const std::string& strSource, std::string& strDest)
{
    //解压缩数据
    static char szUnCompressedBuff[LZ4_COMPRESS_BUFF_LEN];
    int iUnCompressedLen = LZ4_decompress_safe(strSource.c_str(), szUnCompressedBuff, strSource.size(), sizeof(szUnCompressedBuff)-1);
    if(iUnCompressedLen < 0)
    {
        return false;
    }

    strDest.assign(szUnCompressedBuff, iUnCompressedLen);

    return true;
}

//序列化protobuf数据并压缩
template <class ProtoType>
bool EncodeProtoData(const ProtoType& stProtoMsg, std::string& strCompressedData)
{		
    //先序列化数据
    std::string strSerialized;
    if(!stProtoMsg.SerializeToString(&strSerialized))
    {
        return false;
    }

	return CompressData(strSerialized, strCompressedData);
};

//解压数据并反序列化到protobuf中
template <class ProtoType>
bool DecodeProtoData(const std::string& strCompressData, ProtoType& stProtoMsg)
{
    std::string strUnCompressData;
    if(!UnCompressData(strCompressData, strUnCompressData))
    {
        return false;
    }

    //反序列化protobuf
    stProtoMsg.Clear();
    if(!stProtoMsg.ParseFromString(strUnCompressData))
    {
        return false;
    }

    return true;
};

}

#endif

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
