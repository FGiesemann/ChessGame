/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/pgn.h"
#include "chessgame/move_matcher.h"
#include "chessgame/san.h"

#include <algorithm>
#include <cctype>
#include <istream>
#include <set>

namespace chessgame {

auto to_string(PGNLexer::TokenType type) -> std::string {
    switch (type) {
    case PGNLexer::TokenType::OpenBracket:
        return "OpenBracket";
    case PGNLexer::TokenType::CloseBracket:
        return "CloseBracket";
    case PGNLexer::TokenType::Symbol:
        return "Symbol";
    case PGNLexer::TokenType::String:
        return "String";
    case PGNLexer::TokenType::Number:
        return "Number";
    case PGNLexer::TokenType::NAG:
        return "NAG";
    case PGNLexer::TokenType::Dot:
        return "Dot";
    case PGNLexer::TokenType::OpenParen:
        return "OpenParen";
    case PGNLexer::TokenType::CloseParen:
        return "CloseParen";
    case PGNLexer::TokenType::Comment:
        return "Comment";
    case PGNLexer::TokenType::GameResult:
        return "GameResult";
    case PGNLexer::TokenType::EndOfInput:
        return "EndOfInput";
    case PGNLexer::TokenType::Invalid:
        return "Invalid";
    default:
        return "UNKNOWN TOKEN TYPE";
    }
}

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

auto to_string(PGNWarningType type) -> std::string {
    switch (type) {
    case PGNWarningType::UnexpectedChar:
        return "unexpected character";
    case PGNWarningType::MoveMissingCapture:
        return "move missing capturing";
    case PGNWarningType::MoveMissingPieceType:
        return "move missing piece type";
    default:
        return "UNKNOWN WARNING!";
    }
}

auto PGNLexer::next_token() -> Token {
    int character = m_in_stream->get();
    if (is_whitespace(static_cast<char>(character))) {
        skip_whitespace(character);
        character = m_in_stream->get();
    }
    while (!m_in_stream->bad()) {
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
        case '*':
            return Token{.type = TokenType::GameResult, .line = m_line_number, .value = "*"};
        default:
            return Token{.type = TokenType::Invalid, .line = m_line_number, .value = std::string{static_cast<char>(character)}};
        }
    }
    throw PGNError{PGNErrorType::InputError, m_line_number};
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
    return Token{.type = TokenType::Invalid, .line = m_line_number, .value = result};
}

auto PGNLexer::is_symbol_character(char character) -> bool {
    static std::set<char> symbol_characters = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
                                               'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                                               'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '/', '+', '#', '=', '?', '!'};
    return symbol_characters.contains(character);
}

auto PGNLexer::read_symbol(char first_c) -> Token {
    std::string result{first_c};
    int character = m_in_stream->get();
    while (!m_in_stream->bad() && is_symbol_character(static_cast<char>(character))) {
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
    m_warnings.clear();
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
        case PGNLexer::TokenType::Dot:
            m_warnings.emplace_back(PGNWarningType::UnexpectedChar, m_token.line, "Unexpected char in movetext: .");
            next_token();
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
        case PGNLexer::TokenType::Invalid:
            if (m_token.value == "," || m_token.value == "}") {
                m_warnings.emplace_back(PGNWarningType::UnexpectedChar, m_token.line, "Unexpected char in movetext: " + m_token.value);
                next_token();
                break;
            } else {
                throw PGNError(PGNErrorType::UnexpectedToken, m_token.line, std::string{"Invalid token in movetext '"} + m_token.value + std::string{"'"});
            }
        default:
            throw PGNError(
                PGNErrorType::UnexpectedToken, m_token.line,
                std::string{"Unexpected token of type "} + to_string(m_token.type) + std::string{" in movetext '"} + m_token.value + std::string{"'"}
            );
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
    m_metadata.add(tag_name, tag_value);
    expect_token(PGNLexer::TokenType::CloseBracket, "Close bracket expected");
}

auto PGNParser::annotate_move() -> void {
    current_game_line().node()->nags().emplace_back(stoi(m_token.value));
    next_token();
}

auto PGNParser::process_game_result() -> void {}

auto PGNParser::process_move_comment() -> void {
    if (!m_rav_stack.empty() && !m_rav_stack.top().has_moves) {
        m_rav_stack.top().comment = m_token.value;
    } else {
        current_game_line().append_comment(m_token.value);
    }
    next_token();
}

auto PGNParser::start_rav() -> void {
    auto opt_parent = current_game_line().parent();
    if (opt_parent.has_value()) {
        m_cursors.push(opt_parent.value());
        m_rav_stack.emplace(false, std::string{});
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
    if (!m_rav_stack.empty()) {
        m_rav_stack.pop();
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
    if (legal_moves.empty()) {
        throw PGNError{PGNErrorType::IllegalMove, m_token.line, san_move.san_string};
    }
    const auto matched_moves = match_san_move(san_move, legal_moves);
    if (matched_moves.size() == 1) {
        return matched_moves[0];
    }
    if (matched_moves.size() > 1) {
        throw PGNError{PGNErrorType::AmbiguousMove, m_token.line, san_move.san_string};
    }

    // Could not match the SAN move against the legal moves, try some modifications...

    const auto matched_without_piece_type = match_san_move_wildcard_piece_type(san_move, legal_moves);
    if (matched_without_piece_type.size() == 1) {
        m_warnings.emplace_back(PGNWarningType::MoveMissingPieceType, m_token.line, san_move.san_string);
        return matched_without_piece_type[0];
    }
    if (!san_move.capturing) {
        auto try_move = san_move;
        try_move.capturing = true;
        const auto matched_captures = match_san_move(try_move, legal_moves);
        if (matched_captures.size() == 1) {
            m_warnings.emplace_back(PGNWarningType::MoveMissingCapture, m_token.line, san_move.san_string);
            return matched_captures[0];
        }
    }

    throw PGNError{PGNErrorType::IllegalMove, m_token.line, san_move.san_string};
}

auto PGNParser::process_move() -> void {
    const auto san_move = parse_san_move(m_token.value);
    const auto move = find_legal_move(san_move);
    Cursor &cursor = current_game_line();
    const auto new_cursor = cursor.play_move(move);
    current_game_line() = new_cursor;
    if (!m_rav_stack.empty()) {
        m_rav_stack.top().has_moves = true;
        if (!m_rav_stack.top().comment.empty()) {
            current_game_line().append_premove_comment(m_rav_stack.top().comment);
        }
    }
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

auto PGNWriter::write_game(const Game &game) -> void {
    write_metadata(game.metadata());
    write_game_lines(game.const_cursor());
}

auto PGNWriter::write_game_lines(const ConstCursor &node) -> void {
    ConstCursor cursor = node;
    while (cursor.child_count() > 0) {
        const auto mainline_child = cursor.child(0);
        if (mainline_child) {
            write_move(mainline_child.value().node()->move());
        } else {
            break;
        }
        for (size_t child_index = 1; child_index < cursor.child_count(); ++child_index) {
            const auto variant_cursor = cursor.child(child_index);
            if (variant_cursor) {
                write_rav(variant_cursor.value());
            }
        }
        cursor = mainline_child.value();
    }
}

auto PGNWriter::write_move([[maybe_unused]] const chesscore::Move &move) -> void {
    write(move.from.file().name());
    write(move.from.rank().rank);
    write(move.to.file().name());
    write(move.to.rank().rank);
    write(' ');
}

auto PGNWriter::write_rav([[maybe_unused]] const ConstCursor &node) -> void {
    write('(');
    write_move(node.node()->move());
    write_game_lines(node);
    write(") ");
}

auto PGNWriter::write_metadata(const GameMetadata &metadata) -> void {
    write_str_tags(metadata);
    write_non_str_tags(metadata);
    newline();
}

auto PGNWriter::write_str_tags(const GameMetadata &metadata) -> void {
    for (const auto &tag_name : GameMetadata::str_tags) {
        const auto value = metadata.get(tag_name).value_or("?");
        write_tag_pair(tag_name, value);
    }
}

auto PGNWriter::write_non_str_tags(const GameMetadata &metadata) -> void {
    std::vector<metadata_tag> non_str_tags;
    std::ranges::copy_if(metadata, std::back_inserter(non_str_tags), [](const metadata_tag &tag) { return !GameMetadata::is_str_tag(tag); });
    std::ranges::sort(non_str_tags, [](const metadata_tag &tag1, const metadata_tag &tag2) { return tag1.name < tag2.name; });
    std::ranges::for_each(non_str_tags, [this](const metadata_tag &tag) { write_tag_pair(tag); });
}

auto PGNWriter::write_tag_pair(const std::string &name, const std::string &value) -> void {
    write('[');
    write(name);
    write(" \"");
    write(value);
    write("\"]\n");
}

auto PGNWriter::write_tag_pair(const metadata_tag &tag) -> void {
    write_tag_pair(tag.name, tag.value);
}

template<typename T>
auto PGNWriter::write(const T &data) -> void {
    *m_ostream << data;
}

template<size_t N>
auto PGNWriter::write(const char data[N]) -> void {
    *m_ostream << data;
}

auto PGNWriter::newline() -> void {
    write('\n');
}

} // namespace chessgame
