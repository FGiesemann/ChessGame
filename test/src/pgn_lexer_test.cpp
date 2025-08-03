/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chessgame/pgn.h"

#include <string>

using namespace chessgame;

auto check_token(PGNLexer &lexer, PGNLexer::TokenType expected_type, int line = -1, std::string expected_value = "") -> void {
    auto token = lexer.next_token();
    CAPTURE(expected_type);
    CAPTURE(line);
    CAPTURE(expected_value);
    REQUIRE(token.has_value());
    CHECK(token->type == expected_type);
    if (line != -1) {
        CHECK(token->line == line);
    }
    CHECK(token->value == expected_value);
}

auto check_tag(PGNLexer &lexer, const std::string &expected_name, const std::string &expected_value, int line) -> void {
    CAPTURE(expected_name);
    CAPTURE(expected_value);
    check_token(lexer, PGNLexer::TokenType::OpenBracket, line);
    check_token(lexer, PGNLexer::TokenType::Name, line, expected_name);
    check_token(lexer, PGNLexer::TokenType::String, line, expected_value);
    check_token(lexer, PGNLexer::TokenType::CloseBracket, line);
}

auto check_full_move(PGNLexer &lexer, int number, const std::string &white_move, const std::string &black_move) -> void {
    CAPTURE(number);
    CAPTURE(white_move);
    CAPTURE(black_move);
    check_token(lexer, PGNLexer::TokenType::Number, -1, std::to_string(number));
    check_token(lexer, PGNLexer::TokenType::Dot);
    check_token(lexer, PGNLexer::TokenType::Name, -1, white_move);
    check_token(lexer, PGNLexer::TokenType::Name, -1, black_move);
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
    auto lexer = PGNLexer{pgn_stream};

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

    check_full_move(lexer, 1, "e4", "c6");
    check_full_move(lexer, 2, "d4", "d5");
    check_full_move(lexer, 3, "Nc3", "dxe4");
    check_full_move(lexer, 4, "Nxe4", "Nd7");
    check_full_move(lexer, 5, "Ng5", "Ngf6");
    check_full_move(lexer, 6, "Bd3", "e6");
    check_full_move(lexer, 7, "N1f3", "h6");
    check_full_move(lexer, 8, "Nxe6", "Qe7");
    check_full_move(lexer, 9, "O-O", "fxe6");
    check_full_move(lexer, 10, "Bg6+", "Kd8");
    check_token(lexer, PGNLexer::TokenType::Comment, 13, "Kasparov schüttelt kurz den Kopf");
    check_full_move(lexer, 11, "Bf4", "b5");
    check_full_move(lexer, 12, "a4", "Bb7");
    check_full_move(lexer, 13, "Re1", "Nd5");
    check_full_move(lexer, 14, "Bg3", "Kc8");
    check_full_move(lexer, 15, "axb5", "cxb5");
    check_full_move(lexer, 16, "Qd3", "Bc6");
    check_full_move(lexer, 17, "Bf5", "exf5");
    check_full_move(lexer, 18, "Rxe7", "Bxe7");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "19");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "c4");
    check_token(lexer, PGNLexer::TokenType::GameResult, 13, "1-0");
}

TEST_CASE("PGN.Lexer.Game with variations", "[pgn]") {
    const std::string pgn_data{
        "[Event \"Yugoslavian Club Championship\"]\n"
        "[Site \"Vrnjacka-Banja\"]\n"
        "[Date \"1999\"]\n"
        "[White \"Beliavsky, Alexander\"]\n"
        "[Black \"Sakaev, Konstantin\"]\n"
        "[WhiteELO \"2618\"]\n"
        "[BlackELO \"2648\"]\n"
        "[Result \"1/2-1/2\"]\n"
        "[Source \"Russian Chess\"]\n"
        "[Annotator \"GM Konstantin Sakaev\"]\n"
        " \n"
        "1. d4 d5 2. c4 dxc4 3. e4 Nf6 4. e5 Nd5 5. Bxc4 Nb6 6. Bb3 Nc6 7. Ne2 Bf5 8. a3 e6 9. Nbc3 Qd7 10. O-O O-O-O 11. Be3 h5 $1 \n"
        "(11...f6 12. exf6 gxf6 13. Re1 $1 $14)\n"
        "24. Rfe1\n"
        "({Better is} 24. f3 $1 Re2 25. Rf2 Rde8 26. Kf1 R2e6 $15)\n"
        "24...Re4 25. Rxe4 fxe4 26. Re1 Re8 27. g3 hxg3 28. fxg3 f5 29. Rf1 Rf8 30. Kf2 Kd7 31. Ke2 Ke6 32. Bg5 Rg8 33. h4 c6 34. Rc1 $6 Kf7 $1 35. Kf2 Kg6 36. Kg2 Kh5\n"
        "$15\n"
        "(34...a6 $6) \n"
        "1/2-1/2 \n"
    };
    auto pgn_stream = std::istringstream{pgn_data};
    auto lexer = PGNLexer{pgn_stream};

    check_tag(lexer, "Event", "Yugoslavian Club Championship", 1);
    check_tag(lexer, "Site", "Vrnjacka-Banja", 2);
    check_tag(lexer, "Date", "1999", 3);
    check_tag(lexer, "White", "Beliavsky, Alexander", 4);
    check_tag(lexer, "Black", "Sakaev, Konstantin", 5);
    check_tag(lexer, "WhiteELO", "2618", 6);
    check_tag(lexer, "BlackELO", "2648", 7);
    check_tag(lexer, "Result", "1/2-1/2", 8);
    check_tag(lexer, "Source", "Russian Chess", 9);
    check_tag(lexer, "Annotator", "GM Konstantin Sakaev", 10);

    check_full_move(lexer, 1, "d4", "d5");
    check_full_move(lexer, 2, "c4", "dxc4");
    check_full_move(lexer, 3, "e4", "Nf6");
    check_full_move(lexer, 4, "e5", "Nd5");
    check_full_move(lexer, 5, "Bxc4", "Nb6");
    check_full_move(lexer, 6, "Bb3", "Nc6");
    check_full_move(lexer, 7, "Ne2", "Bf5");
    check_full_move(lexer, 8, "a3", "e6");
    check_full_move(lexer, 9, "Nbc3", "Qd7");
    check_full_move(lexer, 10, "O-O", "O-O-O");
    check_full_move(lexer, 11, "Be3", "h5");
    check_token(lexer, PGNLexer::TokenType::NAG, 12, "1");
    check_token(lexer, PGNLexer::TokenType::OpenParen, 13);
    check_token(lexer, PGNLexer::TokenType::Number, 13, "11");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "f6");
    check_full_move(lexer, 12, "exf6", "gxf6");
    check_token(lexer, PGNLexer::TokenType::Number, 13, "13");
    check_token(lexer, PGNLexer::TokenType::Dot, 13);
    check_token(lexer, PGNLexer::TokenType::Name, 13, "Re1");
    check_token(lexer, PGNLexer::TokenType::NAG, 13, "1");
    check_token(lexer, PGNLexer::TokenType::NAG, 13, "14");
    check_token(lexer, PGNLexer::TokenType::CloseParen, 13);
    check_token(lexer, PGNLexer::TokenType::Number, 14, "24");
    check_token(lexer, PGNLexer::TokenType::Dot, 14);
    check_token(lexer, PGNLexer::TokenType::Name, 14, "Rfe1");
    //({Better is} 24. f3 $1 Re2
    check_token(lexer, PGNLexer::TokenType::OpenParen, 15);
    check_token(lexer, PGNLexer::TokenType::Comment, 15, "Better is");
    check_token(lexer, PGNLexer::TokenType::Number, 15, "24");
    check_token(lexer, PGNLexer::TokenType::Dot, 15);
    check_token(lexer, PGNLexer::TokenType::Name, 15, "f3");
    check_token(lexer, PGNLexer::TokenType::NAG, 15, "1");
    check_token(lexer, PGNLexer::TokenType::Name, 15, "Re2");
}
