/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_PGN_H
#define CHESSGAME_GAME_PGN_H

#include <iosfwd>
#include <optional>
#include <ostream>
#include <stack>
#include <string>

#include "chessgame/cursor.h"
#include "chessgame/game.h"
#include "chessgame/san.h"
#include "chessgame/types.h"

namespace chessgame {

/**
 * \brief Type of a PGN error.
 *
 * These errors can appear when parsing PGN data.
 */
enum class PGNErrorType {
    InputError,        ///< Error reading the input.
    UnexpectedChar,    ///< Unexpected character in input.
    UnexpectedToken,   ///< Unexpected token in input.
    InvalidMove,       ///< Invalid move in input.
    IllegalMove,       ///< Move is illegal in the current position.
    AmbiguousMove,     ///< Move is ambiguous in the current position.
    InvalidGameResult, ///< Invalid game result.
    CannotStartRav,    ///< Cannot start a RAV in this position.
    NoPenRav,          ///< There is currently no RAV active.
    EndOfInput         ///< End of input.
};

auto to_string(PGNErrorType type) -> std::string;

/**
 * \brief Error when parsing PGN data.
 *
 * Describes an error that occured while reading PGN data.
 */
class PGNError : public ChessGameError {
public:
    explicit PGNError(PGNErrorType type, int line, std::string message = {}) : ChessGameError{std::move(message)}, m_type{type}, m_line{line} {}

    [[nodiscard]] auto type() const noexcept -> PGNErrorType { return m_type; }
    [[nodiscard]] auto line() const noexcept -> int { return m_line; }
private:
    PGNErrorType m_type; ///< Error type.
    int m_line;          ///< Line number where the error occured.
};

/**
 * \brief Types of (ignored) errors in PGN files.
 */
enum class PGNWarningType {
    UnexpectedChar,       ///< Unexpected character in input.
    MoveMissingCapture,   ///< The SAN move only matched, when adding capturing.
    MoveMissingPieceType, ///< The SAN move didn't mention any piece type.
};

auto to_string(PGNWarningType type) -> std::string;

/**
 * \brief Describes a problem with PGN data.
 *
 * This is used to describe a problem, when a PGN file does not follow the PGN
 * specification, but the error is ignored in order to allow reading the PGN
 * file anyways.
 *
 * These warnings are collected by the PGNParser and can be retrieved after the
 * game was parsed.
 */
struct PGNWarning {
    PGNWarningType type;     ///< The type of the warning.
    int line;                ///< The line number where the warning occured.
    std::string description; ///< A description of the warning.
};

/**
 * \brief Lexical analysis of PGN data.
 *
 * Extracts tokens from PGN data.
 */
class PGNLexer {
public:
    /**
     * \brief Type of a PGN token.
     */
    enum class TokenType {
        OpenBracket,  ///< Opening bracket [
        CloseBracket, ///< Closing bracket ]
        Symbol,       ///< Name, e.g. of a tag; move
        String,       ///< String
        Number,       ///< Number (integer)
        NAG,          ///< Numeric Annotation Glyph
        Dot,          ///< Dot
        OpenParen,    ///< Opening parenthesis (
        CloseParen,   ///< Closing parenthesis )
        Comment,      ///< Comment
        GameResult,   ///< Game result (1-0, 0-1, 1/2-1/2, ...)
        EndOfInput,   ///< End of input
        Invalid,      ///< Invalid token
    };

    /**
     * \brief A token of PGN data.
     *
     * Describes a lexical unit in the PGN data stream.
     */
    struct Token {
        TokenType type{TokenType::Invalid}; ///< The type of the token.
        int line{0};                        ///< The line number of the token.
        std::string value;                  ///< The value of the token.
    };

    /**
     * \brief Create a PGNLexer for a given input stream.
     *
     * \param in_stream The PGN input.
     */
    explicit PGNLexer(std::istream *in_stream) : m_in_stream{in_stream} {}

    /**
     * \brief Retrieve the next token from the input stream.
     *
     * Tries to find the next token in the PGN input.
     * \return The token or an error description.
     */
    [[nodiscard]] auto next_token() -> Token;

    /**
     * \brief The current line number.
     *
     * The number of the line the lexer is currently analysing.
     * \return The current line number.
     */
    [[nodiscard]] auto line_number() const -> int { return m_line_number; }

    auto skip_back() -> void;
private:
    std::istream *m_in_stream; ///< The input stream
    int m_line_number{1};      ///< Current line number

    [[nodiscard]] static auto is_whitespace(char character) -> bool;
    auto skip_whitespace(int character) -> void;
    auto read_string() -> Token;
    auto read_token_starting_with_number(char first_c) -> Token;
    [[nodiscard]] static auto is_symbol_character(char character) -> bool;
    auto read_symbol(char first_c) -> Token;
    auto read_comment() -> Token;
    auto read_nag() -> Token;
};

auto to_string(PGNLexer::TokenType type) -> std::string;

/**
 * \brief Parser for PGN data.
 *
 * Performs syntactical analysis of PGN data and extracts games.
 */
class PGNParser {
public:
    explicit PGNParser(std::istream &in_stream) : m_lexer{&in_stream} {}

    auto read_game() -> std::optional<Game>;

    auto warnings() const -> const std::vector<PGNWarning> & { return m_warnings; }

    auto skip_to_next_game() -> void;
private:
    PGNLexer m_lexer;
    PGNLexer::Token m_token;
    GameMetadata m_metadata;
    Game m_game;
    std::string m_overall_game_comment;

    struct rav_descriptor {
        bool has_moves{false};
        std::string comment;
    };
    std::stack<rav_descriptor> m_rav_stack;

    mutable std::vector<PGNWarning> m_warnings;

    std::stack<Cursor> m_cursors;
    auto reset() -> void;
    auto setup_game() -> void;
    auto clear_cursor_stack() -> void;
    auto current_game_line() -> Cursor & { return m_cursors.top(); }
    [[nodiscard]] auto current_game_line() const -> const Cursor & { return m_cursors.top(); }

    auto next_token() -> void;
    auto read_metadata() -> void;
    auto read_movetext() -> void;
    auto read_move() -> void;
    auto read_tag() -> void;
    auto annotate_move() -> void;
    auto process_game_result() -> void;
    auto process_move_comment() -> void;
    auto start_rav() -> void;
    auto finish_rav() -> void;
    auto read_move_number_indication() -> void;

    auto process_move() -> void;
    [[nodiscard]] auto parse_san_move(const std::string &san_str) const -> SANMove;
    [[nodiscard]] auto find_legal_move(const SANMove &san_move) const -> chesscore::Move;

    auto check_token_type(PGNLexer::TokenType expected_type, const std::string &error_message) const -> void;
    auto expect_token(PGNLexer::TokenType expected_type, const std::string &error_message) -> void;
    auto skip_tokens(PGNLexer::TokenType type) -> void;
};

class PGNTokenOutput {
public:
    explicit PGNTokenOutput(std::ostream *ostream) : m_ostream{ostream} {}

    enum class OutToken { None, Tag, MoveNumber, Move, Comment, RavStart, RavEnd };

    template<typename... Args>
    auto write(OutToken type, Args &&...args) -> void {
        static_assert(sizeof...(args) > 0, "No data to write");
        std::string token = (... + to_string(std::forward<Args>(args)));
        write_token(type, token);
    }

    auto newline() -> void;

    auto end_metadata_section() -> void { newline(); }
private:
    std::ostream *m_ostream;
    OutToken m_last_out_token{OutToken::None};
    const size_t m_max_line_length{79};
    size_t m_current_line_length{};

    template<size_t N>
    std::string to_string(const char (&val)[N]) {
        return std::string(val);
    }

    std::string to_string(const std::string &val) { return val; }

    std::string to_string(char val) { return std::string(1, val); }

    template<typename T>
    std::string to_string(const T &val) {
        return std::to_string(val);
    }

    auto write_token(OutToken type, const std::string &token) -> void;

    auto needs_whitespace(OutToken type) const -> bool;
};

/**
 * \brief Writer for PGN data.
 *
 * Provides functionality to write a chess game as PGN data to a stream.
 */
class PGNWriter {
public:
    explicit PGNWriter(std::ostream &ostream) : m_output{&ostream} {}

    auto write_game(const Game &game) -> void;

    auto write_metadata(const GameMetadata &metadata) -> void;
    auto write_game_lines(const ConstCursor &node) -> void;

    auto write_str_tags(const GameMetadata &metadata) -> void;
    auto write_non_str_tags(const GameMetadata &metadata) -> void;
    auto write_tag_pair(const std::string &name, const std::string &value) -> void;
    auto write_tag_pair(const metadata_tag &tag) -> void;
    auto write_move(const GameNode &node) -> void;
    auto write_rav(const ConstCursor &node) -> void;
private:
    PGNTokenOutput m_output;
    bool m_write_black_move_number{false};
};

} // namespace chessgame

#endif
