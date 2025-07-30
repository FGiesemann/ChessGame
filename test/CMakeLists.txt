add_executable(chessgame_tests
    src/san_parser_test.cpp
)
add_compiler_warnings(chessgame_tests)

target_link_libraries(chessgame_tests PRIVATE ChessGame Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(chessgame_tests)
