// CpPredictor.h
// 推理引擎
#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "CpExport.h"

class CP_API ICpPredictor
{
public:
    virtual ~ICpPredictor() = default;
    // 用户执行的命令 存入 推理引擎
    virtual void observeCommand(const std::string &cmdName) = 0;

    // 推理引擎 预测下一步命令
    virtual std::string predictSingle() = 0;

    // 推理引擎 预测下一串命令
    virtual std::vector<std::string> predictSequence() = 0;

    // 是否可用
    virtual bool isLoaded() const = 0;

    virtual std::string getEngineName() const = 0;
};

// N-gram 推理引擎
class CP_API NgramPredictor final : public ICpPredictor
{
public:
    NgramPredictor() = default;

    void observeCommand(const std::string &cmdName) override;
    std::string predictSingle() override;
    std::vector<std::string> predictSequence() override;
    bool isLoaded() const override;
    std::string getEngineName() const override;

    // 展示统计信息
    void dumpStats() const;

private:
    std::vector<std::string> m_history_;                                    // 历史命令记录
    std::map<std::pair<std::string, std::string>, int> m_transitionCounts_; // 转移计数器，记录命令之间的转移次数
    std::map<std::string, int> m_commandCounts_;                            // 命令计数器，记录每个命令的出现次数
};

// ONNX
class CP_API ONNXPredictor final : public ICpPredictor
{
public:
    ONNXPredictor() = default;

    void observeCommand(const std::string &cmdName) override;
    std::string predictSingle() override;
    std::vector<std::string> predictSequence() override;
    bool isLoaded() const override;
    std::string getEngineName() const override { return "ONNX"; }

private:
    std::vector<std::string> m_history_; // 历史命令记录
};