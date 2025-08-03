/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_PGN_H
#define CHESSGAME_GAME_PGN_H

#include <expected>
#include <iosfwd>
#include <vector>

#include "chessgame/game.h"

namespace chessgame {

/**
 * \brief Type of a PGN error.
 *
 * These errors can appear when parsing PGN data.
 */
enum class PGNErrorType {

};

/**
 * \brief Error when parsing PGN data.
 *
 * Describes an error that occured while reading PGN data.
 */
struct PGNError {
    PGNErrorType type;   ///< Error type.
    std::string message; ///< Additional description of the error.
    int line;            ///< Line number where the error occured.
};

/**
 * \brief Parse PGN data.
 *
 * Reads PGN data from an input stream. Extracts the game information and
 * returns it as a list of game objects.
 * \param in_stream The input stream.
 * \return The parsed games or an error description.
 */
auto read_pgn(std::istream &in_stream) -> std::expected<std::vector<Game>, PGNError>;

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
        Dot,          ///< Dot
        OpenParen,    ///< Opening parenthesis (
        CloseParen,   ///< Closing parenthesis )
        OpenBrace,    ///< Opening brace {
        CloseBrace,   ///< Closing brace }
        Move,         ///< Move
        GameResult,   ///< Game result (1-0, 0-1, 1/2-1/2, ...)
        Invalid,      ///< Invalid token
    };

    /**
     * \brief A token of PGN data.
     *
     * Describes a lexical unit in the PGN data stream.
     */
    struct Token {
        TokenType type;    ///< The type of the token.
        std::string value; ///< The value of the token.
        int line;          ///< The line number of the token.
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
    [[nodiscard]] auto next_token() -> std::expected<Token, PGNError>;

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
};

} // namespace chessgame

#endif
