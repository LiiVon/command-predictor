# Command-Predictor

一个基于 C++17 的命令预测与命令协议转换示例项目。项目当前包含：

- `CmdPredictor`：命令预测核心库和命令数据模型。
- `PredictProtocol`：将应用侧命令转换为预测侧命令，并将预测命令应用回应用侧的协议库。
- `demo`：可执行的端到端测试程序。

## 项目结构

```text
.
├── CMakeLists.txt
├── CmdPredictor/
│   ├── include/
│   │   ├── CpCmdProtocl.h       # 命令协议接口
│   │   ├── CpManager.h          # 协议注册、上下文和预测结果管理
│   │   ├── CpPredictInfo.h      # 命令、参数和预测结果数据结构
│   │   └── CpPredictor.h        # 预测器接口和 N-gram 实现
│   └── src/
├── PredictProtocol/
│   ├── include/
│   │   ├── PpGlobal.h           # 协议库初始化接口
│   │   ├── PpPrivate.h          # 协议工具和协议基类
│   │   └── PpExport.h           # DLL 导出宏
│   └── src/
└── demo/
    ├── CMakeLists.txt
    └── main.cpp                 # 构建后执行的测试程序
```

## 构建要求

- CMake 3.16 或更高版本
- 支持 C++17 的编译器
- Windows 下可使用 Visual Studio 生成器；Linux/macOS 下可使用 GCC 或 Clang

## 构建和运行

在项目根目录执行：

### Windows

```powershell
cmake -S . -B build
cmake --build build --config Debug --parallel
.\build\demo\Debug\command_predictor_demo.exe
```

### Linux/macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/demo/command_predictor_demo
```

demo 构建完成后会自动将 `CmdPredictor` 和 `PredictProtocol` 的动态库复制到可执行文件目录，因此可以直接运行 demo。

运行成功时输出：

```text
command_predictor_demo: all tests passed
```

## 核心模块

### 1. 命令预测

`NgramPredictor` 实现了基于相邻命令转移次数的简单预测：

```cpp
#include "CpPredictor.h"

NgramPredictor predictor;
predictor.observeCommand("Create");
predictor.observeCommand("Edit");
predictor.observeCommand("Save");

const std::string next = predictor.predictSingle();
const std::vector<std::string> sequence = predictor.predictSequence();
```

- `observeCommand`：记录用户实际执行的命令。
- `predictSingle`：根据最近一条命令预测下一条命令；没有转移记录时返回出现次数最多的命令。
- `predictSequence`：连续预测最多 3 条命令。
- `isLoaded`：当前是否已经记录过命令。
- `dumpStats`：输出命令和转移统计信息。

`ONNXPredictor` 已定义接口，但当前实现仍是占位实现，尚未接入 ONNX 模型。

### 2. 命令数据模型

命令数据由以下层级组成：

```text
CpCmdInfo
└── CpCmdGroup
    └── CpCmd
        └── CpParam
            └── CpParamValue
```

参数值支持：

- `CpNormalValue`：普通输入值，包括整数、浮点数和字符串。
- `CpSelectValue`：通过选择操作获得的值，例如点或实体。

`CpCmdInfo`、`CpCmdGroup`、`CpCmd` 和 `CpParam` 按当前代码设计负责释放其持有的指针对象。添加对象后不要重复释放，也不要将同一个对象交给多个拥有者。

### 3. 协议管理

`CpManager` 是单例管理器，负责：

- 注册和查找 `ICpCmdProtocol`。
- 保存预测结果 `CpCmdInfo`。
- 管理上下文 ID 和上下文数据。
- 调用协议完成应用命令与预测命令之间的转换。

典型调用流程如下：

```cpp
#include "CpManager.h"

auto& manager = CpManager::getInstance();
const int sourceIndex = CmdDataStore::addCommand("Cylinder");
CmdDataStore::setParam(sourceIndex, "radius", 4.0);

std::vector<CpCmd*> commands;
manager.createCmdInfo("Cylinder", sourceIndex, commands);

int executionIndex = -1;
std::string appliedContextId;
manager.applyCmdInfo(
    commands.front(),
    commandGroup,
    executionIndex,
    appliedContextId);
```

`createCmdInfo` 的命令容器使用引用传递，协议实现会将创建的 `CpCmd` 添加到调用方提供的 `std::vector<CpCmd*>` 中。

命令名还提供类型编码工具：

```cpp
const auto encoded = CpManager::encodeCmdNameAndType("Cylinder", 2);
// encoded == "Cylinder__2"
CpManager::decodeCmdName(encoded);  // "Cylinder"
CpManager::decodeCmdtype(encoded);  // 2
```

### 4. 当前协议实现

当前 `PredictProtocol` 中实现了 `Cylinder` 协议：

- 从 `CmdDataStore` 读取 `centerX`、`centerY`、`centerZ`、`radius` 和 `length`。
- 创建一个包含点参数和两个浮点参数的 `CpCmd`。
- 将预测命令应用回 `CmdDataStore`。
- 通过 `PpTool` 在 `x,y,z` 字符串与三个浮点数之间转换点数据。

调用协议库初始化函数后，`Cylinder` 协议会注册到 `CpManager`：

```cpp
#include "PpGlobal.h"

PredictProtocolInit();
// CpManager::getInstance().getProtocol("Cylinder") 可取得协议
```

`PpFtrProtocol` 和 `PpProfProtocol` 是协议基类，目前不是可直接实例化的具体协议实现。后续新增协议时，可参考 `PpFtrAllCyl.cpp` 中的实现和注册宏。

## 新增协议

新增具体协议通常需要：

1. 继承 `PpFtrProtocol` 或 `PpProfProtocol`。
2. 实现以下接口：
   - `getCmdName`
   - `createInfo`
   - `applyInfo`
   - `getContextData`
3. 将新的 `.cpp` 文件加入 `PredictProtocol/CMakeLists.txt`。
4. 使用 `PP_REGISTER_IMPLEMENT(协议类名)` 生成注册函数。
5. 在 `PredictProtocolInit` 中调用对应注册函数。

其中 `createInfo` 的签名需要使用命令容器引用：

```cpp
int createInfo(
    int sourceDataIdx,
    std::vector<CpCmd*>& cmdVec) override;
```

## demo 测试范围

[demo/main.cpp](demo/main.cpp) 当前覆盖：

- N-gram 预测器的命令记录和单步/序列预测。
- `PredictProtocolInit` 和 `Cylinder` 协议注册。
- 圆柱命令的创建与应用。
- 参数在 `CmdDataStore` 中的回写。
- 上下文数据获取。
- 命令名称编码与解码。
- 点数据序列化与反序列化。

项目目前使用 C++ `assert` 进行 demo 验证；如果以 `Release` 配置运行，请注意部分编译器配置可能会关闭断言。正式项目可在此基础上替换为独立测试框架。
