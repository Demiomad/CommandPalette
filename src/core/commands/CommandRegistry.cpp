#include "CommandRegistry.hpp"
#include "InputParser.hpp"

const std::vector<CommandPalette::Core::Commands::CommandDefinition> CommandPalette::Core::Commands::CommandRegistry::getCommands()
{
    return m_commands;
}

const CommandPalette::Core::Commands::CommandDefinition* CommandPalette::Core::Commands::CommandRegistry::getCommandByName(const std::string& name)
{
    for (const auto& command : m_commands)
    {
        if (command.m_name == name || hasAlias(command, name))
        {
            return &command;
        }
    }
    return nullptr;
}

const bool CommandPalette::Core::Commands::CommandRegistry::executeCommandFromInput(const std::string& input)
{
    auto parsed = InputParser::parse(input);
    auto command = getCommandByName(parsed.m_commandName);

    if (command == nullptr)
    {
        geode::log::error("Command {} doesn't exist", parsed.m_commandName);
        return false;    
    }

    return command->m_callback(parsed.m_args);
}

const bool CommandPalette::Core::Commands::CommandRegistry::hasAlias(const CommandPalette::Core::Commands::CommandDefinition& cmd, const std::string& alias)
{
    for (const auto& commandAlias : cmd.m_aliases)
    {
        if (commandAlias == alias)
        {
            return true;
        }
    }
    return false;
}

void CommandPalette::Core::Commands::CommandRegistry::registerCommand(const std::string& name, const std::string& description, const std::string& usage, const std::vector<std::string> aliases, std::function<bool(const CommandPalette::Core::Commands::CommandArgs&)> callback)
{
    auto cmd = CommandDefinition
    {
        .m_name = name,
        .m_description = description,
        .m_usage = usage,
        .m_aliases = aliases,
        .m_callback = callback
    };

    m_commands.push_back(cmd);
}

const bool CommandPalette::Core::Commands::CommandRegistry::hasFlag(const CommandPalette::Core::Commands::CommandArgs& args, const std::string& flagName)
{
    for (const auto& flag : args.m_flags)
    {
        if (flag == flagName)
        {
            return true;
        }
    }
    return false;
}

const std::string CommandPalette::Core::Commands::CommandRegistry::createMarkdownString()
{
    std::string output;

    for (const auto& command : m_commands)
    {
        // it doesnt look right with single \n idk why
        
        output.append(
            fmt::format(
                "# {}\n(AKA {})\n\n{}\n\n",
                command.m_name,
                fmt::join(command.m_aliases, ", "),
                command.m_description
            )
        );

        output.append(fmt::format("Usage:\n{}", command.m_usage));
        output.append("\n");
    }

    return output;
}