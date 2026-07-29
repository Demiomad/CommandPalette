#ifndef COMMANDDEFINITION_HPP
#define COMMANDDEFINITION_HPP

#include <Geode/Geode.hpp>
#include "CommandArgs.hpp"

namespace CommandPalette::Core::Commands
{
    struct CommandDefinition
    {
        std::string m_name;
        std::string m_description;
        std::string m_usage;
        std::vector<std::string> m_aliases;
        std::function<bool(const CommandArgs&)> m_callback;
    };
}

#endif