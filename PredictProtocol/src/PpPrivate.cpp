#include "PpPrivate.h"



std::string PpTool::pointToString(double x, double y, double z)
{
    return std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
}

int PpTool::pointFromString(const std::string &str, double &x, double &y, double &z)
{
    size_t firstComma = str.find(',');
    if (firstComma == std::string::npos)
        return -1; // invalid format
    size_t secondComma = str.find(',', firstComma + 1);
    if (secondComma == std::string::npos)
        return -1; // invalid format

    try
    {
        x = stod(str.substr(0, firstComma));
        y = stod(str.substr(firstComma + 1, secondComma - firstComma - 1));
        z = stod(str.substr(secondComma + 1));
    }
    catch (...)
    {
        return -1; // conversion failed
    }
    return 0; // success
}

std::string PpFtrProtocol::getContextData(int sourceDataIdx)
{
    // For feature protocol, we can return a string representation of the sourceDataIdx
    return "Ftr#" + CmdDataStore::getName(sourceDataIdx) + ":" + std::to_string(sourceDataIdx);
}

std::string PpProfProtocol::getContextData(int sourceDataIdx)
{
    // For profile protocol, we can return a string representation of the sourceDataIdx
    return "Prof#" + CmdDataStore::getName(sourceDataIdx) + ":" + std::to_string(sourceDataIdx);
}