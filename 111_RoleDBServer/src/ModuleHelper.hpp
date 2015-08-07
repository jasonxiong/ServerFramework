#ifndef __MODULE_HELPER_HPP__
#define __MODULE_HELPER_HPP__

class CModuleHelper
{
public:
    // ·þÎñÆ÷ID
	static void RegisterWorldID(int iWorldID) { m_iWorldID = iWorldID; }
    static int GetWorldID() { return m_iWorldID; };

private:
	static int m_iWorldID;
};

#endif
