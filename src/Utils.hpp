#ifndef UTILS_HPP
#define UTILS_HPP

#include "ui/SearchArea.hpp"
#include "core/SearchAreaManager.hpp"
#include <Geode/Geode.hpp>
using namespace geode::prelude;

inline std::string getSoundPath(std::string fileName) {
    auto mod = Mod::get();
    auto resourcesPath = mod->getResourcesDir();
    auto path = resourcesPath / fileName;
    return string::pathToString(path);
}

inline void showSearchArea() {
    auto isInsideLevel = GJBaseGameLayer::get() != nullptr;
    if (isInsideLevel) {
        // let's maybe consider telling the user why it cannot be summoned
        Notification::create("Cannot summon command palette, please exit the level!", NotificationIcon::Error)->show();
        return;
    }

    auto isToggled = CommandPalette::Core::SearchAreaManager::getIsToggled();

    if (!isToggled) {
        auto area = CommandPalette::UI::SearchArea::create();
        OverlayManager::get()->addChild(area);
        CommandPalette::Core::SearchAreaManager::setIsToggled(true);
    }
}

#endif