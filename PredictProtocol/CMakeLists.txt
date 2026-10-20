add_library(PredictProtocol SHARED
    src/PpFtAllCyl.cpp
    src/PpGlobal.cpp
    src/PpPrivate.cpp
)

target_compile_definitions(PredictProtocol PRIVATE PP_SHARED PP_BUILD)

target_include_directories(PredictProtocol
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/CmdPredictor/include
)

target_link_libraries(PredictProtocol
    PUBLIC
        CmdPredictor
)