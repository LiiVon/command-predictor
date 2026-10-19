// CpCmdProtocol
// app的命令与预测工具的命令   的 翻译协议
#pragma once

#include <string>
#include <vector>

#include "CpPredictInfo.h"

enum class CpApplyResult
{
    kOk = 0,
    kDontExecute = 1, // app 端 主动停止
    kError = 2
};

class ICpCmdProtocol
{
public:
    virtual ~ICpCmdProtocol() = default;

    // 目前翻译的是 哪个命令
    virtual std::string getCmdName() const = 0;

    // app端的 命令  --->   CpCmd 预测机制端的命令
    virtual int createInfo(int sourceDataIdx,std::vector<CpCmd *> cmdVec) = 0;

    // CpCmd 预测机制端的命令  --->   app端的 命令
    virtual CpApplyResult applyInfo(CpCmd *cmd,CpCmdGroup *group,int &execDataIdx,std::string& appliedContextId) = 0;

    // 目前翻译的命令  涉及到的 上下文书籍
    virtual std::string contextData(int sourceDataIdx) const = 0;
};