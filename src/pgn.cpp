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

auto to_string(PGNErrorType type) -> std::string {
    switch (type) {
    case PGNErrorType::InputError:
        return "input error";
    case PGNErrorType::UnexpectedChar:
        return "unexpected character";
    case PGNErrorType::UnexpectedToken:
        return "unexpected token";
    case PGNErrorType::InvalidMove:
        return "invalid move";
    case PGNErrorType::IllegalMove:
        return "illegal move";
    case PGNErrorType::AmbiguousMove:
        return "ambiguous move";
    case PGNErrorType::EndOfInput:
        return "end of input";
    default:
        return "UNKNOWN ERROR!";
    }
}

auto PGNLexer::next_token() -> Token {
    int character = m_in_stream->get();
    if (is_whitespace(static_cast<char>(character))) {
        skip_whitespace(character);
        character = m_in_stream->get();
    }
    if (!m_in_stream->bad()) {
        if (character == std::istream::traits_type::eof()) {
            return Token{.type = TokenType::EndOfInput, .line = m_line_number, .value = ""};
        }
        if (std::isdigit(character) != 0) {
            return read_token_starting_with_number(static_cast<char>(character));
        }
        if (std::isalpha(character) != 0) {
            return read_symbol(static_cast<char>(character));
        }
        switch (character) {
        case '[':
            return Token{.type = TokenType::OpenBracket, .line = m_line_number, .value = ""};
        case ']':
            return Token{.type = TokenType::CloseBracket, .line = m_line_number, .value = ""};
        case '$':
            return read_nag();
        case '.':
            return Token{.type = TokenType::Dot, .line = m_line_number, .value = ""};
        case '"':
            return read_string();
        case '(':
            return Token{.type = TokenType::OpenParen, .line = m_line_number, .value = ""};
        case ')':
            return Token{.type = TokenType::CloseParen, .line = m_line_number, .value = ""};
        case '{':
            return read_comment();
        default:
            break;
        }
    } else {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    throw PGNError{PGNErrorType::UnexpectedChar, m_line_number, std::string{static_cast<char>(character)}};
}

auto PGNLexer::is_whitespace(char character) -> bool {
    return character == ' ' || character == '\t' || character == '\n' || character == '\r';
}

auto PGNLexer::skip_whitespace(int character) -> void {
    while (!m_in_stream->bad() && (character == ' ' || character == '\t' || character == '\n' || character == '\r')) {
        if (character == '\n') {
            m_line_number++;
        }
        character = m_in_stream->get();
    }
    if (!m_in_stream->bad()) {
        m_in_stream->unget();
    } else {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
}

auto PGNLexer::read_string() -> Token {
    std::string result{};
    int character = m_in_stream->get();
    while (!m_in_stream->bad() && character != '"') {
        result += static_cast<char>(character);
        character = m_in_stream->get();
    }
    if (m_in_stream->bad()) {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    return Token{.type = TokenType::String, .line = m_line_number, .value = result};
}

auto PGNLexer::read_token_starting_with_number(char first_c) -> Token {
    std::string result{first_c};
    int character = m_in_stream->get();
    bool only_numbers = true;
    while (!m_in_stream->bad()) {
        while (!m_in_stream->bad() && (std::isdigit(character) != 0)) {
            result += static_cast<char>(character);
            character = m_in_stream->get();
        }
        if (m_in_stream->bad()) {
            if (m_in_stream->eof()) {
                break;
            }
            throw PGNError{PGNErrorType::InputError, m_line_number};
        }
        if (character == '/' || character == '-') {
            only_numbers = false;
            result += static_cast<char>(character);
            character = m_in_stream->get();
        } else {
            break;
        }
    }
    m_in_stream->unget();
    if (only_numbers) {
        return Token{.type = TokenType::Number, .line = m_line_number, .value = result};
    }
    if (result == "1-0" || result == "0-1" || result == "1/2-1/2") {
        return Token{.type = TokenType::GameResult, .line = m_line_number, .value = result};
    }
    throw PGNError(PGNErrorType::InvalidGameResult, m_line_number, result);
}

auto PGNLexer::read_symbol(char first_c) -> Token {
    std::string result{first_c};
    int character = m_in_stream->get();
    while (!m_in_stream->bad() && !is_whitespace(static_cast<char>(character)) && (character != ')')) {
        result += static_cast<char>(character);
        character = m_in_stream->get();
    }
    if (m_in_stream->bad()) {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    m_in_stream->unget();
    return Token{.type = TokenType::Symbol, .line = m_line_number, .value = result};
}

auto PGNLexer::read_comment() -> Token {
    std::string result{};
    int character = m_in_stream->get();
    while (!m_in_stream->bad() && character != '}') {
        if (character == '\n') {
            m_line_number++;
        }
        if (is_whitespace(static_cast<char>(character))) {
            character = ' ';
        }
        result += static_cast<char>(character);
        character = m_in_stream->get();
    }
    if (m_in_stream->bad()) {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    return Token{.type = TokenType::Comment, .line = m_line_number, .value = result};
}

auto PGNLexer::read_nag() -> Token {
    std::string result{};
    int character = m_in_stream->get();
    while (!m_in_stream->bad() && (std::isdigit(character) != 0)) {
        result += static_cast<char>(character);
        character = m_in_stream->get();
    }
    if (m_in_stream->bad()) {
        throw PGNError{PGNErrorType::InputError, m_line_number};
    }
    m_in_stream->unget();
    return Token{.type = TokenType::NAG, .line = m_line_number, .value = result};
}

auto PGNLexer::skip_back() -> void {
    m_in_stream->unget();
}

auto PGNParser::reset() -> void {
    m_metadata = GameMetadata{};
}

auto PGNParser::setup_game() -> void {
    m_game = Game{m_metadata};
    if (!m_overall_game_comment.empty()) {
        m_game.edit().set_comment(m_overall_game_comment);
    }
    clear_cursor_stack();
    m_cursors.push(m_game.edit());
}

auto PGNParser::read_game() -> std::optional<Game> {
    reset();
    next_token();
    if (m_token.type == PGNLexer::TokenType::EndOfInput) {
        return std::nullopt;
    }
    check_token_type(PGNLexer::TokenType::OpenBracket, "Metadata tags expected");
    read_metadata();
    setup_game();
    read_movetext();
    return m_game;
}

auto PGNParser::skip_to_next_game() -> void {
    while (m_token.type != PGNLexer::TokenType::EndOfInput && m_token.type != PGNLexer::TokenType::OpenBracket) {
        next_token();
    }
    if (m_token.type == PGNLexer::TokenType::OpenBracket) {
        m_lexer.skip_back();
    }
}

auto PGNParser::read_movetext() -> void {
    while (m_token.type != PGNLexer::TokenType::GameResult) {
        switch (m_token.type) {
        case PGNLexer::TokenType::Number:
            read_move_number_indication();
            break;
        case PGNLexer::TokenType::Symbol:
            read_move();
            break;
        case PGNLexer::TokenType::NAG:
            annotate_move();
            break;
        case PGNLexer::TokenType::Comment:
            process_move_comment();
            break;
        case PGNLexer::TokenType::OpenParen:
            start_rav();
            break;
        case PGNLexer::TokenType::CloseParen:
            finish_rav();
            break;
        default:
            throw PGNError(PGNErrorType::UnexpectedToken, m_token.line, "Unexpected token in movetext");
        }
    }
    process_game_result();
}

auto PGNParser::read_move() -> void {
    check_token_type(PGNLexer::TokenType::Symbol, "Move expected");
    process_move();
}

auto PGNParser::read_metadata() -> void {
    while (m_token.type == PGNLexer::TokenType::OpenBracket) {
        read_tag();
        next_token();
    }
    if (m_token.type == PGNLexer::TokenType::Comment) {
        m_overall_game_comment = m_token.value;
        next_token();
    }
}

auto PGNParser::read_tag() -> void {
    expect_token(PGNLexer::TokenType::Symbol, "Name expected");
    const auto tag_name = m_token.value;
    expect_token(PGNLexer::TokenType::String, "String expected");
    const auto tag_value = m_token.value;
    m_metadata.emplace_back(tag_name, tag_value);
    expect_token(PGNLexer::TokenType::CloseBracket, "Close bracket expected");
}

auto PGNParser::annotate_move() -> void {
    current_game_line().node()->nags().emplace_back(stoi(m_token.value));
    next_token();
}

auto PGNParser::process_game_result() -> void {}

auto PGNParser::process_move_comment() -> void {
    current_game_line().set_comment(m_token.value);
    next_token();
}

auto PGNParser::start_rav() -> void {
    auto opt_parent = current_game_line().parent();
    if (opt_parent.has_value()) {
        m_cursors.push(opt_parent.value());
        next_token();
    } else {
        throw PGNError(PGNErrorType::CannotStartRav, m_token.line, "No parent in curent position");
    }
}

auto PGNParser::finish_rav() -> void {
    if (m_cursors.size() > 1) {
        m_cursors.pop();
        next_token();
    } else {
        throw PGNError(PGNErrorType::NoPenRav, m_token.line, "No RAV to close");
    }
}

auto PGNParser::read_move_number_indication() -> void {
    next_token();
    while (m_token.type == PGNLexer::TokenType::Dot) {
        next_token();
    }
}

auto PGNParser::parse_san_move(const std::string &san_str) const -> SANMove {
    const auto san_exp = parse_san(san_str, current_game_line().position().side_to_move());
    if (san_exp.has_value()) {
        return san_exp.value();
    }
    throw PGNError{PGNErrorType::InvalidMove, m_token.line, san_exp.error().san};
}

auto PGNParser::find_legal_move(const SANMove &san_move) const -> chesscore::Move {
    const auto legal_moves = current_game_line().position().all_legal_moves();
    const auto matched_moves = match_san_move(san_move, legal_moves);
    if (matched_moves.size() == 1) {
        return matched_moves[0];
    }
    if (matched_moves.size() > 1) {
        throw PGNError{PGNErrorType::AmbiguousMove, m_token.line, san_move.san_string};
    }
    throw PGNError{PGNErrorType::IllegalMove, m_token.line, san_move.san_string};
}

auto PGNParser::process_move() -> void {
    const auto san_move = parse_san_move(m_token.value);
    const auto move = find_legal_move(san_move);
    Cursor &cursor = current_game_line();
    const auto new_cursor = cursor.play_move(move);
    current_game_line() = new_cursor;
    next_token();
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
