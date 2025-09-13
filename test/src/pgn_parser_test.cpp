/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include <catch2/catch_all.hpp>

#include "chesscore_io/chesscore_io.h"
#include "chessgame/pgn.h"

#include <algorithm>
#include <sstream>
#include <string>

using namespace chessgame;
using namespace chesscore;

auto count_ply_on_mainline(const chessgame::Game &game) -> int {
    const auto cursor = game.cursor();
    int child_count{0};
    auto next_cursor = cursor.child(0);
    while (next_cursor.has_value()) {
        ++child_count;
        next_cursor = next_cursor.value().child(0);
    }
    return child_count;
}

class GamePath {
public:
    GamePath(std::initializer_list<int> list) : m_path{list} {};
    explicit GamePath(std::vector<int> &&path) : m_path{std::move(path)} {};

    operator std::vector<int>() const { return m_path; }

    auto begin() -> std::vector<int>::iterator { return m_path.begin(); }
    auto end() -> std::vector<int>::iterator { return m_path.end(); }
    auto begin() const -> std::vector<int>::const_iterator { return m_path.begin(); }
    auto end() const -> std::vector<int>::const_iterator { return m_path.end(); }
    auto size() const -> size_t { return m_path.size(); }

    auto operator[](size_t index) const -> int { return m_path[index]; }

    auto operator+=(const GamePath &other) -> GamePath & {
        m_path.insert(m_path.end(), other.m_path.begin(), other.m_path.end());
        return *this;
    }
private:
    std::vector<int> m_path;
};

auto operator+(const GamePath &lhs, const GamePath &rhs) -> GamePath {
    return GamePath{lhs} += rhs;
}

auto mainline(size_t depth) -> GamePath {
    std::vector<int> path(depth);
    std::ranges::fill(path, 0);
    return GamePath{std::move(path)};
}

auto var(int index) -> GamePath {
    return GamePath{{index}};
}

auto get_node(const chessgame::Game &game, const GamePath &path) -> std::shared_ptr<const chessgame::GameNode> {
    auto cursor = game.cursor();
    for (size_t index = 0U; index < path.size(); ++index) {
        const auto child_cursor = cursor.child(path[index]);
        CAPTURE(index);
        REQUIRE(child_cursor.has_value());
        cursor = child_cursor.value();
    }
    return cursor.node();
}

auto get_move(const chessgame::Game &game, const GamePath &path) -> Move {
    return get_node(game, path)->move();
}

auto check_move(const chessgame::Game &game, const GamePath &path, const Move &expected_move) -> void {
    CAPTURE(path);
    const auto &move = get_move(game, path);
    CAPTURE(move);
    CHECK(move == expected_move);
}

auto has_no_following_move(const chessgame::Game &game, const GamePath &path) -> bool {
    CAPTURE(path);
    const auto &node = get_node(game, path);
    return node->child_count() == 0;
}

TEST_CASE("PGN.Parser.Simple Linear Game", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1-0"]

1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 4. Ba4 1-0)";
    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 7);
    check_move(game, mainline(1), Move{.from = Square::E2, .to = Square::E4, .piece = Piece::WhitePawn});
    check_move(game, mainline(2), Move{.from = Square::E7, .to = Square::E5, .piece = Piece::BlackPawn});
    check_move(game, mainline(3), Move{.from = Square::G1, .to = Square::F3, .piece = Piece::WhiteKnight});
    check_move(game, mainline(4), Move{.from = Square::B8, .to = Square::C6, .piece = Piece::BlackKnight});
    check_move(game, mainline(7), Move{.from = Square::B5, .to = Square::A4, .piece = Piece::WhiteBishop});
    CHECK(has_no_following_move(game, mainline(7)));
}

TEST_CASE("PGN.Parser.Game with Comments", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1-0"]

1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 {It was Pillsbury who first demonstrated the
strength of the this move, which today is routine} 4..Be7 5. Nf3 Nbd7 6. Rc1 O-O
7. e3 b6 {In order to develop the Queen Bishop on Bb7. This was the most popular
way of defending the Queen's gambit declined at the time.} 8. cxd5 {Depriving
Black of the opportunity to play dxc4 when the diagonal b7-g2 would be open for
his Queen Bishop.} 8...exd5 9. Bd3 Bb7 10. O-O c5 1-0)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 20);
    check_move(game, mainline(1), Move{.from = Square::D2, .to = Square::D4, .piece = Piece::WhitePawn});
    check_move(game, mainline(2), Move{.from = Square::D7, .to = Square::D5, .piece = Piece::BlackPawn});
    check_move(game, mainline(7), Move{.from = Square::C1, .to = Square::G5, .piece = Piece::WhiteBishop});
    check_move(game, mainline(8), Move{.from = Square::F8, .to = Square::E7, .piece = Piece::BlackBishop});
    check_move(game, mainline(12), Move{.from = Square::E8, .to = Square::G8, .piece = Piece::BlackKing});
    check_move(game, mainline(14), Move{.from = Square::B7, .to = Square::B6, .piece = Piece::BlackPawn});
    check_move(game, mainline(15), Move{.from = Square::C4, .to = Square::D5, .piece = Piece::WhitePawn, .captured = Piece::BlackPawn});
    check_move(game, mainline(16), Move{.from = Square::E6, .to = Square::D5, .piece = Piece::BlackPawn, .captured = Piece::WhitePawn});
    check_move(game, mainline(20), Move{.from = Square::C7, .to = Square::C5, .piece = Piece::BlackPawn});
}

TEST_CASE("PGN.Parser.Game with NAG", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1-0"]

{The active Bishop puts White in a position to start a Kingside attack} 1. e4
e5 2. Nf3 $1 Nc6 3. Bb5 a6 4. Ba4 Nf6 $2 5. O-O Be7 $1 $32 6. Qe2 b5 7. Bb3 O-O 8. c3 8...
d5 9. d3 $1 {An excellent reply, avoiding the complications arising from 9.
exd5 and ensuring White a positional advantage since the opening of the d-file
is in his favour (as he can immediately occupy it) - Alekhine} 1-0
)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 17);
    check_move(game, mainline(3), Move{.from = Square::G1, .to = Square::F3, .piece = Piece::WhiteKnight});
    check_move(game, mainline(4), Move{.from = Square::B8, .to = Square::C6, .piece = Piece::BlackKnight});
    check_move(game, mainline(8), Move{.from = Square::G8, .to = Square::F6, .piece = Piece::BlackKnight});
    check_move(game, mainline(9), Move{.from = Square::E1, .to = Square::G1, .piece = Piece::WhiteKing});
    check_move(game, mainline(10), Move{.from = Square::F8, .to = Square::E7, .piece = Piece::BlackBishop});
    check_move(game, mainline(11), Move{.from = Square::D1, .to = Square::E2, .piece = Piece::WhiteQueen});
}

TEST_CASE("PGN.Parser.Alternative Start", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[SetUp "1"]
[FEN "r4rk1/pp3ppp/2n1q3/8/8/P7/1P3PPP/R1BQ1RK1 w - - 0 1"]
[Result "1-0"]

1. Re1 Rfd8 2. Bd2 Qf5 3. Rc1 Ne5 $1 4. Qc2 Nd3 5. Rf1 1-0)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 9);
    check_move(game, mainline(1), Move{.from = Square::F1, .to = Square::E1, .piece = Piece::WhiteRook});
    check_move(game, mainline(2), Move{.from = Square::F8, .to = Square::D8, .piece = Piece::BlackRook});
    check_move(game, mainline(7), Move{.from = Square::D1, .to = Square::C2, .piece = Piece::WhiteQueen});
    check_move(game, mainline(8), Move{.from = Square::E5, .to = Square::D3, .piece = Piece::BlackKnight});
    check_move(game, mainline(9), Move{.from = Square::E1, .to = Square::F1, .piece = Piece::WhiteRook});
}

TEST_CASE("PGN.Parser.Game with RAV", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1/2-1/2"]

1. d4 Nf6 2. c4 e6 3. Nc3 Bb4 4. a3 Bxc3+ 5. bxc3 c5 6. f3 d5 7. e3 O-O
8. cxd5 Nxd5
9. Bd2 (9. c4 Ne7 10. Bd3 cxd4 11. exd4 Nf5 12. Bxf5 12... Qa5+)
9... Nc6 10. Bd3 cxd4 11. cxd4 e5
12. dxe5 (12. e4 Nf4 13. Bxf4 exf4 14. d5 Qh4+ 15. Kf1 15... Ne5 $36)
         (12. Ne2 12... exd4 13. exd4 Nxd4 14. Nxd4 Qh4+ 15. g3 Qxd4)
12... Nxe5
13. Be4 Nc4 $2 (13... Nf6 $1
    14. Bb4 (14. Bc3 Qc7 15. Qd4 Nxe4 16. fxe4 f6)
            (14. Bc2 Nd3+)
    14... Nxe4 15. Bxf8 Nd3+ 16. Kf1 Nef2 17. Qc2 17... Nxh1 $17)
14. Qc1 Nxd2 15. Qxd2 Nf6 16. Bd3 Re8 17. Ne2 Qb6
18. Nd4 Nd5 (18... Qxd4 $4 19. Bxh7+ Kxh7 20. Qxd4 $18)
19. Be4 Nxe3 1/2-1/2)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 38);
    check_move(game, mainline(1), Move{.from = Square::D2, .to = Square::D4, .piece = Piece::WhitePawn});
    check_move(game, mainline(17), Move{.from = Square::C1, .to = Square::D2, .piece = Piece::WhiteBishop});
    check_move(game, mainline(18), Move{.from = Square::B8, .to = Square::C6, .piece = Piece::BlackKnight});
    check_move(game, mainline(19), Move{.from = Square::F1, .to = Square::D3, .piece = Piece::WhiteBishop});
    check_move(game, mainline(23), Move{.from = Square::D4, .to = Square::E5, .piece = Piece::WhitePawn, .captured = Piece::BlackPawn});
    check_move(game, mainline(24), Move{.from = Square::C6, .to = Square::E5, .piece = Piece::BlackKnight, .captured = Piece::WhitePawn});
    check_move(game, mainline(27), Move{.from = Square::D1, .to = Square::C1, .piece = Piece::WhiteQueen});
    check_move(game, mainline(37), Move{.from = Square::D3, .to = Square::E4, .piece = Piece::WhiteBishop});

    check_move(game, mainline(16) + var(1), Move{.from = Square::C3, .to = Square::C4, .piece = Piece::WhitePawn});
    check_move(game, mainline(16) + var(1) + mainline(1), Move{.from = Square::D5, .to = Square::E7, .piece = Piece::BlackKnight});
    check_move(game, mainline(16) + var(1) + mainline(7), Move{.from = Square::D8, .to = Square::A5, .piece = Piece::BlackQueen});
    CHECK(has_no_following_move(game, mainline(16) + var(1) + mainline(7)));

    check_move(game, mainline(22) + var(1), Move{.from = Square::E3, .to = Square::E4, .piece = Piece::WhitePawn});
    check_move(game, mainline(22) + var(1) + mainline(7), Move{.from = Square::C6, .to = Square::E5, .piece = Piece::BlackKnight});
    CHECK(has_no_following_move(game, mainline(22) + var(1) + mainline(7)));

    check_move(game, mainline(22) + var(2), Move{.from = Square::G1, .to = Square::E2, .piece = Piece::WhiteKnight});
    check_move(game, mainline(22) + var(2) + mainline(7), Move{.from = Square::H4, .to = Square::D4, .piece = Piece::BlackQueen, .captured = Piece::WhiteKnight});
    CHECK(has_no_following_move(game, mainline(22) + var(2) + mainline(7)));

    check_move(game, mainline(25) + var(1), Move{.from = Square::D5, .to = Square::F6, .piece = Piece::BlackKnight});
    check_move(game, mainline(25) + var(1) + mainline(1), Move{.from = Square::D2, .to = Square::B4, .piece = Piece::WhiteBishop});
    check_move(game, mainline(25) + var(1) + var(1), Move{.from = Square::D2, .to = Square::C3, .piece = Piece::WhiteBishop});
    check_move(game, mainline(25) + var(1) + var(1) + mainline(5), Move{.from = Square::F7, .to = Square::F6, .piece = Piece::BlackPawn});
    CHECK(has_no_following_move(game, mainline(25) + var(1) + var(1) + mainline(5)));
    check_move(game, mainline(25) + var(1) + var(2), Move{.from = Square::E4, .to = Square::C2, .piece = Piece::WhiteBishop});
    check_move(game, mainline(25) + var(1) + var(2) + mainline(1), Move{.from = Square::E5, .to = Square::D3, .piece = Piece::BlackKnight});
    CHECK(has_no_following_move(game, mainline(25) + var(1) + var(2) + mainline(1)));
}

TEST_CASE("PGN.Parser.Annotations", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1-0"]

{The active Bishop puts White in a position to start a Kingside attack} 1. e4
e5 2. Nf3 $1 Nc6 3. Bb5 a6 4. Ba4 Nf6 $2 5. O-O Be7 $1 $32 6. Qe2 b5 7. Bb3 O-O 8. c3 8...
d5 9. d3 $1 {An excellent reply, avoiding the complications arising from 9.
exd5 and ensuring White a positional advantage since the opening of the d-file
is in his favour (as he can immediately occupy it) - Alekhine} 1-0
)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    CHECK(count_ply_on_mainline(game) == 17);

    CHECK(game.const_cursor().node()->comment() == "The active Bishop puts White in a position to start a Kingside attack");
    const auto node1 = get_node(game, mainline(3));
    REQUIRE(node1 != nullptr);
    REQUIRE(node1->nags().size() == 1);
    CHECK(node1->nags()[0] == 1);

    const auto node2 = get_node(game, mainline(10));
    REQUIRE(node2 != nullptr);
    REQUIRE(node2->nags().size() == 2);
    CHECK(node2->nags()[0] == 1);
    CHECK(node2->nags()[1] == 32);

    const auto node3 = get_node(game, mainline(17));
    REQUIRE(node3 != nullptr);
    REQUIRE(node3->nags().size() == 1);
    CHECK(node3->nags()[0] == 1);
    CHECK(
        node3->comment() == "An excellent reply, avoiding the complications arising from 9. exd5 and ensuring White a positional advantage since the opening of the d-file is in "
                            "his favour (as he can immediately occupy it) - Alekhine"
    );
}

TEST_CASE("PGN.Parser.Annotated with RAV", "[pgn]") {
    const std::string game_data = R"([Event "Test Event"]
[Site "Test Site"]
[White "Player W"]
[Black "Player B"]
[Result "1-0"]

1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 {Comment 1} 4...Be7 5. Nf3 Nbd7 6. Rc1 O-O
7. e3 b6 {Comment 2} 8. cxd5 exd5 {Comment 3} ({Comment 4} 8...
Nf6xd5 {Comment 5} 9. Bxe7, Qxe7 10. Nxd5, e6xd5) 9. Bd3 {Comment 6} Bb7 1-0
)";

    std::istringstream pgn_data{game_data};
    auto parser = chessgame::PGNParser{pgn_data};
    auto opt_game = parser.read_game();
    REQUIRE(opt_game.has_value());
    const auto &game = opt_game.value();

    const auto node1 = get_node(game, mainline(7));
    CHECK(node1->comment() == "Comment 1");
    CHECK(node1->premove_comment().empty());
    const auto node2 = get_node(game, mainline(8));
    CHECK(node2->comment().empty());
    CHECK(node2->premove_comment().empty());
    const auto node3 = get_node(game, mainline(14));
    CHECK(node3->comment() == "Comment 2");
    const auto node4 = get_node(game, mainline(16));
    CHECK(node4->comment() == "Comment 3");
    const auto node5 = get_node(game, mainline(15) + var(1));
    CHECK(node5->premove_comment() == "Comment 4");
    CHECK(node5->comment() == "Comment 5");
    const auto node6 = get_node(game, mainline(15) + var(1) + mainline(1));
    CHECK(node6->premove_comment().empty());
    CHECK(node6->comment().empty());
    const auto node7 = get_node(game, mainline(17));
    CHECK(node7->comment() == "Comment 6");
}
