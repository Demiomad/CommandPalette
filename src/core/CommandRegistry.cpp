#include "CommandRegistry.hpp"

std::vector<CommandPalette::Core::CommandDefinition> CommandPalette::Core::CommandRegistry::getCommands() {
    return m_commands;
}

std::vector<CommandPalette::Core::CommandDefinition> CommandPalette::Core::CommandRegistry::getSubcommands(const std::string& parent) {
    auto cmd = getCommand(parent);
    if (cmd == nullptr)
        return {};
    
    return cmd->m_subcommands;
}

CommandPalette::Core::CommandDefinition* CommandPalette::Core::CommandRegistry::getCommand(const std::string& name) {
    for (auto& command : m_commands) {
        geode::log::debug("command: {}", command.m_name);
        if (command.m_name == name)
            return &command;
    }
    geode::log::debug("command not found: {}", name);
    return nullptr;
}

CommandPalette::Core::CommandDefinition CommandPalette::Core::CommandRegistry::createCommand(const std::string& name, const std::string& description, std::function<void(const std::vector<std::string>&)> callback, const bool autoRegister) {
    auto command = CommandDefinition();
    command.m_name = name;
    command.m_description = description;
    command.m_callback = callback;
    if (autoRegister) m_commands.push_back(command);

    geode::log::debug("adding command {}: {}", name, description);

    return command;
}

bool CommandPalette::Core::CommandRegistry::run(const std::string& name, const std::vector<std::string>& args) {
    auto cmd = getCommand(name);
    if (cmd == nullptr)
        return false;

    cmd->m_callback(args);
    return true;
}

const CommandPalette::Core::CommandDefinition* CommandPalette::Core::CommandRegistry::getSubcommand(const std::string& parent, const std::string& name) {
    auto command = getCommand(parent);
    if (command == nullptr)
        return nullptr;

    for (auto& subcommand : command->m_subcommands) {
        geode::log::debug("subcommand: {}", subcommand.m_name);
        if (subcommand.m_name == name)
            return &subcommand;
    }
    geode::log::debug("subcommand from {} not found: {}", command->m_name, name);

    return nullptr;
}

bool CommandPalette::Core::CommandRegistry::run(const std::string& parent, const std::string& name, const std::vector<std::string>& args) {
    auto subCmd = getSubcommand(parent, name);
    if (subCmd == nullptr)
        return false;

    subCmd->m_callback(args);
    return true;
}

void CommandPalette::Core::CommandRegistry::addSubcommand(const std::string& parent, const std::string& name, const std::string& description, std::function<void(const std::vector<std::string>&)> callback) {
    auto command = getCommand(parent);
    if (command == nullptr)
        return;
    
    auto subcommand = createCommand(name, description, callback, false);
    geode::log::debug("subcommand {} for {}: {}", name, command->m_name, description);

    command->m_subcommands.push_back(subcommand);
}

std::string CommandPalette::Core::CommandRegistry::createMarkdownString() {
    std::string output;

    for (const auto& command : m_commands) {
        output.append(fmt::format("# {}\n{}\n", command.m_name, command.m_description));

        for (const auto& subcommand : command.m_subcommands) {
            output.append(fmt::format("- `{} {}` - {}\n", command.m_name, subcommand.m_name, subcommand.m_description));
        }

        output.append("\n");
    }

    return output;
}