add_executable(pgn_parser
    pgn_parser.cpp
    pgn_database.cpp
)

target_link_libraries(pgn_parser ChessGame)
