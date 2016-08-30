/**
*@file ErrorDef.hpp
*@author jasonxiong
*@date 2009-11-11
*@version 1.0
*@brief 错误码定义文件
*
*	包含所有模块错误码的定义，前两个字节表示模块代号
*/

#ifndef __ERROR_DEF_HPP__
#define __ERROR_DEF_HPP__

#include <string.h>
#include <assert.h>

namespace ServerLib
{

/**
*@brief 错误码定义
*
*	前两个字节表示模块代号，如0x0001XXXX表示日志模块的对应错误，具体错误码最小从0010开始
*/
typedef enum enmErrorNO
{
    EEN_LOGFILE = 0x00010000, //!<日志模块
    EEN_LOGFILE__NULL_POINTER = 0x00010010, //!<指针为空
    EEN_LOGFILE__OPEN_FILE_FAILED = 0x00010011, //!<打开文件失败
    EEN_LOGFILE__GET_FILE_STAT_FAILED = 0x00010012, //!<获取文件统计信息失败，调用stat
    EEN_LOGFILE__REMOVE_FILE_FAILED = 0x00010013, //!<删除文件失败
    EEN_LOGFILE__RENAME_FILE_FAILED = 0x00010014, //!<文件重命名失败
    //...添加日志模块其他错误码

    EEN_SECTION_CONFIG = 0x00020000, //!<按段读配置模块
    EEN_SECTION_CONFIG__NULL_POINTER = 0x00020010, //!<指针为空
    EEN_SECTION_CONFIG__OPEN_FILE_FAILED = 0x00020011, //!<open文件失败
    EEN_SECTION_CONFIG__FSEEK_FAILED = 0x00020012, //!<调用fseek失败
    //...添加按段读配置模块其他错误码

    EEN_STRING_SPLITTER = 0x00030000, //!<字符串分离器模块
    EEN_STRING_SPLITTER__NULL_POINTER = 0x00030010, //!<指针为空
    EEN_STRING_SPLITTER__BAD_TOKEN_BEG = 0x00030011, //!<Token开始指针不合法
    EEN_STRING_SPLITTER__BAD_SEPARTOR = 0x00030012, //!<Token分隔符不合法
    EEN_STRING_SPLITTER__REACH_END = 0x00030013, //!<字符串搜索到末尾了
    EEN_STRING_SPLITTER__STRSTR_FAILED = 0x00040014, //!<调用strstr失败
    //...添加字符串分离器模块其他错误码

    EEN_LINE_CONFIG = 0x00040000, //!<按行读配置模块
    EEN_LINE_CONFIG__NULL_POINTER = 0x00040010, //!<指针为空
    EEN_LINE_CONFIG__OPEN_FILE_FAILED = 0x00040011, //!<open文件失败
    EEN_LINE_CONFIG__REACH_EOF = 0x00040012, //!<文件读取到末尾
    //...添加按行读取配置类其他错误码

    EEN_TCP_CONNECTOR = 0x00050000, //!<TCP连接类
    EEN_TCP_CONNECTOR__NULL_POINTER = 0x00050010, //!<指针为空
    EEN_TCP_CONNECTOR__INVALID_FD = 0x00050011, //!<非法的FD
    EEN_TCP_CONNECTOR__SOCKET_FAILED = 0x00050012, //!<创建Socket失败
    EEN_TCP_CONNECTOR__BIND_SOCKET_FAILED = 0x00050013, //!<绑定IP地址失败
    EEN_TCP_CONNECTOR__CONNECT_FAILED = 0x00050014, //!<连接失败
    EEN_TCP_CONNECTOR__CONFIG_USR_BUF_FAILED = 0x00050015, //!<用户缓冲区配置失败
    EEN_TCP_CONNECTOR__USR_RECV_BUF_FULL = 0x00050016, //!<用户缓冲区满
    EEN_TCP_CONNECTOR__REMOTE_DISCONNECT = 0x00050017, //!<远端关闭连接
    EEN_TCP_CONNECTOR__RECV_ERROR = 0x00050018, //!<调用recv失败，非EAGAIN
    EEN_TCP_CONNECTOR__SEND_ERROR = 0x00050019, //!<调用send失败，非EAGAIN
    EEN_TCP_CONNECTOR__SYS_SEND_BUF_FULL = 0x0005001A, //!<系统发送缓冲区满
    EEN_TCP_CONNECTOR__USR_SEND_BUF_FULL = 0x0005001B, //!<用户发送缓冲区满
    EEN_TCP_CONNECTOR__NB_CONNECT_TIMEOUT = 0x0005001C, //!<非阻塞连接超时
    EEN_TCP_CONNECTOR__NB_CONNECT_FAILED = 0x0005001D, //!<非阻塞连接失败
    EEN_TCP_CONNECTOR__NB_CONNECT_REFUSED = 0x0005001E, //!<非阻塞连接被拒绝
    EEN_TCP_CONNECTOR__ALLOC_USR_BUF_FAILED = 0x0005001F, //!<用户缓冲区分配内存失败
    EEN_TCP_CONNECTOR__BAD_USR_BUF_ALLOC_TYPE = 0x00050020, //!<错误的用户缓冲区分配方式
    //...添加TCP连接类其他错误码

    EEN_TCP_LISTENER = 0x00060000, //!<TCP监听类
    EEN_TCP_LISTENER__NULL_POINTER = 0x00060010, //!<指针为空
    EEN_TCP_LISTENER__INVALID_FD = 0x00060011, //!<非法的FD
    EEN_TCP_LISTENER__SOCKET_FAILED = 0x00060012, //!<创建Socket失败
    EEN_TCP_LISTENER__BIND_SOCKET_FAILED = 0x00060013, //!<绑定IP地址失败
    EEN_TCP_LISTENER__REUSE_ADDR_FAILED = 0x00060014, //!<设置地址复用失败
    EEN_TCP_LISTENER__LISTEN_FAILED = 0x00060015, //!<监听失败
    EEN_TCP_LISTENER__ACCEPT_FAILED = 0x00060016, //!<接受连接失败
    //...添加TCP监听类其他错误码

    EEN_STATISTIC = 0x00070000, //!<基本统计类
    EEN_STATISTIC__NULL_POINTER = 0x00070010, //!<指针为空
    EEN_STATISTIC__INVALID_SECTION_NUMBER = 0x00070011, //!<非法的统计段个数
    EEN_STATISTIC__NEW_SECTION_FAILED = 0x00070012, //!<动态分配Section失败
    EEN_STATISTIC__SECTION_NUMBER_REACH_UPPER_LIMIT = 0x00070013, //!<Section个数达到上限
    EEN_STATISTIC__INVALID_SECTION_INDEX = 0x00070014, //!<非法的段索引
    EEN_STATISTIC__LOG_FILE_INIT_FAILED = 0x00070015, //!<统计打印配置初始化失败
    EEN_STATISTIC__ITEM_NUMBER_REACH_UPPER_LIMIT = 0x00070016, //!<统计项个数达到上限
    EEN_STATISTIC__INVALID_ITEM_INDEX = 0x00070017, //!<非法的项索引
    //...添加基本统计类其他错误码

    EEN_MSG_STATISTIC = 0x00080000, //!<消息统计类
    EEN_MSG_STATISTIC__NULL_POINTER = 0x00080010, //!<指针为空
    EEN_MSG_STATISTIC__STATISTIC_INIT_FAILED = 0x00080011, //!<消息统计类初始化CStatistic失败
    EEN_MSG_STATISTIC__MSG_ID_REACH_UPPER_LIMIT = 0x00080012, //!<统计消息个数达到上限
    EEN_MSG_STATISTIC__ADD_SECTION_FAILED = 0x00080013, //!<增加消息统计段失败
    EEN_MSG_STATISTIC__GET_SECTION_FAILED = 0x00080014, //!<获取消息统计段指针失败
    //...添加消息统计类其他错误码

    EEN_CACHE_STATISTIC = 0x00090000, //!<Cache统计类
    EEN_CACHE_STATISTIC__NULL_POINTER = 0x00090010, //!<指针为空
    EEN_CACHE_STATISTIC__STATISTIC_INIT_FAILED = 0x00090011, //!<Cache统计类初始化CStatistic失败
    EEN_CACHE_STATISTIC__CACHE_TYPE_REACH_UPPER_LIMIT = 0x00090012, //!<统计消息个数达到上限
    EEN_CACHE_STATISTIC__ADD_SECTION_FAILED = 0x00090013, //!<增加Cache统计段失败
    EEN_CACHE_STATISTIC__GET_SECTION_FAILED = 0x00090014, //!<获取Cache统计段指针失败
    //...添加Cache统计类其他错误码

    EEN_CODEQUEUE = 0x000A0000, //!<编码队列模块
    EEN_CODEQUEUE__NULL_POINTER = 0x000A0010, //!<指针为空
    EEN_CODEQUEUE__QUEUE_SIZE_TOO_LOW = 0x000A0011, //!<队列缓存区长度太小
    EEN_CODEQUEUE__QUEUE_IS_FULL = 0x000A0012, //!<队列已满
    EEN_CODEQUEUE__DATA_OFFSET_INVALID = 0x000A0013, //!<数据偏移不合法
    EEN_CODEQUEUE__MID_OFFSET_INVALID = 0x000A0014, //!<中段偏移不合法
    EEN_CODEQUEUE__FREE_LENGTH_INVALID = 0x000A0015, //!<空闲长度不合法，内存写乱了？
    EEN_CODEQUEUE__USED_LENGTH_INVALID = 0x000A0016, //!<已用长度不合法，内存写乱了？
    EEN_CODEQUEUE__CODE_LENGTH_INVALID = 0x000A0017, //!<代码长度不合法
    EEN_CODEQUEUE__OUT_BUFFER_TOO_SMALL = 0x000A0018, //!<输出缓冲区太小
    //...添加CodeQueue其他错误码

    EEN_TEMPMEMORYMNG = 0x000B0000, //!<临时内存管理类
    EEN_TEMPMEMORYMNG__NULL_POINTER = 0x000B0010, //!<指针为空
    EEN_TEMPMEMORYMNG__CONFIG_OJB_FAILED = 0x000B0011, //!<配置对象信息失败
    EEN_TEMPMEMORYMNG__INVALID_MEM_SIZE = 0x000B0012, //!<非法的内存大小
    EEN_TEMPMEMORYMNG__ALLOC_MEM_FAILED = 0x000B0013, //!<分配内存失败
    EEN_TEMPMEMORYMNG__BAD_MEM_ALLOC_TYPE = 0x000B0014, //!<错误的内存分配方式
    EEN_TEMPMEMORYMNG__OBJ_MEMORY_USED_UP = 0x000B0015, //!<内存对象耗尽
    EEN_TEMPMEMORYMNG__INVALID_OBJ_IDX = 0x000B0016, //!<非法的内存对象索引
    //...添加CTempMemoryMng其他错误码

    EEN_ENCODER = 0x000C0000, //!<编码类
    EEN_ENCODER__NULL_POINTER = 0x000C0010, //!<指针为空
    EEN_ENCODER__EXCEED_CODE_BUF_SIZE = 0x000C0010, //!<超过码流长度
    //...添加编码类其他错误码

    EEN_DECODER = 0x000D0000, //!<解码类
    EEN_DECODER__NULL_POINTER = 0x000D0010, //!<指针为空
    EEN_DECODER__EXCEED_CODE_BUF_SIZE = 0x000D0010, //!<超过码流长度
    //...添加解码类其他错误码

    EEN_HASH_MAP = 0x000E0000, //!<HashMap
    EEN_HASH_MAP__NULL_POINTER = 0x000E0010, //!<指针为空
    EEN_HASH_MAP__INVALID_INDEX = 0x000E0011, //!<无效的索引值
    EEN_HASH_MAP__INSERT_FAILED_FOR_KEY_DUPLICATE = 0x000E0012, //!<无效的索引值
    EEN_HASH_MAP__NODE_IS_FULL = 0x000E0013, //!<Map节点已用完
    EEN_HASH_MAP__NODE_NOT_EXISTED = 0x000E0014, //!<Map节点不存在
    //...添加HashMap类其他错误码

    EEN_OBJ_ALLOCATOR = 0x000F0000, //!<CObjAllocator
    EEN_OBJ_ALLOCATOR__NULL_POINTER = 0x000F0010, //!<指针为空
    EEN_OBJ_ALLOCATOR__INVALID_OBJ_COUNT = 0x000F0011, //!<无效的对象个数
    EEN_OBJ_ALLOCATOR__OBJ_IS_FULL = 0x000F0012, //!<对象已用完
    EEN_OBJ_ALLOCATOR__INVALID_OBJ_INDEX = 0x000F0013, //!<无效的对象索引
    EEN_OBJ_ALLOCATOR__DESTROY_FREE_OBJ = 0x000F0014, //!<销毁空闲对象
    EEN_OBJ_ALLOCATOR__GET_FREE_OBJ = 0x000F0015, //!<直接返回空闲对象
    //...添加ObjAllocator其他错误码

    EEN_BI_DIR_QUEUE = 0x00100000, //!<CBiDirQueue
    EEN_BI_DIR_QUEUE__NULL_POINTER = 0x00100010, //!<指针为空
    EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX = 0x00100011, //!<无效的Item索引
    EEN_BI_DIR_QUEUE__ITEM_ALREADY_IN_QUEUE = 0x00100012, //!<Item对象已经在队列中
    EEN_BI_DIR_QUEUE__ITEM_NOT_BE_USED = 0x00100013, //!<Item对象未被使用
    EEN_BI_DIR_QUEUE__INSERT_SAME_INDEX = 0x00100014, //!<插入相同的索引
    //...添加BiDirQueue其他错误码

    EEN_PERFORMANCE_STAT = 0x00110000, //!<CPerformanceStatistic
    EEN_PERFORMANCE_STAT__NULL_POINTER = 0x00110010, //!<指针为空
    EEN_PERFORMANCE_STAT__STATISTIC_INIT_FAILED = 0x00110011, //!<CStatistic类初始化失败
    EEN_PERFORMANCE_STAT__SECTION_NUMBER_REACH_UPPER_LIMIT = 0x00110012, //!<统计个数达到上限
    //...添加CPerformanceStatistic其他错误码

    EEN_LINEAR_QUEUE = 0x00120000, //!<CLinearQueue
    EEN_LINEAR_QUEUE__NULL_POINTER = 0x00120010, //!<指针为空
    EEN_LINEAR_QUEUE__QUEUE_IS_FULL = 0x01200011, //!<队列已经满了
    //...添加CLinearQueue其他错误码

    EEN_UDP_SOCKET = 0x00130000, //!<UDP连接类
    EEN_UDP_SOCKET__NULL_POINTER = 0x00130010, //!<指针为空
    EEN_UDP_SOCKET__INVALID_FD = 0x00130011, //!<非法的FD
    EEN_UDP_SOCKET__SOCKET_FAILED = 0x00130012, //!<创建Socket失败
    EEN_UDP_SOCKET__BIND_SOCKET_FAILED = 0x00130013, //!<绑定IP地址失败
    EEN_UDP_SOCKET__CONFIG_USR_BUF_FAILED = 0x00130014, //!<用户缓冲区配置失败
    EEN_UDP_SOCKET__USR_RECV_BUF_FULL = 0x00130015, //!<用户缓冲区满
    EEN_UDP_SOCKET__RECV_FAILED = 0x00130016, //!<调用recvfrom失败
    EEN_UDP_SOCKET__RECV_EAGAIN = 0x00130017, //!<调用recvfrom出现egain
    EEN_UDP_SOCKET__NO_DATA_RECIEVED = 0x00130018, //!<没有接收到数据
    EEN_UDP_SOCKET__SENDTO_FAILED = 0x00130019, //!<调用sendto失败
    //...添加CUDPSocket其他错误码

    EEN_OBJ_HASH_MAP = 0x00140000, //!<TCP连接类
    EEN_OBJ_HASH_MAP__NULL_POINTER = 0x00140010, //!<指针为空
    EEN_OBJ_HASH_MAP__OBJ_IS_FULL = 0x00140011, //!<对象用完了
    EEN_OBJ_HASH_MAP__INSERT_HASH_MAP_FAILED = 0x00140012, //!<插入HashMap时失败

    //...添加CObjHashMap其他错误码

} ENMERRORNO;

const int MAX_ERROR_STRING_LENGTH = 256; //!<错误信息的最大长度

#define USING_ERROR_NO \
	public: \
		int GetErrorNO() const { return m_iMsgErrorNO; } \
	private: \
		int m_iMsgErrorNO; \
		void SetErrorNO(int iErrorNO) { m_iMsgErrorNO = iErrorNO; }
}


#endif //__ERROR_DEF_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
