#include "CpPredictor.h"
#include <iostream>

// ========================= N-gram Predictor =========================

// 统计 命令A --> 命令B 的转移次数
void NgramPredictor::observeCommand(const std::string &cmdName)
{
    if (cmdName.empty())
        return;
    if (!m_history_.empty())
    {
        const std::string &prevCmd = m_history_.back();
        m_transitionCounts_[std::make_pair(prevCmd, cmdName)]++;
    }
    m_commandCounts_[cmdName]++;
    m_history_.push_back(cmdName);
}

std::string NgramPredictor::predictSingle()
{
    if (m_history_.empty())
        return "";

    const std::string &lastCmd = m_history_.back();
    std::string bestCmd;
    int bestCount = 0;

    for (const auto &kv : m_transitionCounts_)
    {
        if (kv.first.first == lastCmd && kv.second > bestCount)
        {
            bestCount = kv.second;
            bestCmd = kv.first.second;
        }
    }
    if (!bestCmd.empty())
    {
        return bestCmd;
    }
    // 如果没有找到转移命令，则返回出现次数最多的命令
    for (const auto &kv : m_commandCounts_)
    {
        if (kv.second > bestCount)
        {
            bestCount = kv.second;
            bestCmd = kv.first;
        }
    }
    return bestCmd;
}

std::vector<std::string> NgramPredictor::predictSequence()
{
    std::vector<std::string> seq;
    for (int i = 0; i < 3; ++i)
    {
        std::string nextCmd = predictSingle();
        if (nextCmd.empty())
            break;
        seq.push_back(nextCmd);
        observeCommand(nextCmd); // 将预测的命令加入历史，以便预测下一条
    }
    for (size_t i = 0; i < seq.size(); ++i)
    {
        m_history_.pop_back(); // 移除预测的命令，恢复历史
    }
    return seq;
}

bool NgramPredictor::isLoaded() const
{
    return !m_history_.empty();
}

std::string NgramPredictor::getEngineName() const
{
    return "N-gram";
}

void NgramPredictor::dumpStats() const
{
    std::cout << "Transition Counts:\n";
    for (const auto &kv : m_transitionCounts_)
    {
        std::cout << kv.first.first << " -> " << kv.first.second << ": " << kv.second << "\n";
    }
    std::cout << "Command Counts:\n";
    for (const auto &kv : m_commandCounts_)
    {
        std::cout << kv.first << ": " << kv.second << "\n";
    }
}

void ONNXPredictor::observeCommand(const std::string &cmdName)
{
    // Implementation for observing commands in ONNX predictor
}

std::string ONNXPredictor::predictSingle()
{
    // Implementation for predicting a single command in ONNX predictor
    return "";
}

std::vector<std::string> ONNXPredictor::predictSequence()
{
    // Implementation for predicting a sequence of commands in ONNX predictor
    return {};
}

bool ONNXPredictor::isLoaded() const
{
    // Implementation to check if ONNX predictor is loaded
    return false;
}