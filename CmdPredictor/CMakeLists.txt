add_library(CmdPredictor SHARED
    src/CpManager.cpp
    src/CpPredictInfo.cpp
    src/CpPredictor.cpp
)

target_compile_definitions(CmdPredictor PRIVATE CP_BUILD)

target_include_directories(CmdPredictor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)