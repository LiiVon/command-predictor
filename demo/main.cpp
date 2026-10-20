#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "CpManager.h"
#include "CpPredictor.h"
#include "PpGlobal.h"
#include "PpPrivate.h"

namespace
{
void testPredictor()
{
    NgramPredictor predictor;
    assert(!predictor.isLoaded());

    predictor.observeCommand("Create");
    predictor.observeCommand("Edit");
    predictor.observeCommand("Save");
    predictor.observeCommand("Edit");
    predictor.observeCommand("Save");

    assert(predictor.isLoaded());
    assert(predictor.predictSingle() == "Edit");
    const auto sequence = predictor.predictSequence();
    assert(sequence.size() == 3);
    assert(sequence.front() == "Edit");
}

void testProtocol()
{
    assert(PredictProtocolInit() == 0);

    auto &manager = CpManager::getInstance();
    assert(manager.getProtocolCount() >= 1);
    assert(manager.getProtocol("Cylinder") != nullptr);

    const int sourceIndex = CmdDataStore::addCommand("Cylinder");
    CmdDataStore::setParam(sourceIndex, "centerX", 1.0);
    CmdDataStore::setParam(sourceIndex, "centerY", 2.0);
    CmdDataStore::setParam(sourceIndex, "centerZ", 3.0);
    CmdDataStore::setParam(sourceIndex, "radius", 4.0);
    CmdDataStore::setParam(sourceIndex, "length", 5.0);

    std::vector<CpCmd *> commands;
    assert(manager.createCmdInfo("Cylinder", sourceIndex, commands) == 0);
    assert(commands.size() == 1);

    auto *group = new CpCmdGroup();
    group->addCmd(commands.front());
    manager.setPredictedResult(new CpCmdInfo());
    assert(manager.getPredictedCmdGroupCount() == 0);

    int executionIndex = -1;
    std::string contextId;
    const auto result = manager.applyCmdInfo(commands.front(), group, executionIndex, contextId);
    assert(result == CpApplyResult::kOk);
    assert(executionIndex >= 0);
    assert(contextId == commands.front()->getId());

    double value = 0.0;
    assert(CmdDataStore::getParam(executionIndex, "centerX", value));
    assert(std::abs(value - 1.0) < 1e-9);
    assert(CmdDataStore::getParam(executionIndex, "radius", value));
    assert(std::abs(value - 4.0) < 1e-9);
    assert(manager.getContextData("Cylinder", sourceIndex).find("Ftr#Cylinder") == 0);

    delete group;
    manager.setPredictedResult(nullptr);
}

void testUtilities()
{
    const auto encoded = CpManager::encodeCmdNameAndType("Cylinder", 2);
    assert(encoded == "Cylinder__2");
    assert(CpManager::decodeCmdName(encoded) == "Cylinder");
    assert(CpManager::decodeCmdtype(encoded) == 2);

    const auto point = PpTool::pointToString(1.0, 2.0, 3.0);
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    assert(PpTool::pointFromString(point, x, y, z) == 0);
    assert(std::abs(x - 1.0) < 1e-9);
    assert(std::abs(y - 2.0) < 1e-9);
    assert(std::abs(z - 3.0) < 1e-9);
}
} // namespace

int main()
{
    testPredictor();
    testProtocol();
    testUtilities();
    PredictProtocolExit();
    std::cout << "command_predictor_demo: all tests passed\n";
    return 0;
}
