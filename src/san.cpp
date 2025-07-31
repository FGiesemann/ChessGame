/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/san.h"

#include <chesscore/piece.h>
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
    SuffixAnnotation,
    Invalid,
};

struct SANToken {
    TokenType type{TokenType::Invalid};
    std::string_view value;
};

auto get_token(std::string_view san_str) -> SANToken {
    if (san_str.empty()) {
        return {.type = TokenType::Invalid, .value = ""};
    }
    char character = san_str[0];
    switch (character) {
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
    case '!':
    case '?': {
        if (san_str.size() < 2) {
            return {.type = TokenType::SuffixAnnotation, .value = san_str.substr(0, 1)};
        }
        if (san_str[1] == '!' || san_str[1] == '?') {
            return {.type = TokenType::SuffixAnnotation, .value = san_str.substr(0, 2)};
        }
        return {.type = TokenType::SuffixAnnotation, .value = san_str.substr(0, 2)};
    }
    default:
        return {.type = TokenType::Invalid, .value = ""};
    }
}

auto extract_rank(std::string_view str) -> chesscore::Rank {
    return chesscore::Rank{str[0] - '0'};
}

auto get_suffix_annotation(std::string_view str) -> SuffixAnnotation {
    if (str == "!") {
        return SuffixAnnotation::GoodMove;
    }
    if (str == "!!") {
        return SuffixAnnotation::VeryGoodMove;
    }
    if (str == "?") {
        return SuffixAnnotation::PoorMove;
    }
    if (str == "??") {
        return SuffixAnnotation::VeryPoorMove;
    }
    if (str == "!?") {
        return SuffixAnnotation::SpeculativeMove;
    }
    if (str == "?!") {
        return SuffixAnnotation::QuestionableMove;
    }
    throw InvalidSAN("Invalid suffix annotation '" + std::string{str} + "'");
}

void parse_suffixes(const std::string &san, SANMove &move, std::string_view &san_str, SANToken &token) {
    if (token.type == TokenType::Check) {
        move.check_state = CheckState::Check;
        san_str = san_str.substr(1);
        token = get_token(san_str);
    }
    if (token.type == TokenType::Checkmate) {
        if (move.check_state != CheckState::None) {
            throw InvalidSAN("Check and checkmate are mutually exclusive in SAN string '" + san + "'");
        }
        move.check_state = CheckState::Checkmate;
        san_str = san_str.substr(1);
        token = get_token(san_str);
    }
    if (token.type == TokenType::Check) {
        if (move.check_state != CheckState::None) {
            throw InvalidSAN("Check and checkmate are mutually exclusive in SAN string '" + san + "'");
        }
        move.check_state = CheckState::Check;
        san_str = san_str.substr(1);
        token = get_token(san_str);
    }
    if (token.type == TokenType::SuffixAnnotation) {
        move.suffix_annotation = get_suffix_annotation(token.value);
        san_str = san_str.substr(token.value.length());
    }
}

void parse_promotions(const std::string &san, chesscore::Color &side_to_move, SANMove &move, std::string_view &san_str, SANToken &token) {
    if (token.type == TokenType::Promotion) {
        san_str = san_str.substr(1);
        token = get_token(san_str);
        if (token.type != TokenType::PieceType) {
            throw InvalidSAN("Expected promotion piece, but not found in SAN string '" + san + "'");
        }
        move.promotion = chesscore::Piece{.type = chesscore::piece_type_from_char(token.value[0]), .color = side_to_move};
        san_str = san_str.substr(1);
        token = get_token(san_str);
    }
}

void parse_piece_type(chesscore::Color &side_to_move, SANMove &move, std::string_view &san_str, SANToken &token) {
    if (token.type == TokenType::PieceType) {
        move.moving_piece = chesscore::Piece{.type = chesscore::piece_type_from_char(token.value[0]), .color = side_to_move};
        san_str = san_str.substr(1);
        token = get_token(san_str);
    } else {
        move.moving_piece = chesscore::Piece{.type = chesscore::PieceType::Pawn, .color = side_to_move};
    }
}

void parse_disambiguation_chars(SANMove &move, std::string_view &san_str, SANToken &token) {
    if (token.type == TokenType::File) {
        const auto next_token = get_token(san_str.substr(1));
        if (next_token.type != TokenType::Rank) {
            move.disambiguation_file = chesscore::File{token.value[0]};
            san_str = san_str.substr(1);
            token = get_token(san_str);
        }
    } else if (token.type == TokenType::Rank) {
        move.disambiguation_rank = extract_rank(token.value);
        san_str = san_str.substr(1);
        token = get_token(san_str);
    }
}

void parse_capture(SANMove &move, std::string_view &san_str, SANToken &token) {
    if (token.type == TokenType::Capturing) {
        move.capturing = true;
        san_str = san_str.substr(1);
        token = get_token(san_str);
    }
}

void parse_target_square(const std::string &san, SANMove &move, std::string_view &san_str, SANToken &token) {
    if (token.type == TokenType::File) {
        chesscore::File to_file{token.value[0]};
        const auto rank_token = get_token(san_str.substr(1));
        if (rank_token.type == TokenType::Rank) {
            move.target_square = chesscore::Square{to_file, extract_rank(rank_token.value)};
            san_str = san_str.substr(2);
            token = get_token(san_str);
        } else {
            throw InvalidSAN("Expected rank, but not found in SAN string '" + san + "'");
        }
    } else {
        throw InvalidSAN("Expected file, but not found in SAN string '" + san + "'");
    }
}

const std::string long_castling{"O-O-O"};
const std::string short_castling{"O-O"};

auto parse_castling_move(const std::string &san, chesscore::Color side_to_move, SANMove &move, std::string_view san_str) -> void {
    move.moving_piece = chesscore::Piece{.type = chesscore::PieceType::King, .color = side_to_move};
    chesscore::Square target_square;
    SANToken token;
    if (san_str.starts_with(long_castling)) {
        target_square = side_to_move == chesscore::Color::White ? chesscore::Square::C1 : chesscore::Square::C8;
        san_str = san_str.substr(long_castling.length());
    } else {
        target_square = side_to_move == chesscore::Color::White ? chesscore::Square::G1 : chesscore::Square::G8;
        san_str = san_str.substr(short_castling.length());
    }
    token = get_token(san_str);
    parse_suffixes(san, move, san_str, token);
    move.target_square = target_square;
}

} // namespace

auto parse_san(const std::string &san, chesscore::Color side_to_move) -> SANMove {
    SANMove move;
    move.san_string = san;
    std::string_view san_str{san};
    if (san_str.starts_with(short_castling)) {
        parse_castling_move(san, side_to_move, move, san_str);
        return move;
    }

    auto token = get_token(san_str);
    if (token.type == TokenType::Invalid) {
        throw InvalidSAN("Could not parse SAN string '" + san + "'");
    }

    parse_piece_type(side_to_move, move, san_str, token);
    parse_disambiguation_chars(move, san_str, token);
    parse_capture(move, san_str, token);
    parse_target_square(san, move, san_str, token);
    parse_promotions(san, side_to_move, move, san_str, token);
    parse_suffixes(san, move, san_str, token);

    if (!san_str.empty()) {
        throw InvalidSAN("Unexpected characters at end of SAN string '" + san + "'");
    }

    return move;
}

} // namespace chessgame
