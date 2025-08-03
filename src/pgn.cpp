/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/pgn.h"

#include <cctype>
#include <istream>

namespace chessgame {

auto PGNLexer::next_token() -> std::expected<Token, PGNError> {
    int c = m_in_stream.get();
    if (is_whitespace(static_cast<char>(c))) {
        skip_whitespace(c);
        c = m_in_stream.get();
    }
    if (m_in_stream) {
        if (std::isdigit(c) != 0) {
            return read_token_starting_with_number(static_cast<char>(c));
        }
        if (std::isalpha(c) != 0) {
            return read_name(static_cast<char>(c));
        }
        switch (c) {
        case '[':
            return Token{.type = TokenType::OpenBracket, .line = m_line_number};
        case ']':
            return Token{.type = TokenType::CloseBracket, .line = m_line_number};
        case '$':
            return read_nag();
        case '.':
            return Token{.type = TokenType::Dot, .line = m_line_number};
        case '"':
            return read_string();
        case '(':
            return Token{.type = TokenType::OpenParen, .line = m_line_number};
        case ')':
            return Token{.type = TokenType::CloseParen, .line = m_line_number};
        case '{':
            return read_comment();
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

auto PGNLexer::is_whitespace(char c) -> bool {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

auto PGNLexer::skip_whitespace(int c) -> void {
    while (m_in_stream && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
        if (c == '\n') {
            m_line_number++;
        }
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

auto PGNLexer::read_token_starting_with_number(char first_c) -> std::expected<Token, PGNError> {
    std::string result{first_c};
    int c = m_in_stream.get();
    bool only_numbers = true;
    while (m_in_stream) {
        while (m_in_stream && (std::isdigit(c) != 0)) {
            result += static_cast<char>(c);
            c = m_in_stream.get();
        }
        if (!m_in_stream) {
            if (m_in_stream.eof()) {
                break;
            }
            return std::unexpected(PGNError{.type = PGNErrorType::InputError, .line = m_line_number});
        }
        if (c == '/' || c == '-') {
            only_numbers = false;
            result += static_cast<char>(c);
            c = m_in_stream.get();
        } else {
            break;
        }
    }
    m_in_stream.unget();
    return Token{.type = only_numbers ? TokenType::Number : TokenType::GameResult, .line = m_line_number, .value = result};
}

auto PGNLexer::read_name(char first_c) -> std::expected<Token, PGNError> {
    std::string result{first_c};
    int c = m_in_stream.get();
    while (m_in_stream && !is_whitespace(static_cast<char>(c))) {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        return std::unexpected(PGNError{.type = PGNErrorType::InputError, .line = m_line_number});
    }
    m_in_stream.unget();
    return Token{.type = TokenType::Name, .line = m_line_number, .value = result};
}

auto PGNLexer::read_comment() -> std::expected<Token, PGNError> {
    std::string result{};
    int c = m_in_stream.get();
    while (m_in_stream && c != '}') {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        return std::unexpected(PGNError{.type = PGNErrorType::InputError, .line = m_line_number});
    }
    return Token{.type = TokenType::Comment, .line = m_line_number, .value = result};
}

auto PGNLexer::read_nag() -> std::expected<Token, PGNError> {
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
    return Token{.type = TokenType::NAG, .line = m_line_number, .value = result};
}

} // namespace chessgame
