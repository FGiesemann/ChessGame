/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/pgn.h"
#include "chessgame/move_matcher.h"
#include "chessgame/san.h"

#include <cctype>
#include <istream>

namespace chessgame {

auto PGNLexer::next_token() -> Token {
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
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    throw PGNError{PGNErrorType::UnexpectedChar, m_line_number, std::string{static_cast<char>(c)}};
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

auto PGNLexer::read_string() -> Token {
    std::string result{};
    int c = m_in_stream.get();
    while (m_in_stream && c != '"') {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    return Token{.type = TokenType::String, .line = m_line_number, .value = result};
}

auto PGNLexer::read_token_starting_with_number(char first_c) -> Token {
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
            throw PGNError{PGNErrorType::InputError, m_line_number};
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

auto PGNLexer::read_name(char first_c) -> Token {
    std::string result{first_c};
    int c = m_in_stream.get();
    while (m_in_stream && !is_whitespace(static_cast<char>(c))) {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    m_in_stream.unget();
    return Token{.type = TokenType::Name, .line = m_line_number, .value = result};
}

auto PGNLexer::read_comment() -> Token {
    std::string result{};
    int c = m_in_stream.get();
    while (m_in_stream && c != '}') {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    return Token{.type = TokenType::Comment, .line = m_line_number, .value = result};
}

auto PGNLexer::read_nag() -> Token {
    std::string result{};
    int c = m_in_stream.get();
    while (m_in_stream && (std::isdigit(c) != 0)) {
        result += static_cast<char>(c);
        c = m_in_stream.get();
    }
    if (!m_in_stream) {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    m_in_stream.unget();
    return Token{.type = TokenType::NAG, .line = m_line_number, .value = result};
}

auto PGNParser::reset() -> void {
    m_game = Game{};
    clear_cursor_stack();
    m_cursors.push(m_game.edit());
}

auto PGNParser::read_game() -> std::optional<Game> {
    reset();
    expect_token(PGNLexer::TokenType::OpenBracket, "Metadata tags expected");
    read_metadata();
    read_movetext();
    return m_game;
}

auto PGNParser::read_movetext() -> void {
    switch (m_token.type) {
    case PGNLexer::TokenType::Number:
    case PGNLexer::TokenType::Name:
        read_move();
        break;
    default:
        throw PGNError(PGNErrorType::UnexpectedToken, m_token.line, "Move expected");
    }
}

auto PGNParser::read_move() -> void {
    std::optional<int> number;
    if (m_token.type == PGNLexer::TokenType::Number) {
        number = std::stoi(m_token.value);
        // TODO: check for "continuation", e.g. 1...e5 -> black move
        skip_tokens(PGNLexer::TokenType::Dot);
    }
    check_token_type(PGNLexer::TokenType::Name, "Move expected");
    process_move(m_token.value, chesscore::Color::White, number);

    // TODO check black half-move
}

auto PGNParser::read_metadata() -> void {
    while (m_token.type == PGNLexer::TokenType::OpenBracket) {
        read_tag();
        next_token();
    }
    if (m_token.type == PGNLexer::TokenType::Comment) {
        m_game.metadata().overall_comment = m_token.value;
        next_token();
    }
}

auto PGNParser::read_tag() -> void {
    expect_token(PGNLexer::TokenType::Name, "Name expected");
    const auto tag_name = m_token.value;
    expect_token(PGNLexer::TokenType::String, "String expected");
    const auto tag_value = m_token.value;
    m_game.set_tag(tag_name, tag_value);
    expect_token(PGNLexer::TokenType::CloseBracket, "Close bracket expected");
}

auto PGNParser::process_move(const std::string &san_str, chesscore::Color side_to_move, [[maybe_unused]] std::optional<int> number) -> void {
    // TODO: check move number, if provided
    const auto exp_san = parse_san(san_str, side_to_move);
    if (!exp_san.has_value()) {
        throw PGNError(PGNErrorType::InvalidMove, m_token.line, exp_san.error().san);
    }
    const auto &san_move = exp_san.value();
    const auto legal_moves = current_game_line().position().all_legal_moves();
    const auto matched_moves = match_san_move(san_move, legal_moves);
    if (matched_moves.size() == 0) {
        throw PGNError(PGNErrorType::IllegalMove, m_token.line, san_str);
    }
    if (matched_moves.size() > 1) {
        throw PGNError(PGNErrorType::AmbiguousMove, m_token.line, san_str);
    }
    const auto move = matched_moves.front();
    current_game_line() = current_game_line().play_move(move);
}

auto PGNParser::next_token() -> void {
    m_token = m_lexer.next_token();
}

auto PGNParser::check_token_type(PGNLexer::TokenType expected_type, const std::string &error_message) const -> void {
    if (m_token.type != expected_type) {
        throw PGNError{PGNErrorType::UnexpectedToken, m_token.line, error_message};
    }
}

auto PGNParser::expect_token(PGNLexer::TokenType expected_type, const std::string &error_message) -> void {
    next_token();
    check_token_type(expected_type, error_message);
}

auto PGNParser::skip_tokens(PGNLexer::TokenType type) -> void {
    next_token();
    while (m_token.type == type) {
        next_token();
    }
}

auto PGNParser::clear_cursor_stack() -> void {
    while (!m_cursors.empty()) {
        m_cursors.pop();
    }
}

} // namespace chessgame
