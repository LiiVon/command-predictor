#pragma once

#include <string>
#include <vector>

#include "CpCmdProtocl.h"
#include "CpManager.h"
#include "PpExport.h"

class PP_API PpTool final
{
public:
    static std::string pointToString(double x, double y, double z);
    static int pointFromString(const std::string &str, double &x, double &y, double &z);
};

#define PP_REGISTER_IMPLEMENT(PROTOCOL_CLASS)                                    \
    static PROTOCOL_CLASS *g_p##PROTOCOL_CLASS##Ptr = nullptr;                   \
    void CdPp##PROTOCOL_CLASS##Register()                                        \
    {                                                                            \
        if (!g_p##PROTOCOL_CLASS##Ptr)                                           \
        {                                                                        \
            g_p##PROTOCOL_CLASS##Ptr = new PROTOCOL_CLASS();                     \
            CpManager::getInstance().registerProtocol(g_p##PROTOCOL_CLASS##Ptr); \
        }                                                                        \
    }

#define PP_REGISTER(PROTOCOL_CLASS)               \
    extern void CdPp##PROTOCOL_CLASS##Register(); \
    CdPp##PROTOCOL_CLASS##Register()



// 特征feature 翻译
class PP_API PpFtrProtocol  : public ICpCmdProtocol
{
public:
    std::string getContextData(int sourceDataIdx)  override;
};

// 草图profile 翻译
class PP_API PpProfProtocol  : public ICpCmdProtocol
{
public:
    std::string getContextData(int sourceDataIdx)  override;
};