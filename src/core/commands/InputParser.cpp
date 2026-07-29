#include "InputParser.hpp"

const CommandPalette::Core::Commands::ParsedInput CommandPalette::Core::Commands::InputParser::parse(const std::string& input)
{
    auto parsed = CommandPalette::Core::Commands::ParsedInput();
    auto tokens = geode::utils::string::split(input, " ");
    auto tokenCount = tokens.size();

    if (tokenCount == 0) return parsed;

    parsed.m_args.m_tokens = tokens;
    parsed.m_commandName = tokens.at(0);
    
    if (tokenCount > 1)
    {
        for (int i = 1; i < tokenCount; i++)
        {
            auto token = tokens.at(i);
            if (geode::utils::string::startsWith(token, FLAG_PREFIX))
            {
                auto flag = token.substr(1, token.length() - 1);
                parsed.m_args.m_flags.push_back(flag);
            }
            else
            {
                parsed.m_args.m_positional.push_back(token);
            }
        }
    }
    
    return parsed;
}