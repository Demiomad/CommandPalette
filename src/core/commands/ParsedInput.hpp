#ifndef PARSEDINPUT_HPP
#define PARSEDINPUT_HPP

#include <Geode/Geode.hpp>
#include "CommandArgs.hpp"

namespace CommandPalette::Core::Commands
{
    struct ParsedInput
    {
        std::string m_commandName;
        CommandArgs m_args;
    };
}

#endif