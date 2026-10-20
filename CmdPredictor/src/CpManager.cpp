#include "CpManager.h"

#include <algorithm>
#include <sstream>

// ========================= CmdDataStore =========================
CmdDataStore &CmdDataStore::getInstance()
{
    static CmdDataStore instance;
    return instance;
}

int CmdDataStore::addCommand(const std::string name)
{
    getInstance().m_records_.push_back({name, {}});
    return static_cast<int>(getInstance().m_records_.size() - 1);
}

void CmdDataStore::setParam(int idx, const std::string &key, double value)
{
    if (idx < 0 || idx >= static_cast<int>(getInstance().m_records_.size()))
        return;
    getInstance().m_records_[idx].params[key] = value;
}

bool CmdDataStore::getParam(int idx, const std::string &key, double &value)
{
    if (idx < 0 || idx >= static_cast<int>(getInstance().m_records_.size()))
        return false;
    auto it = getInstance().m_records_[idx].params.find(key);
    if (it == getInstance().m_records_[idx].params.end())
        return false;
    value = it->second;
    return true;
}

std::string CmdDataStore::getName(int idx)
{
    if (idx < 0 || idx >= static_cast<int>(getInstance().m_records_.size()))
        return {};
    return getInstance().m_records_[idx].name;
}

// ========================= CpManager =========================
CpManager::~CpManager()
{
    if (m_predictInfo_)
    {
        delete m_predictInfo_;
    }
    m_predictInfo_ = nullptr;
}

CpManager &CpManager::getInstance()
{
    static CpManager instance;
    return instance;
}

int CpManager::registerProtocol(ICpCmdProtocol *protocol)
{
    if (!protocol)
        return -1;
    std::string cmdName = protocol->getCmdName();
    if (cmdName.empty())
        return -2;
    if (m_protocolMap_.find(cmdName) != m_protocolMap_.end())
        return -3; // already registered
    m_protocolMap_[cmdName] = protocol;
    return 0;
}

void CpManager::unregisterProtocol(ICpCmdProtocol *protocol)
{
    if (!protocol)
        return;
    std::string cmdName = protocol->getCmdName();
    auto it = m_protocolMap_.find(cmdName);
    if (it != m_protocolMap_.end())
    {
        m_protocolMap_.erase(it);
    }
}

ICpCmdProtocol *CpManager::getProtocol(const std::string &cmdName) const
{
    auto it = m_protocolMap_.find(cmdName);
    if (it != m_protocolMap_.end())
    {
        return it->second;
    }
    return nullptr;
}

int CpManager::getProtocolCount() const
{
    return static_cast<int>(m_protocolMap_.size());
}

void CpManager::setPredictedResult(CpCmdInfo *info)
{
    if (m_predictInfo_)
    {
        delete m_predictInfo_;
    }
    m_predictInfo_ = info;
}

int CpManager::getPredictedCmdGroupCount() const
{
    if (!m_predictInfo_)
        return 0;
    return m_predictInfo_->getCmdGroupCount();
}

CpCmdGroup *CpManager::getPredictedCmdGroup(int idx) const
{
    if (!m_predictInfo_)
        return nullptr;
    if (idx < 0 || idx >= m_predictInfo_->getCmdGroupCount())
        return nullptr;
    return m_predictInfo_->getGroup(idx);
}

std::string CpManager::getContextId() const
{
    return std::string("cmd_") + std::to_string(m_curId_);
}

void CpManager::moveContextIdToNext()
{
    ++m_curId_;
}

int CpManager::addToCurrent(const std::string &contextId, const std::string &contextData)
{
    if (contextId.empty() || contextData.empty())
        return -1;
    if (m_contextMap_.find(contextId) != m_contextMap_.end())
    {
        return -2; // already exists
    }
    m_contextMap_[contextId] = contextData;
    return 0;
}

int CpManager::getDataFromCurrent(const std::string &contextId, std::string &contextData) const
{
    auto it = m_contextMap_.find(contextId);
    if (it == m_contextMap_.end())
    {
        return -1; // not found
    }
    contextData = it->second;
    return 0;
}

void CpManager::resetContext()
{
    m_contextMap_.clear();
    m_curId_ = 0;
}

int CpManager::createCmdInfo(const std::string &cmdName, int sourceDataIdx, std::vector<CpCmd *> cmdVec)
{
    ICpCmdProtocol *protocol = getProtocol(cmdName);
    if (!protocol)
        return -1; // protocol not found
    return protocol->createInfo(sourceDataIdx, cmdVec);
}

CpApplyResult CpManager::applyCmdInfo(CpCmd *cmd, CpCmdGroup *group, int &execDataIdx, std::string &appliedContextId)
{
    if(cmd == nullptr || group == nullptr || cmd->getName().empty())
        return CpApplyResult::kError; // invalid input
    ICpCmdProtocol *protocol = getProtocol(cmd->getName());
    if (!protocol)
        return CpApplyResult::kError; // protocol not found
    return protocol->applyInfo(cmd, group, execDataIdx, appliedContextId);
}

std::string CpManager::getContextData(const std::string &cmdName, int sourceDataIdx) const
{
    ICpCmdProtocol *protocol = getProtocol(cmdName);
    if (!protocol)
        return {}; // protocol not found
    return protocol->getContextData(sourceDataIdx);
}

#define NAME_TOKEN "__"

std::string CpManager::encodeCmdNameAndType(const std::string &cmdName, int type)
{
    if(type < 0)
        return cmdName;
    return cmdName + NAME_TOKEN + std::to_string(type);
}

std::string CpManager::decodeCmdName(const std::string &CmdName)
{
    int idx = static_cast<int>(CmdName.rfind(NAME_TOKEN));
    if(idx == -1)
        return CmdName;
    return CmdName.substr(0, idx);
}

int CpManager::decodeCmdtype(const std::string &encodedCmdName)
{
    int idx = static_cast<int>(encodedCmdName.rfind(NAME_TOKEN));
    if(idx == -1)
        return -1;
    std::string typeStr = encodedCmdName.substr(idx + strlen(NAME_TOKEN));
    try
    {
        return std::stoi(typeStr);
    }
    catch (...)
    {
        return -1; // invalid type
    }
}