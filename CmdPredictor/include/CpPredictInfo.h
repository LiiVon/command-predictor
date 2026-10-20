#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <CpExport.h>

// 值的类型
enum class CpParamValueType
{
    kInt = 0,
    kDouble = 1,
    kString = 2,
    kPoint = 3,   // 点
    kEntity = 4   // 实体
};

// 值的来源
enum class CpParamValueSource
{
    kNormal, // 正常填写
    kSelect  // 鼠标点击选择
};

// 值  记录种类和来源  抽象类
class CP_API CpParamValue
{
public:
    CpParamValue(CpParamValueSource source, CpParamValueType type) : m_source_(source), m_type_(type) {}
    virtual ~CpParamValue() = default;
    CpParamValueSource getSource() const { return m_source_; }
    CpParamValueType getType() const { return m_type_; }

private:
    CpParamValueSource m_source_;
    CpParamValueType m_type_;
};

class CP_API CpNormalValue final : public CpParamValue
{
public:
    explicit CpNormalValue(CpParamValueType type) : CpParamValue(CpParamValueSource::kNormal, type) {}
    void setInt(int value) { m_doubleValue_ = static_cast<double>(value); }
    void setDouble(double value) { m_doubleValue_ = value; }
    void setString(const std::string &value) { m_stringValue_ = value; }

    int getInt(int &val) const
    {
        val = static_cast<int>(m_doubleValue_);
        return 0;
    }

    int getDouble(double &val) const
    {
        val = m_doubleValue_;
        return 0;
    }

    int getString(std::string &val) const
    {
        val = m_stringValue_;
        return 0;
    }

private:
    double m_doubleValue_;
    std::string m_stringValue_;
};

class CP_API CpSelectValue final : public CpParamValue
{
public:
    explicit CpSelectValue(CpParamValueType type) : CpParamValue(CpParamValueSource::kSelect, type) {}

    void setValue(const std::string &value) { m_value_ = value; }
    std::string getValue() const { return m_value_; }

private:
    std::string m_value_;
};

// 参数：名字 + 若干值
class CP_API CpParam final
{
public:
    explicit CpParam(const std::string &name) : m_name_(name) {}
    ~CpParam()
    {
        for (auto *v : m_values_)
        {
            delete v;
        }
        m_values_.clear();
    }

    std::string getName() const { return m_name_; }
    void addValue(CpParamValue *value) { m_values_.push_back(value); }
    CpParamValue *getValue(size_t idx) const
    {
        if(idx < 0 || idx >= m_values_.size())
        {
            return nullptr;
        }
        return m_values_[idx];
    }
    int valueCount() const { return static_cast<int>(m_values_.size()); }

private:
    std::string m_name_;
    std::vector<CpParamValue *> m_values_;
};

// 命令：名字 + 单号 + 版本 + 若干参数
class CP_API CpCmd final
{
public:
    CpCmd(const std::string&name,const std::string id,const std::string version) : m_name_(name), m_id_(id), m_version_(version) {}
    ~CpCmd()
    {
        for(auto *p : m_params_)
        {
            delete p;
        }
        m_params_.clear();
    }

    CpParam* addParam(const std::string &name)
    {
        CpParam *param = new CpParam(name);
        m_params_.push_back(param);
        return param;
    }

    CpParam* getParam(size_t idx) const
    {
        if(idx < 0 || idx >= m_params_.size())
        {
            return nullptr;
        }
        return m_params_[idx];
    }

    CpParam* getParam(const std::string &name) const
    {
        if (name.empty())
        {
            return nullptr;
        }
        auto it = std::find_if(m_params_.begin(), m_params_.end(), [&name](const CpParam *param) {
            return param->getName() == name;
        });
        if (it != m_params_.end())
        {
            return *it;
        }
        return nullptr;
    }

    std::string getName() const { return this->m_name_; }
    std::string getId() const { return this->m_id_; }
    std::string getVersion() const { return this->m_version_; }
private:
    std::string m_name_;
    std::string m_id_;
    std::string m_version_;
    std::vector<CpParam *> m_params_;
};

// 一组 命令
class CP_API CpCmdGroup final
{
public:
    CpCmdGroup() = default;
    ~CpCmdGroup()
    {
        for(auto *c : m_cmds_)
        {
            delete c;
        }
        m_cmds_.clear();
    }

    void addCmd(CpCmd *cmd) { m_cmds_.push_back(cmd); }
    CpCmd* getCmd(size_t idx) const
    {
        if(idx < 0 || idx >= m_cmds_.size())
        {
            return nullptr;
        }
        return m_cmds_[idx];
    }
    const std::vector<CpCmd *>& getCmds() const { return m_cmds_; }
private:
    std::vector<CpCmd *> m_cmds_;
};


// 上述结果 最终容器
class CP_API CpCmdInfo final
{
public:
    CpCmdInfo() = default;
    ~CpCmdInfo()
    {
        for(auto *g : m_cmdGroups_)
        {
            delete g;
        }
        m_cmdGroups_.clear();
    }

    void addGroup(CpCmdGroup *group) { m_cmdGroups_.push_back(group); }
    CpCmdGroup* getGroup(size_t idx) const
    {
        if(idx < 0 || idx >= m_cmdGroups_.size())
        {
            return nullptr;
        }
        return m_cmdGroups_[idx];
    }
    const std::vector<CpCmdGroup *>& getGroups() const { return m_cmdGroups_; }
    int getCmdGroupCount() const { return static_cast<int>(m_cmdGroups_.size()); }
private:
    std::vector<CpCmdGroup *> m_cmdGroups_;
};