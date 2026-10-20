#include "PpGlobal.h"
#include "CpManager.h"
#include "PpPrivate.h"

extern void CdPpPpFtrAllCylRegister();

PP_C_API int PredictProtocolInit()
{
    CdPpPpFtrAllCylRegister();
    return 0;
}

PP_C_API int PredictProtocolExit()
{
    // 清理资源
    return 0;
}