/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_PGN_H
#define CHESSGAME_GAME_PGN_H

#include <iosfwd>
#include <optional>
#include <string>

#include "chessgame/game.h"
#include "chessgame/types.h"

namespace chessgame {

/**
 * \brief Type of a PGN error.
 *
 * These errors can appear when parsing PGN data.
 */
enum class PGNErrorType {
    InputError,     ///< Error reading the input.
    UnexpectedChar, ///< Unexpected character in input.
    EndOfInput      ///< End of input.
};

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
        Name,         ///< Name, e.g. of a tag
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
        std::string value{};                ///< The value of the token.
    };

    /**
     * \brief Create a PGNLexer for a given input stream.
     *
     * \param in_stream The PGN input.
     */
    explicit PGNLexer(std::istream &in_stream) : m_in_stream{in_stream} {}

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
private:
    std::istream &m_in_stream; ///< The input stream
    int m_line_number{1};      ///< Current line number

    [[nodiscard]] static auto is_whitespace(char c) -> bool;
    auto skip_whitespace(int c) -> void;
    auto read_string() -> Token;
    auto read_token_starting_with_number(char first_c) -> Token;
    auto read_name(char first_c) -> Token;
    auto read_comment() -> Token;
    auto read_nag() -> Token;
};

/**
 * \brief Parser for PGN data.
 *
 * Performs syntactical analysis of PGN data and extracts games.
 */
class PGNParser {
public:
    explicit PGNParser(std::istream &in_stream) : m_lexer{in_stream} {}

    auto read_game() -> std::optional<Game>;
private:
    PGNLexer m_lexer;
    PGNLexer::Token m_token;
    Game m_game;

    auto next_token() -> void;
    auto read_tag() -> void;
};

} // namespace chessgame

#endif
