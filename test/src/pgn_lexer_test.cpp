/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessgame/pgn.h"

#include <string>

using namespace chessgame;

auto check_token(PGNLexer &lexer, PGNLexer::TokenType expected_type, int line, std::string expected_value = "") -> void {
    auto token = lexer.next_token();
    CAPTURE(expected_type);
    CAPTURE(line);
    CAPTURE(expected_value);
    REQUIRE(token.has_value());
    CHECK(token->type == expected_type);
    CHECK(token->line == line);
    CHECK(token->value == expected_value);
}

TEST_CASE("PGN.Lexer", "[pgn]") {
    std::string pgn_data{"[Event \"Test Event\"]\n"
                         "[Site \"Test Site\"]\n"
                         "[Date \"2020.01.01\"]\n"
                         "[Round \"1\"]\n"
                         "[White \"White\"]\n"
                         "[Black \"Black\"]\n"
                         "[Result \"1-0\"]\n\n"
                         "1. e4 e5 Nf3 2. .. Nc6 3 Bb5+ 1-0"};
    auto pgn_stream = std::istringstream{pgn_data};
    auto lexer = PGNLexer{pgn_stream};
    CHECK(lexer.line_number() == 1);

    check_token(lexer, PGNLexer::TokenType::OpenBracket, 1);
    check_token(lexer, PGNLexer::TokenType::Name, 1, "Event");
    check_token(lexer, PGNLexer::TokenType::String, 1, "Test Event");
    check_token(lexer, PGNLexer::TokenType::CloseBracket, 1);

    check_token(lexer, PGNLexer::TokenType::OpenBracket, 2);
    check_token(lexer, PGNLexer::TokenType::Name, 2, "Site");
    check_token(lexer, PGNLexer::TokenType::String, 2, "Test Site");
    check_token(lexer, PGNLexer::TokenType::CloseBracket, 2);

    check_token(lexer, PGNLexer::TokenType::OpenBracket, 3);
    check_token(lexer, PGNLexer::TokenType::Name, 3, "Date");
    check_token(lexer, PGNLexer::TokenType::String, 3, "2020.01.01");
    check_token(lexer, PGNLexer::TokenType::CloseBracket, 3);

    check_token(lexer, PGNLexer::TokenType::OpenBracket, 4);
    check_token(lexer, PGNLexer::TokenType::Name, 4, "Round");
    check_token(lexer, PGNLexer::TokenType::String, 4, "1");
    check_token(lexer, PGNLexer::TokenType::CloseBracket, 4);

    check_token(lexer, PGNLexer::TokenType::OpenBracket, 5);
    check_token(lexer, PGNLexer::TokenType::Name, 5, "White");
    check_token(lexer, PGNLexer::TokenType::String, 5, "White");
    check_token(lexer, PGNLexer::TokenType::CloseBracket, 5);

    check_token(lexer, PGNLexer::TokenType::OpenBracket, 6);
    check_token(lexer, PGNLexer::TokenType::Name, 6, "Black");
    check_token(lexer, PGNLexer::TokenType::String, 6, "Black");
    check_token(lexer, PGNLexer::TokenType::CloseBracket, 6);

    check_token(lexer, PGNLexer::TokenType::OpenBracket, 7);
    check_token(lexer, PGNLexer::TokenType::Name, 7, "Result");
    check_token(lexer, PGNLexer::TokenType::String, 7, "1-0");
    check_token(lexer, PGNLexer::TokenType::CloseBracket, 7);

    check_token(lexer, PGNLexer::TokenType::Number, 9, "1");
    check_token(lexer, PGNLexer::TokenType::Dot, 9);
    check_token(lexer, PGNLexer::TokenType::Name, 9, "e4");
    check_token(lexer, PGNLexer::TokenType::Name, 9, "e5");
    check_token(lexer, PGNLexer::TokenType::Name, 9, "Nf3");
    check_token(lexer, PGNLexer::TokenType::Number, 9, "2");
    check_token(lexer, PGNLexer::TokenType::Dot, 9);
    check_token(lexer, PGNLexer::TokenType::Dot, 9);
    check_token(lexer, PGNLexer::TokenType::Dot, 9);
    check_token(lexer, PGNLexer::TokenType::Name, 9, "Nc6");
    check_token(lexer, PGNLexer::TokenType::Number, 9, "3");
    check_token(lexer, PGNLexer::TokenType::Name, 9, "Bb5+");
    check_token(lexer, PGNLexer::TokenType::GameResult, 9, "1-0");
}
