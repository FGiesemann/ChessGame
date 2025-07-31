/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/san.h"

#include <chesscore/piece.h>
#include <cstddef>
#include <string_view>

namespace chessgame {

namespace {

enum class TokenType {
    PieceType,
    File,
    Rank,
    Capturing,
    Check,
    Checkmate,
    Promotion,
    Invalid,
};

struct SANToken {
    TokenType type;
    std::string_view value;
};

auto get_token(std::string_view san_str) -> SANToken {
    if (san_str.empty()) {
        return {.type = TokenType::Invalid, .value = ""};
    }
    char c = san_str[0];
    switch (c) {
    case 'P':
    case 'R':
    case 'N':
    case 'B':
    case 'Q':
    case 'K':
        return {.type = TokenType::PieceType, .value = san_str.substr(0, 1)};
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
        return {.type = TokenType::File, .value = san_str.substr(0, 1)};
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
        return {.type = TokenType::Rank, .value = san_str.substr(0, 1)};
    case 'x':
        return {.type = TokenType::Capturing, .value = ""};
    case '+':
        return {.type = TokenType::Check, .value = ""};
    case '#':
        return {.type = TokenType::Checkmate, .value = ""};
    case '=':
        return {.type = TokenType::Promotion, .value = ""};
    default:
        return {.type = TokenType::Invalid, .value = ""};
    }
}

auto extract_rank(std::string_view str) -> chesscore::Rank {
    return chesscore::Rank{str[0] - '0'};
}

auto get_check_state(bool check, bool checkmate) -> CheckState {
    if (check && checkmate) {
        throw InvalidSAN("Check and checkmate are mutually exclusive");
    }
    if (check) {
        return CheckState::Check;
    }
    if (checkmate) {
        return CheckState::Checkmate;
    }
    return CheckState::None;
}

} // namespace

auto parse_san(const std::string &san, chesscore::Color side_to_move) -> SANMove {
    if (san == "O-O") {
        const auto target_square = side_to_move == chesscore::Color::White ? chesscore::Square::G1 : chesscore::Square::G8;
        return SANMove{san, chesscore::Piece{.type = chesscore::PieceType::King, .color = side_to_move}, target_square, false, std::nullopt, CheckState::None};
    }
    if (san == "O-O-O") {
        const auto target_square = side_to_move == chesscore::Color::White ? chesscore::Square::C1 : chesscore::Square::C8;
        return SANMove{san, chesscore::Piece{.type = chesscore::PieceType::King, .color = side_to_move}, target_square, false, std::nullopt, CheckState::None};
    }

    std::string_view san_str{san};
    auto token = get_token(san_str);
    if (token.type == TokenType::Invalid) {
        throw InvalidSAN("Could not parse SAN string '" + san + "'");
    }

    std::optional<chesscore::Piece> piece{{.type = chesscore::PieceType::Pawn, .color = side_to_move}};
    std::optional<chesscore::File> from_file;
    std::optional<chesscore::Rank> from_rank;
    bool capturing = false;
    std::optional<chesscore::Square> target_square;
    std::optional<chesscore::Piece> promotion;
    bool check = false;
    bool checkmate = false;
    if (token.type == TokenType::PieceType) {
        piece = chesscore::Piece{.type = chesscore::piece_type_from_char(token.value[0]), .color = side_to_move};
        san_str = san_str.substr(1);
    } else if (token.type == TokenType::File) {
        const auto next_token = get_token(san_str.substr(1));
        if (next_token.type != TokenType::Rank) {
            from_file = chesscore::File{token.value[0]};
            san_str = san_str.substr(1);
        }
    } else if (token.type == TokenType::Rank) {
        from_rank = token.value[0];
        san_str = san_str.substr(1);
    }
    token = get_token(san_str);
    if (token.type == TokenType::Capturing) {
        capturing = true;
        san_str = san_str.substr(1);
        token = get_token(san_str);
    }
    if (token.type == TokenType::File) {
        chesscore::File to_file{token.value[0]};
        const auto rank_token = get_token(san_str.substr(1));
        if (rank_token.type == TokenType::Rank) {
            target_square = chesscore::Square{to_file, extract_rank(rank_token.value)};
            san_str = san_str.substr(2);
        } else {
            throw InvalidSAN("Expected rank, but not found in SAN string '" + san + "'");
        }
    } else {
        throw InvalidSAN("Expected file, but not found in SAN string '" + san + "'");
    }
    token = get_token(san_str);
    if (token.type == TokenType::Promotion) {
        san_str = san_str.substr(1);
        token = get_token(san_str);
        if (token.type != TokenType::PieceType) {
            throw InvalidSAN("Expected promotion piece, but not found in SAN string '" + san + "'");
        }
        promotion = chesscore::Piece{.type = chesscore::piece_type_from_char(token.value[0]), .color = side_to_move};
        san_str = san_str.substr(1);
        token = get_token(san_str);
    }
    if (token.type == TokenType::Check) {
        check = true;
        san_str = san_str.substr(1);
    }
    if (token.type == TokenType::Checkmate) {
        checkmate = true;
        san_str = san_str.substr(1);
    }

    if (!san_str.empty()) {
        throw InvalidSAN("Unexpected characters at end of SAN string '" + san + "'");
    }

    if (!target_square.has_value()) {
        throw InvalidSAN("No target square found in SAN string '" + san + "'");
    }

    if (from_file.has_value()) {
        return SANMove{san, *piece, *target_square, capturing, promotion, get_check_state(check, checkmate), *from_file};
    }
    if (from_rank.has_value()) {
        return SANMove{san, *piece, *target_square, capturing, promotion, get_check_state(check, checkmate), *from_rank};
    }
    return SANMove{san, *piece, *target_square, capturing, promotion, get_check_state(check, checkmate)};
}

} // namespace chessgame
