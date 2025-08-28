/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */
/** \file */

#ifndef CHESSGAME_GAME_METADATA_H
#define CHESSGAME_GAME_METADATA_H

#include <array>
#include <optional>
#include <string>
#include <vector>

namespace chessgame {

/**
 * \brief A metadata tag.
 *
 * A metadata tag is a key-value pair and describes meta data of a chess game.
 */
struct metadata_tag {
    std::string name;  ///< Name of the metadata tag.
    std::string value; ///< Value of the metadata tag.
};

/**
 * \brief A collection of metadata tags.
 */
class GameMetadata {
public:
    using iterator = std::vector<metadata_tag>::iterator;
    using const_iterator = std::vector<metadata_tag>::const_iterator;

    iterator begin() { return m_tags.begin(); }
    iterator end() { return m_tags.end(); }
    const_iterator begin() const { return m_tags.begin(); }
    const_iterator end() const { return m_tags.end(); }

    static const std::array<std::string, 7> str_tags;

    /**
     * \brief Retrieve the value of a tag pair.
     *
     * Returns the value of a tag pair with the given name, if it exists.
     * Returns nullopt otherwise.
     * \param name Name of the tag pair.
     * \return Value of the tag pair, if it exists.
     */
    auto get(const std::string &name) const -> std::optional<std::string>;

    /**
     * \brief Add a tag pair.
     *
     * \param name Name of the tag.
     * \param value Value of the tag.
     */
    template<typename NameT, typename ValueT>
    auto emplace_back(NameT &&name, ValueT &&value) -> void {
        m_tags.emplace_back(std::forward<NameT>(name), std::forward<ValueT>(value));
    }

    /**
     * \brief Determine, if the tag belongs to the seven tag roster (STR).
     *
     * The STR tags are: Event, Site, Date, Round, White, Black, Result.
     * \param name Name of the tag.
     * \return If the tag belongs to the STR.
     */
    static auto is_str_tag(const std::string &name) -> bool;

    /**
     * \brief Determine, if the tag belongs to the seven tag roster (STR).
     *
     * The STR tags are: Event, Site, Date, Round, White, Black, Result.
     * \param tag The tag.
     * \return If the tag belongs to the STR.
     */
    static auto is_str_tag(const metadata_tag &tag) -> bool;
private:
    std::vector<metadata_tag> m_tags{}; ///< Collection of metadata tags.
};

} // namespace chessgame

#endif
