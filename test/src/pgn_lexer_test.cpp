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

auto check_tag(PGNLexer &lexer, std::string expected_name, std::string expected_value, int line) -> void {
    CAPTURE(expected_name);
    CAPTURE(expected_value);
    check_token(lexer, PGNLexer::TokenType::OpenBracket, line);
    check_token(lexer, PGNLexer::TokenType::Name, line, expected_name);
    check_token(lexer, PGNLexer::TokenType::String, line, expected_value);
    check_token(lexer, PGNLexer::TokenType::CloseBracket, line);
}

TEST_CASE("PGN.Lexer.Single linear game", "[pgn]") {
    const std::string pgn_data{"[Event \"Test Event\"]\n"
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

    check_tag(lexer, "Event", "Test Event", 1);
    check_tag(lexer, "Site", "Test Site", 2);
    check_tag(lexer, "Date", "2020.01.01", 3);
    check_tag(lexer, "Round", "1", 4);
    check_tag(lexer, "White", "White", 5);
    check_tag(lexer, "Black", "Black", 6);
    check_tag(lexer, "Result", "1-0", 7);

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

TEST_CASE("PGN.Lexer.Commented game", "[pgn]") {
    const std::string pgn_data{
        "[Event \"IBM Kasparov vs. Deep Blue Rematch\"]\n"
        "[Site \"New York, NY USA\"]\n"
        "[Date \"1997.05.11\"]\n"
        "[Round \"6\"]\n"
        "[White \"Deep Blue\"]\n"
        "[Black \"Kasparov, Garry\"]\n"
        "[Opening \"Caro-Kann: 4...Nd7\"]\n"
        "[ECO \"B17\"]\n"
        "[Result \"1-0\"]\n\n"
        "{Beispielpartie in PGN}\n\n"
        "1. e4 c6 2. d4 d5 3. Nc3 dxe4 4. Nxe4 Nd7 5. Ng5 Ngf6 6. Bd3 e6 7. N1f3 h6 8. Nxe6 Qe7 9. O-O fxe6 10. Bg6+ Kd8 {Kasparov schüttelt kurz den Kopf} 11. "
        "Bf4 b5 12. a4 Bb7 13. Re1 Nd5 14. Bg3 Kc8 15. axb5 cxb5 16. Qd3 Bc6 17. Bf5 exf5 18. Rxe7 Bxe7 19. c4 1-0\n"
    };

    auto pgn_stream = std::istringstream{pgn_data};
    PGNLexer lexer{pgn_stream};

    check_tag(lexer, "Event", "IBM Kasparov vs. Deep Blue Rematch", 1);
    check_tag(lexer, "Site", "New York, NY USA", 2);
    check_tag(lexer, "Date", "1997.05.11", 3);
    check_tag(lexer, "Round", "6", 4);
    check_tag(lexer, "White", "Deep Blue", 5);
    check_tag(lexer, "Black", "Kasparov, Garry", 6);
    check_tag(lexer, "Opening", "Caro-Kann: 4...Nd7", 7);
    check_tag(lexer, "ECO", "B17", 8);
    check_tag(lexer, "Result", "1-0", 9);
    check_token(lexer, PGNLexer::TokenType::Comment, 11, "Beispielpartie in PGN");

    check_token(lexer, PGNLexer::TokenType::Number, 13, "1");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "e4");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "c6");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "2");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "d4");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "d5");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "3");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Nc3");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "dxe4");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "4");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Nxe4");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Nd7");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "5");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Ng5");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Ngf6");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "6");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Bd3");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "e6");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "7");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "N1f3");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "h6");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "8");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Nxe6");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Qe7");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "9");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "O-O");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "fxe6");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "10");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Bg6+");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Kd8");
    check_token(lexer, PGNLexer::TokenType::Comment, 13, "Kasparov schüttelt kurz den Kopf");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "11");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Bf4");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "b5");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "12");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "a4");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Bb7");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "13");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Re1");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Nd5");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "14");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Bg3");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Kc8");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "15");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "axb5");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "cxb5");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "16");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Qd3");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Bc6");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "17");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Bf5");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "exf5");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "18");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Rxe7");
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Bxe7");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "19");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "c4");
    check_token(lexer, PGNLexer::TokenType::GameResult, 13, "1-0");
}
