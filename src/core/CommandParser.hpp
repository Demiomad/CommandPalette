#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include <Geode/Geode.hpp>
#include "ParsedCommandData.hpp"

namespace CommandPalette::Core {
    class CommandParser {
        public:
            static ParsedCommandData parse(const std::string& input);
            static ParsedCommandData parseSubcommand(const std::string& input);
    };
}

#endif