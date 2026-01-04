add_executable(chessgame_tests
    src/pgn_lexer_test.cpp
    src/pgn_parser_test.cpp
    src/pgn_writer_test.cpp
    src/san_generator_test.cpp
    src/san_move_matcher_test.cpp
    src/san_parser_test.cpp
)
add_compiler_warnings(chessgame_tests)
target_compile_options(chessgame_tests PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc>)
target_link_libraries(chessgame_tests
  PRIVATE 
  ChessGame 
  Catch2::Catch2WithMain
)
add_optimization_settings(chessgame_tests)

include(Catch)
catch_discover_tests(chessgame_tests)
