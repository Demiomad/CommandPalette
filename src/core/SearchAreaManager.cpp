#include "SearchAreaManager.hpp"

bool CommandPalette::Core::SearchAreaManager::s_isToggled = false;

bool CommandPalette::Core::SearchAreaManager::getIsToggled() {
    return s_isToggled;
}

void CommandPalette::Core::SearchAreaManager::setIsToggled(bool value) {
    s_isToggled = value;
}