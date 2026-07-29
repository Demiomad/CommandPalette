#ifndef COMMANDREGISTRY_HPP
#define COMMANDREGISTRY_HPP

#include <Geode/Geode.hpp>
#include "CommandDefinition.hpp"
#include "CommandArgs.hpp"

namespace CommandPalette::Core::Commands
{
    class CommandRegistry
    {
        private:
            std::vector<CommandDefinition> m_commands;

        public:
            static const bool hasFlag(const CommandArgs& args, const std::string& flagName);

            const std::vector<CommandDefinition> getCommands();
            const CommandDefinition* getCommandByName(const std::string& name);
            const bool executeCommandFromInput(const std::string& input);
            const bool hasAlias(const CommandDefinition& cmd, const std::string& alias);            

            void registerCommand(const std::string& name, const std::string& description,
                const std::string& usage, const std::vector<std::string> aliases, std::function<bool(const CommandArgs&)> callback);

            const std::string createMarkdownString();
    };
}

#endif