#ifndef INPUTPARSER_HPP
#define INPUTPARSER_HPP
#define FLAG_PREFIX "-"

#include <Geode/Geode.hpp>
#include "ParsedInput.hpp"

namespace CommandPalette::Core::Commands
{
    class InputParser
    {
        public:
            static const ParsedInput parse(const std::string& input);
    };
}

#endif