/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/pgn.h"

#include <istream>

namespace chessgame {

auto PGNLexer::next_token() -> std::expected<Token, PGNError> {
    if (!m_in_stream) {
        return std::unexpected(PGNError{.type = PGNErrorType::EndOfInput, .line = m_line_number});
    }
    return {};
}

} // namespace chessgame
