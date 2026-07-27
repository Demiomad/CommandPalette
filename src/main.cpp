#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_MACOS)
$on_game(Loaded) {
    listenForKeybindSettingPresses("activation-keybind", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down && !repeat) {
            showSearchArea();
        }
    });
}
#else
class $modify(CmdPalMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("CmdPal_mobileIcon.png"_spr),
			this,
			menu_selector(CmdPalMenuLayer::onCmdPalButton)
		);

		auto menu = this->getChildByID("bottom-menu");

		menu->addChild(myButton);
		myButton->setID("my-button"_spr);
		menu->updateLayout();

        return true;
    }

    void onCmdPalButton(CCObject*) {
        showSearchArea();
    }
};
#endif