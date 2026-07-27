#ifndef COMMANDDEFINITION_HPP
#define COMMANDDEFINITION_HPP

#include <Geode/Geode.hpp>

namespace CommandPalette::Core {
    struct CommandDefinition {
        std::string m_name;
        std::string m_description;
        std::vector<CommandDefinition> m_subcommands;
        std::function<void(const std::vector<std::string>&)> m_callback;
    };
}

#endif