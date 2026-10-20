// CpManager
// 中央调度员

#pragma once
#include <map>
#include <string>
#include <vector>

#include "CpCmdProtocl.h"
#include "CpExport.h"

// 简易的 数据存储器,保存 app 的命令
class CP_API CmdDataStore
{
public:
    static int addCommand(const std::string name);
    static void setParam(int idx, const std::string &key, double value);
    static bool getParam(int idx, const std::string &key, double &value);
    static std::string getName(int idx);
    static CmdDataStore &getInstance();

private:
    struct CommandRec
    {
        std::string name;
        std::map<std::string, double> params;
    };
    std::vector<CommandRec> m_records_;
};

class CP_API CpManager final
{
public:
    ~CpManager();
    CpManager(const CpManager &) = delete;
    CpManager &operator=(const CpManager &) = delete;

    static CpManager &getInstance();

    // 注册协议
    int registerProtocol(ICpCmdProtocol *protocol);
    void unregisterProtocol(ICpCmdProtocol *protocol);
    ICpCmdProtocol *getProtocol(const std::string &cmdName) const;
    int getProtocolCount() const;

    // 预测结果 仓库
    void setPredictedResult(CpCmdInfo *info);        // 设置预测结果
    int getPredictedCmdGroupCount() const;           // 预测结果中命令组的数量
    CpCmdGroup *getPredictedCmdGroup(int idx) const; // 按idx取出 第idx组命令

    // 上下文 仓库`
    std::string getContextId() const;                                                     // 获取当前上下文id
    void moveContextIdToNext();                                                           // 将当前上下文id 移动到下一个
    int addToCurrent(const std::string &contextId, const std::string &contextData);       // 将数据添加到当前上下文
    int getDataFromCurrent(const std::string &contextId, std::string &contextData) const; // 从当前上下文中获取数据
    void resetContext();                                                                  // 重置上下文仓库

    // 对外快捷入口
    int createCmdInfo(const std::string &cmdName, int sourceDataIdx, std::vector<CpCmd *> cmdVec); // app端的 命令  --->   CpCmd 预测机制端的命令
    CpApplyResult applyCmdInfo(CpCmd *cmd, CpCmdGroup *group, int &execDataIdx, std::string &appliedContextId);  // CpCmd 预测机制端的命令  --->   app端的 命令 
    std::string getContextData(const std::string &cmdName, int sourceDataIdx) const; // 根据命令名和源数据索引获取上下文数据

    // 命令名 编解码 (命令名__类型)
    static std::string encodeCmdNameAndType(const std::string &cmdName, int type); 
    static std::string decodeCmdName(const std::string &cmdName);
    static int decodeCmdtype(const std::string &encodedCmdName);

private:
    CpManager() = default;

    std::map<std::string, ICpCmdProtocol *> m_protocolMap_; // 协议注册表  命令名 -- > 协议
    CpCmdInfo *m_predictInfo_ = nullptr;                    // 预测结果仓库
    int m_curId_ = 0;                                       // 当前上下文id
    std::map<std::string, std::string> m_contextMap_;       // 上下文仓库  id -- > 数据
};