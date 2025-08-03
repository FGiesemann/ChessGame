/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/pgn.h"

#include <istream>

namespace chessgame {

auto PGNLexer::next_token() -> std::expected<Token, PGNError> {
    int c = m_in_stream.get();
    if (m_in_stream) {
        if (std::isdigit(c) != 0) {
            return read_number();
        }
        if (std::isalpha(c) != 0) {
            return read_name();
        }
        switch (c) {
        case '[':
            return Token{.type = TokenType::OpenBracket, .line = m_line_number};
        case ']':
            return Token{.type = TokenType::CloseBracket, .line = m_line_number};
        case '\n':
        case '\r':
            m_line_number++;
            skip_whitespace();
            break;
        case '.':
            return Token{.type = TokenType::Dot, .line = m_line_number};
        case '"':
            return read_string();
        case '(':
            return Token{.type = TokenType::OpenParen, .line = m_line_number};
        case ')':
            return Token{.type = TokenType::CloseParen, .line = m_line_number};
        case '{':
            return Token{.type = TokenType::OpenBrace, .line = m_line_number};
        case '}':
            return Token{.type = TokenType::CloseBrace, .line = m_line_number};
        default:
            break;
        }
    } else {
        if (m_in_stream.eof()) {
            return Token{.type = TokenType::EndOfInput, .line = m_line_number};
        }
        return std::unexpected(PGNError{.type = PGNErrorType::InputError, .line = m_line_number});
    }
    return std::unexpected(PGNError{.type = PGNErrorType::UnexpectedChar, .line = m_line_number, .message = std::string{static_cast<char>(c)}});
}

auto PGNLexer::skip_whitespace() -> void {
    int c = m_in_stream.get();
    while (m_in_stream && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
        c = m_in_stream.get();
    }
    m_in_stream.unget();
}

auto PGNLexer::read_string() -> std::expected<Token, PGNError> {
    std::string result{};
    int c = m_in_stream.get();
    while (m_in_stream && c != '"') {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        return std::unexpected(PGNError{.type = PGNErrorType::InputError, .line = m_line_number});
    }
    return Token{.type = TokenType::String, .line = m_line_number, .value = result};
}

auto PGNLexer::read_number() -> std::expected<Token, PGNError> {
    std::string result{};
    int c = m_in_stream.get();
    while (m_in_stream && (std::isdigit(c) != 0)) {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        return std::unexpected(PGNError{.type = PGNErrorType::InputError, .line = m_line_number});
    }
    m_in_stream.unget();
    return Token{.type = TokenType::Number, .line = m_line_number, .value = result};
}

auto PGNLexer::read_name() -> std::expected<Token, PGNError> {
    std::string result{};
    int c = m_in_stream.get();
    while (m_in_stream && (std::isalpha(c) != 0)) {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        return std::unexpected(PGNError{.type = PGNErrorType::InputError, .line = m_line_number});
    }
    m_in_stream.unget();
    return Token{.type = TokenType::Name, .line = m_line_number, .value = result};
}

} // namespace chessgame
