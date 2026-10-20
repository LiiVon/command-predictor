#include "PpPrivate.h"
#include "CpManager.h"
#include "CpCmdProtocl.h"

class PpFtrAllCyl final : public PpFtrProtocol
{
public:
    std::string getCmdName() const override;
    int createInfo(int sourceDataIdx, std::vector<CpCmd *> &cmdVec) override;
    CpApplyResult applyInfo(CpCmd *cmd, CpCmdGroup *group, int &execDataIdx, std::string &appliedContextId) override;
};

std::string PpFtrAllCyl::getCmdName() const
{
    return "Cylinder";
}

// 从CmdDataStore中读取 圆柱 参数数据，创建CpCmd对象，并将其添加到cmdVec中
int PpFtrAllCyl::createInfo(int sourceDataIdx, std::vector<CpCmd *> &cmdVec)
{
    if (CmdDataStore::getName(sourceDataIdx) != getCmdName())
        return -1; // sourceDataIdx does not match the command name

    auto &Mgr = CpManager::getInstance();
    CpCmd *pCmd = new CpCmd(CpManager::encodeCmdNameAndType(getCmdName(), -1), Mgr.getContextId(), "1.0");
    Mgr.moveContextIdToNext();
    cmdVec.push_back(pCmd);

    double cx = 0.0, cy = 0.0, cz = 0.0;
    CmdDataStore::getParam(sourceDataIdx, "centerX", cx);
    CmdDataStore::getParam(sourceDataIdx, "centerY", cy);
    CmdDataStore::getParam(sourceDataIdx, "centerZ", cz);
    CpParam *pCenter = pCmd->addParam("point0");
    CpSelectValue *pSel = new CpSelectValue(CpParamValueType::kPoint);
    pSel->setValue(PpTool::pointToString(cx, cy, cz));
    pCenter->addValue(pSel);

    double radius = 0.0;
    CmdDataStore::getParam(sourceDataIdx, "radius", radius);
    CpParam *pRadius = pCmd->addParam("double0");
    CpNormalValue *pNormRadius = new CpNormalValue(CpParamValueType::kDouble);
    pNormRadius->setDouble(radius);
    pRadius->addValue(pNormRadius);

    double length = 0.0;
    CmdDataStore::getParam(sourceDataIdx, "length", length);
    CpParam *pLength = pCmd->addParam("double1");
    CpNormalValue *pNormLength = new CpNormalValue(CpParamValueType::kDouble);
    pNormLength->setDouble(length);
    pLength->addValue(pNormLength);

    return 0;
}

CpApplyResult PpFtrAllCyl::applyInfo(CpCmd *cmd, CpCmdGroup *group, int &execDataIdx, std::string &appliedContextId)
{
    if (CpManager::decodeCmdName(cmd->getName()) != getCmdName())
        return CpApplyResult::kError; // command name does not match

    execDataIdx = CmdDataStore::addCommand(getCmdName());

    // 圆心
    CpParam *pCenter = cmd->getParam("point0");
    double cx = 0.0, cy = 0.0, cz = 0.0;
    if (pCenter && pCenter->getValue(0))
    {
        CpSelectValue *v = static_cast<CpSelectValue *>(pCenter->getValue(0));
        if (PpTool::pointFromString(v->getValue(), cx, cy, cz) >= 0)
        {
            CmdDataStore::setParam(execDataIdx, "centerX", cx);
            CmdDataStore::setParam(execDataIdx, "centerY", cy);
            CmdDataStore::setParam(execDataIdx, "centerZ", cz);
        }
    }

    // 半径
    CpParam *pRadius = cmd->getParam("double0");
    double radius = 0.0;
    if (pRadius && pRadius->getValue(0))
    {
        CpNormalValue *v = static_cast<CpNormalValue *>(pRadius->getValue(0));
        v->getDouble(radius);
        CmdDataStore::setParam(execDataIdx, "radius", radius);
    }

    // 长度
    CpParam *pLength = cmd->getParam("double1");
    double length = 0.0;
    if (pLength && pLength->getValue(0))
    {
        CpNormalValue *v = static_cast<CpNormalValue *>(pLength->getValue(0));
        v->getDouble(length);
        CmdDataStore::setParam(execDataIdx, "length", length);
    }

    appliedContextId = cmd->getId();
    return CpApplyResult::kOk;
}

PP_REGISTER_IMPLEMENT(PpFtrAllCyl);
