/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/san.h"

namespace chessgame {

auto parse_san(const std::string &san, chesscore::Color side_to_move) -> SANMove {
    if (san == "O-O") {
        const auto target_square = side_to_move == chesscore::Color::White ? chesscore::Square::G1 : chesscore::Square::G8;
        return SANMove{san, chesscore::Piece{.type = chesscore::PieceType::King, .color = side_to_move}, target_square, CheckState::None};
    }
    if (san == "O-O-O") {
        const auto target_square = side_to_move == chesscore::Color::White ? chesscore::Square::C1 : chesscore::Square::C8;
        return SANMove{san, chesscore::Piece{.type = chesscore::PieceType::King, .color = side_to_move}, target_square, CheckState::None};
    }

    return SANMove{san, chesscore::Piece::WhitePawn, chesscore::Square::A1, CheckState::None};
}

} // namespace chessgame
