#ifndef COMMANDARGS_HPP
#define COMMANDARGS_HPP

#include <Geode/Geode.hpp>

namespace CommandPalette::Core::Commands
{
    struct CommandArgs
    {
        std::vector<std::string> m_tokens;
        std::vector<std::string> m_positional;
        std::vector<std::string> m_flags;
    };
}

#endif