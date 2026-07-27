#ifndef COMMANDREGISTRY_HPP
#define COMMANDREGISTRY_HPP

#include <Geode/Geode.hpp>
#include "CommandDefinition.hpp"

namespace CommandPalette::Core {
    class CommandRegistry {
        private:
            std::vector<CommandDefinition> m_commands;

        public:
            std::vector<CommandDefinition> getCommands();
            std::vector<CommandDefinition> getSubcommands(const std::string& parent);

            CommandDefinition* getCommand(const std::string& name);
            const CommandDefinition* getSubcommand(const std::string& parent, const std::string& name);

            CommandDefinition createCommand(const std::string& name, const std::string& description, std::function<void(const std::vector<std::string>&)> callback, const bool autoRegister = true);
            void addSubcommand(const std::string& parent, const std::string& name, const std::string& description, std::function<void(const std::vector<std::string>&)> callback);

            bool run(const std::string& name, const std::vector<std::string>& args);
            bool run(const std::string& parent, const std::string& name, const std::vector<std::string>& args);

            std::string createMarkdownString();
    };
}

#endif