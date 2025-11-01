/* ************************************************************************** *
 * Chess Game                                                                 *
 * Representation of a single game of chess                                   *
 * ************************************************************************** */

#include "chessgame/metadata.h"

namespace chessgame {

const std::array<std::string, 7> GameMetadata::str_tags{"Event", "Site", "Date", "Round", "White", "Black", "Result"};

auto GameMetadata::is_str_tag(const std::string &name) -> bool {
    return std::find(std::begin(str_tags), std::end(str_tags), name) != str_tags.end();
}

auto GameMetadata::is_str_tag(const metadata_tag &tag) -> bool {
    return is_str_tag(tag.name);
}

auto GameMetadata::get(const std::string &name) const -> std::optional<std::string> {
    const auto tag = std::find_if(std::begin(*this), std::end(*this), [&name](const metadata_tag &search_tag) { return search_tag.name == name; });
    if (tag == end()) {
        return std::nullopt;
    }
    return tag->value;
}

} // namespace chessgame
