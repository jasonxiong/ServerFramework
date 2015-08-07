#ifndef __SHM_HPP__
#define __SHM_HPP__

#ifndef WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#ifdef WIN32

/* Mode bits for `msgget', `semget', and `shmget'.  */
#define IPC_CREAT       01000           /* Create key if key does not exist. */
#define IPC_EXCL        02000           /* Fail if key exists.  */
#define IPC_NOWAIT      04000           /* Return error on wait.  */

/* Control commands for `msgctl', `semctl', and `shmctl'.  */
#define IPC_RMID        0               /* Remove identifier.  */
#define IPC_SET         1               /* Set `ipc_perm' options.  */
#define IPC_STAT        2               /* Get `ipc_perm' options.  */
#ifdef __USE_GNU
# define IPC_INFO       3               /* See ipcs.  */
#endif



#define key_t unsigned int

/* The following System V style IPC functions implement a shared memory
facility.  The definition is found in XPG4.2.  */

/* Shared memory control operation.  */
extern int shmctl (int __shmid, int __cmd, struct shmid_ds *__buf) __THROW;

/* Get shared memory segment.  */
extern int shmget (key_t __key, size_t __size, int __shmflg) __THROW;

/* Attach shared memory segment.  */
extern void *shmat (int __shmid, __const void *__shmaddr, int __shmflg)
__THROW;

/* Detach shared memory segment.  */
extern int shmdt (__const void *__shmaddr) __THROW;

#endif


//modify by cpp 0716
//#include "GlobalValue.hpp"
#include "LogAdapter.hpp"

using namespace ServerLib;
//end modify by cpp 0716

class CSharedMemory
{
public:
    key_t GenerateShmKey(const char* pszKeyFileName, const unsigned char ucKeyPrjID);
    int AllocateShmSpace(size_t iSize);
    int CreateShmSegment(const char* pszKeyFileName, const unsigned char ucKeyPrjID, size_t iSize);
    int CreateShmSegmentByKey(key_t uiShmKey, size_t iSize);
    int DestroyShmSegment(const char* pszKeyFileName, const unsigned char ucKeyPrjID);
    int DestroyShmSegment();

    int UseShmBlock(size_t iSize);
    int UseShmBlock(size_t iSize, char* & rpszShm);

    const char* GetFreeMemoryAddress();
    const char* GetInitialMemoryAddress();

    int  WriteShmKey();

public:

    size_t GetFreeMemorySize();

private:
    key_t m_uiShmKey;
    size_t m_iShmSize;

    int m_iShmID; //!<创建共享内存的ID,调用shmget得到
    size_t m_iFreeOffset; //!<当前还未分配的共享内存的指针偏移
    char* m_pszInitialMemoryAddress; //!<创建共享内存的头指针


};



#endif


