#include "CommandParser.hpp"

CommandPalette::Core::ParsedCommandData CommandPalette::Core::CommandParser::parse(const std::string& input) {
    auto parsed = ParsedCommandData();
    auto trimmed = geode::utils::string::trim(input);
    auto split = geode::utils::string::split(trimmed, " ");

    if (split.empty())
        return parsed;

    parsed.m_name = split.at(0);

    for (size_t i = 1; i < split.size(); i++) {
        auto part = split.at(i);
        parsed.m_args.push_back(part);
    }

    return parsed;
}

CommandPalette::Core::ParsedCommandData CommandPalette::Core::CommandParser::parseSubcommand(const std::string& input) {
    auto parsed = ParsedCommandData();
    auto trimmed = geode::utils::string::trim(input);
    auto split = geode::utils::string::split(trimmed, " ");

    if (split.empty())
        return parsed;

    parsed.m_name = split.at(0);
    parsed.m_subcommand = split.at(1);

    for (size_t i = 2; i < split.size(); i++) {
        auto part = split.at(i);
        parsed.m_args.push_back(part);
    }

    geode::log::debug("name: {}, subcommand name: {}", parsed.m_name, parsed.m_subcommand);
    geode::log::debug("args: {}", fmt::join(parsed.m_args, ", "));

    return parsed;
}