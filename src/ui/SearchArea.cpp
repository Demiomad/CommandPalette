#include "SearchArea.hpp"
#include "../core/CommandRegistry.hpp"
#include "../core/CommandParser.hpp"
#include "../core/SearchAreaManager.hpp"
#include "../Utils.hpp"
#include "../BlurAPI.hpp"

using namespace geode::prelude;

bool CommandPalette::UI::SearchArea::init() {
    if (!CCNode::init()) return false;

    m_registry = CommandPalette::Core::CommandRegistry();

    auto mod = Mod::get();
    auto playSoundOnOpen = mod->getSettingValue<bool>("play-sound-on-open");
    if (playSoundOnOpen) {
        auto engine = FMODAudioEngine::sharedEngine();

        // @geode-ignore(unknown-resource)
        auto soundPath = getSoundPath("open.ogg");
        engine->playEffect(soundPath);
    }

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = createLayerBG();
    bg->setColor({ 0, 0, 0 });
    bg->setOpacity(100);
    this->addChild(bg);
    
    auto isBlurEnabled = mod->getSettingValue<bool>("blur-behind");
    if (isBlurEnabled && BlurAPI::isBlurAPIEnabled())
        BlurAPI::addBlur(bg);

    auto textInput = TextInput::create(350.f, "Enter a command...");
    textInput->setPosition({ (winSize.width / 2) - 25.f, winSize.height - 35.f });
    textInput->setID("command-input"_spr);
    textInput->setTextAlign(TextInputAlign::Left);
    textInput->focus();

    m_textInput = textInput->getInputNode();
    this->addChild(textInput);

    auto mainMenu = CCMenu::create();
    mainMenu->setPosition(CCPointZero);
    mainMenu->setContentSize(winSize);
    mainMenu->setID("main-menu"_spr);
    this->addChild(mainMenu);

    auto closeBtnSprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    auto closeBtn = CCMenuItemSpriteExtra::create(
        closeBtnSprite,
        this,
        menu_selector(CommandPalette::UI::SearchArea::onClose)
    );

    closeBtn->setPosition({ winSize.width - 25.f, 25.f });
    closeBtn->setID("close-button"_spr);
    mainMenu->addChild(closeBtn);

    auto submitBtnSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    submitBtnSprite->setScale(0.45f);

    auto submitBtn = CCMenuItemSpriteExtra::create(
        submitBtnSprite,
        this,
        menu_selector(CommandPalette::UI::SearchArea::onSubmit)
    );

    submitBtn->setID("submit-button"_spr);
    submitBtn->setPosition({ winSize.width - 105.f, winSize.height - 35.f });
    mainMenu->addChild(submitBtn);

    registerCommands();
    return true;
}

void CommandPalette::UI::SearchArea::registerCommands() {
    auto loader = geode::Loader::get();

    m_registry.createCommand("gd", "Utilities for the game", [loader](const auto&) {
        auto gameVer = loader->getGameVersion();

        auto infoText = fmt::format("You are on Geometry Dash {}", gameVer);
        Notification::create(infoText, NotificationIcon::Info)->show();
    });

    m_registry.addSubcommand("gd", "quit", "Closes the game", [](const auto&) {
        game::exit(true);
    });

    m_registry.addSubcommand("gd", "restart", "Restarts the game", [](const auto&) {
        game::restart(true);
    });

    m_registry.addSubcommand("gd", "quit-nosave", "Closes the game without saving any data", [](const auto&) {
        game::exit(false);
    });

    m_registry.addSubcommand("gd", "restart-nosave", "Restarts the game without saving any data", [](const auto&) {
        game::restart(false);
    });

    m_registry.addSubcommand("gd", "settings", "Opens the game's settings", [](const auto&) {
        auto layer = OptionsLayer::create();
        layer->showLayer(false);
    });

    m_registry.addSubcommand("gd", "achievements", "Displays your achievements", [](const auto&) {
        auto layer = AchievementsLayer::create();
        layer->showLayer(false);
    });

    m_registry.createCommand("geode", "Utilities for Geode", [loader](const auto&) {
        auto loaderVer = loader->getVersion().toVString();
        auto installedMods = loader->getAllMods().size();

        auto infoText = fmt::format(
            "Geode Version: <cy>{}</c>\n<cy>{}</c> mods installed",
            loaderVer,
            installedMods
        );

        FLAlertLayer::create("Geode", infoText, "OK")->show();
    });

    m_registry.addSubcommand("geode", "settings", "Opens a mod's settings", [loader](const std::vector<std::string>& args) {
        if (args.empty()) {
            Notification::create("Usage: geode mod <mod-id>", NotificationIcon::Info)->show();
            return;
        }

        auto modId = args.at(0);        
        auto mod = loader->getInstalledMod(modId);

        if (mod == nullptr) {
            auto notifText = fmt::format("Mod with ID {} not found", modId);
            Notification::create(notifText, NotificationIcon::Error)->show();
            return;
        }

        geode::openSettingsPopup(mod);
    });

    m_registry.addSubcommand("geode", "fetch", "Fetches information about a mod from the Geode index", [](const std::vector<std::string>& args) {
        if (args.empty()) {
            Notification::create("Usage: geode fetch <mod-id>", NotificationIcon::Info)->show();
            return;
        }

        auto modId = args.at(0);     
        geode::openInfoPopup(modId);
    });

    m_registry.addSubcommand("geode", "install", "Installs a mod from a .geode file", [](const auto&) {
        auto geodeFileFilter = file::FilePickOptions::Filter {
            .description = "Geode Mod Files",
            .files = { "*.geode " }
        };

        auto pickOpts = file::FilePickOptions {
            .filters = { geodeFileFilter }
        };

        async::spawn(
            file::pick(file::PickMode::OpenFile, pickOpts),
            [](Result<std::optional<std::filesystem::path>> result) {
                if (result.isOk()) {
                    auto opt = result.unwrap();

                    if (opt) {
                        auto path = opt.value();
                        auto fileName = path.filename();
                        auto destination = dirs::getModsDir() / fileName;
                        log::debug("file path: {}", path);
                        log::debug("destination: {}", destination);
                        
                        std::error_code copyErrCode;
                        std::filesystem::copy_file(path, destination, copyErrCode);

                        if (copyErrCode) {
                            auto errText = fmt::format("Failed to copy file: {}", copyErrCode.message());
                            Notification::create(errText, NotificationIcon::Error)->show();
                        }
                        else {
                            geode::createQuickPopup(
                                "Success",
                                "<cg>Successfully</c> installed the selected mod!\nWould you like to <cy>restart the game?</c>",
                                "No", "Yes",
                                [](auto, bool btn2) {
                                    if (btn2) {
                                        game::restart(true);
                                    }
                                }
                            );
                        }
                    }
                    else {
                        Notification::create("Operation canceled.", NotificationIcon::Info)->show();
                    }
                }
            }
        );
    });

    m_registry.addSubcommand("geode", "id", "Gets a mod's ID by name", [](const std::vector<std::string>& args) {
        if (args.empty()) {
            Notification::create("Usage: geode id <mod-name>", NotificationIcon::Info)->show();
            return;
        }

        auto loader = geode::Loader::get();
        auto modName = string::join(args, " ");    
        auto mods = loader->getAllMods();

        for (const auto& mod : mods) {
            if (mod->getName() == modName) {
                Notification::create(mod->getID(), NotificationIcon::Info)->show();
                return;
            }
        }

        auto errText = fmt::format("Mod \"{}\" not found", modName);
        Notification::create(errText, NotificationIcon::Error)->show();
    });

    m_registry.createCommand("help", "Displays all available commands (and their subcommands)", [this](const auto&) {
        auto mdString = m_registry.createMarkdownString();
        MDPopup::create("Command List", mdString, "OK")->show();
    });

    m_registry.createCommand("level", "Commands for interacting with levels", [](const auto&) {
        Notification::create("Usage: level <local/search> <level-name>", NotificationIcon::Info)->show();
    });

    m_registry.addSubcommand("level", "local", "Displays information about a level that you created", [](const std::vector<std::string>& args) {
        if (args.empty()) {
            Notification::create("Usage: level local <level-name>", NotificationIcon::Info)->show();
            return;
        }

        auto name = string::join(args, " ");
        auto glm = GameLevelManager::get();
        auto level = glm->getLocalLevelByName(name);

        if (level == nullptr) {
            auto errText = fmt::format("Level {} not found", name);
            Notification::create(errText, NotificationIcon::Info)->show();
            return;
        }

        auto scene = EditLevelLayer::scene(level);
        auto transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->pushScene(transition);
    });

    m_registry.addSubcommand("level", "search", "Searches for a level on the servers", [](const std::vector<std::string>& args) {
        if (args.empty()) {
            Notification::create("Usage: level search <query>", NotificationIcon::Info)->show();
            return;
        }

        auto query = string::join(args, " ");
        auto glm = GameLevelManager::get();
        auto searchObj = GJSearchObject::create(SearchType::Search, query);
        auto scene = LevelBrowserLayer::scene(searchObj);
        auto transition = CCTransitionFade::create(0.5f, scene);

        CCDirector::sharedDirector()->pushScene(transition);
    });
}

void CommandPalette::UI::SearchArea::onClose(CCObject*) {
    auto isToggled = CommandPalette::Core::SearchAreaManager::getIsToggled();
    if (!isToggled)
        return;

    auto overlayMgr = OverlayManager::get();
    overlayMgr->removeChild(this);
    CommandPalette::Core::SearchAreaManager::setIsToggled(false);
}

void CommandPalette::UI::SearchArea::onSubmit(CCObject*) {
    auto input = m_textInput->getString();

    if (input.empty())
        return;

    auto trimmed = string::trim(input);
    auto isSubcmd = string::contains(trimmed, ' ');

    if (isSubcmd) {
        auto parsed = CommandPalette::Core::CommandParser::parseSubcommand(trimmed);
        auto name = parsed.m_name;
        auto subcommand = parsed.m_subcommand;
        auto args = parsed.m_args;

        if (!m_registry.run(name, subcommand, args)) {
            auto notifText = fmt::format("Unknown subcommand: {}", subcommand);
            Notification::create(notifText, NotificationIcon::Error)->show();
        }
    }
    else {
        auto parsed = CommandPalette::Core::CommandParser::parse(trimmed);
        auto name = parsed.m_name;
        auto args = parsed.m_args;

        if (!m_registry.run(name, args)) {
            auto notifText = fmt::format("Unknown command: {}", name);
            Notification::create(notifText, NotificationIcon::Error)->show();
        }
    }

    onClose(nullptr);
}