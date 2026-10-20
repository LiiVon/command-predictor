add_executable(command_predictor_demo
    main.cpp
)

target_link_libraries(command_predictor_demo
    PRIVATE
        PredictProtocol
        CmdPredictor
)

add_custom_command(TARGET command_predictor_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:CmdPredictor>"
        "$<TARGET_FILE_DIR:command_predictor_demo>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:PredictProtocol>"
        "$<TARGET_FILE_DIR:command_predictor_demo>"
)

if (MSVC)
    target_compile_options(command_predictor_demo PRIVATE /W4)
else()
    target_compile_options(command_predictor_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()