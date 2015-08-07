#ifndef __PLUTO_STRING_UTILITY_HPP__
#define __PLUTO_STRING_UTILITY_HPP__

#include <assert.h>
#include <vector>
#include <sstream>

class PlutoStringUtility
{
public:
    static void ParseStringToIntArray(const char* pstrSrc, const char cSep, std::vector<int>& vIntArr)
    {
        vIntArr.clear();
        if(!pstrSrc)
        {
            return;
        }

        std::stringstream ss;
        ss.str(pstrSrc);

        int iVar;
        char cSplit;
        while(ss>> iVar)
        {
            vIntArr.push_back(iVar);
            if(ss>> cSplit)
            {
                assert(cSplit == cSep);
            }
        }

        return;
    }

};

#endif
