#ifndef PARSEDCOMMANDDATA_HPP
#define PARSEDCOMMANDDATA_HPP

#include <Geode/Geode.hpp>

namespace CommandPalette::Core {
    struct ParsedCommandData {
        std::string m_name;
        std::string m_subcommand;
        std::vector<std::string> m_args;  
    };
}

#endif