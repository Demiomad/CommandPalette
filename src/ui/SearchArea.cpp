#include "SearchArea.hpp"
#include "../core/commands/CommandRegistry.hpp"
#include "../core/commands/InputParser.hpp"
#include "../core/SearchAreaManager.hpp"
#include "../Utils.hpp"
#include "../BlurAPI.hpp"

using namespace CommandPalette::Core;
using namespace geode::prelude;

bool CommandPalette::UI::SearchArea::init()
{
    if (!CCNode::init()) return false;

    auto mod = Mod::get();
    auto playSoundOnOpen = mod->getSettingValue<bool>("play-sound-on-open");
    if (playSoundOnOpen)
    {
        auto engine = FMODAudioEngine::sharedEngine();

        // @geode-ignore(unknown-resource)
        auto soundPath = getSoundPath("open.ogg");
        engine->playEffect(soundPath);
    }

    m_registry = Commands::CommandRegistry();

    setKeypadEnabled(true);

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
    textInput->setCommonFilter(CommonFilter::Any);

    m_textInput = textInput->getInputNode();
    this->addChild(textInput);

    auto mainMenu = CCMenu::create();
    mainMenu->setPosition(CCPointZero);
    mainMenu->setContentSize(winSize);
    mainMenu->setID("main-menu"_spr);
    this->addChild(mainMenu);

    #if defined(GEODE_IS_MOBILE)
    auto closeBtnSprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    auto closeBtn = CCMenuItemSpriteExtra::create(
        closeBtnSprite,
        this,
        menu_selector(CommandPalette::UI::SearchArea::onClose)
    );

    closeBtn->setPosition({ winSize.width - 25.f, 25.f });
    closeBtn->setID("close-button"_spr);
    mainMenu->addChild(closeBtn);
    #endif

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

    auto helpfulTip = CCLabelBMFont::create("Enter 'help' to view commands!", "chatFont.fnt");
    helpfulTip->setID("helpful-tip"_spr);
    helpfulTip->setPosition(winSize / 2);
    helpfulTip->setOpacity(127);
    this->addChild(helpfulTip);

    registerCommands();
    return true;
}

void CommandPalette::UI::SearchArea::registerCommands()
{
    auto loader = geode::Loader::get();
    
    m_registry.registerCommand(
        "geometry-dash",
        "Utilities for the game",
        "geometry-dash -r|-q [-ns]",
        { "gd", "gmd", "geo-dash" },
        [loader](const Commands::CommandArgs& args)
        {
            auto dontSave = !Commands::CommandRegistry::hasFlag(args, "ns");
            auto quit = Commands::CommandRegistry::hasFlag(args, "q");
            auto restart = Commands::CommandRegistry::hasFlag(args, "r");

            if (restart)
                game::restart(dontSave);
            else if (quit)
                game::exit(dontSave);
            else
            {
                auto notifText = fmt::format("You are on Geometry Dash {}", loader->getGameVersion());
                Notification::create(notifText, NotificationIcon::Info)->show();
            }

            return true;
        }
    );

    m_registry.registerCommand(
        "loader",
        "Utilities for the mod loader",
        "loader [options] [mod-id]\n\n"
        "-g - get a mod by its id, mod-id is required in this case\n\n"
        "-i - install a mod from a file\n\n"
        "-c - open crashlog folder\n\n"
        "-l - open logs folder",
        { "geode" },
        [loader](const Commands::CommandArgs& args) {
            auto getMod = Commands::CommandRegistry::hasFlag(args, "g");
            auto installMod = Commands::CommandRegistry::hasFlag(args, "i");
            auto openCrashlogs = Commands::CommandRegistry::hasFlag(args, "c");
            auto openLogs = Commands::CommandRegistry::hasFlag(args, "l");
            
            if (getMod)
            {
                if (args.m_positional.empty())
                {
                    log::error("Mod ID not specified");
                    return false;
                }

                auto modId = args.m_positional.at(0);
                geode::openInfoPopup(modId);
            }
            else if (installMod)
            {
                auto geodeFileFilter = file::FilePickOptions::Filter
                {
                    .description = "Geode Mod Files",
                    .files = { "*.geode " }
                };

                auto pickOpts = file::FilePickOptions
                {
                    .filters = { geodeFileFilter }
                };

                async::spawn(
                    file::pick(file::PickMode::OpenFile, pickOpts),
                    [](Result<std::optional<std::filesystem::path>> result)
                    {
                        if (result.isOk())
                        {
                            auto opt = result.unwrap();

                            if (opt)
                            {
                                auto path = opt.value();
                                auto fileName = path.filename();
                                auto destination = dirs::getModsDir() / fileName;
                                
                                std::error_code copyErrCode;
                                std::filesystem::copy_file(path, destination, copyErrCode);

                                if (copyErrCode)
                                {
                                    auto errText = fmt::format("Failed to copy file: {}", copyErrCode.message());
                                    Notification::create(errText, NotificationIcon::Error)->show();
                                }
                                else
                                {
                                    geode::createQuickPopup(
                                        "Success",
                                        "<cg>Successfully</c> installed the selected mod!\nWould you like to <cy>restart the game?</c>",
                                        "No", "Yes",
                                        [](auto, bool btn2)
                                        {
                                            if (btn2) game::restart(true);
                                        }
                                    );
                                }
                            }
                        }
                        else
                        {
                            log::error("Error: {}", result.err());
                            return false;
                        }

                        return true;
                    }
                );
            }
            else if (openCrashlogs)
            {
                file::openFolder(dirs::getCrashlogsDir());
            }
            else if (openLogs)
            {
                file::openFolder(dirs::getGeodeLogDir());
            }
            else
            {
                auto loaderVer = loader->getVersion().toVString();
                auto installedMods = loader->getAllMods().size();

                auto infoText = fmt::format(
                    "Geode Version: <cy>{}</c>\n<cy>{}</c> mods installed",
                    loaderVer,
                    installedMods
                );

                FLAlertLayer::create("Geode", infoText, "OK")->show();
            }

            return true;
        }
    );

    m_registry.registerCommand(
        "mod",
        "Utilities for interacting with Geode mods",
        "mod [options] <mod-id>\n\n"
        "-s - open mod settings\n\n"
        "-c - open mod changelog\n\n"
        "-d - disable the mod\n\n"
        "-e - enable the mod",
        { "geode-mod" },
        [loader](const Commands::CommandArgs& args)
        {
            if (args.m_positional.empty())
            {
                log::error("Mod ID not specified");
                return false;
            }

            auto modId = args.m_positional.at(0);
            auto openModSettings = Commands::CommandRegistry::hasFlag(args, "s");
            auto openModChangelog = Commands::CommandRegistry::hasFlag(args, "c");
            auto disableMod = Commands::CommandRegistry::hasFlag(args, "d");
            auto enableMod = Commands::CommandRegistry::hasFlag(args, "e");
            auto mod = loader->getInstalledMod(modId);

            if (mod == nullptr)
            {
                log::error("Mod {} not found", modId);
                return false;
            }

            if (openModSettings)
                geode::openSettingsPopup(mod);
            else if (openModChangelog)
                geode::openChangelogPopup(mod);
            else if (enableMod || disableMod)
            {
                if (modId == "geode.loader")
                {
                    log::error("why are you trying to disable the geode loader");
                    Notification::create("You can't do that!", NotificationIcon::Warning)->show();
                    return true;
                }

                if (disableMod && mod->isOrWillBeEnabled())
                {
                    auto disableResult = mod->disable();
                    if (disableResult.isErr())
                    {
                        log::error("Failed to disable mod: {}", disableResult.err());
                        return false;
                    }
                }
                else if (enableMod && !mod->isOrWillBeEnabled())
                {
                    auto enableResult = mod->enable();
                    if (enableResult.isErr())
                    {
                        log::error("Failed to enable mod: {}", enableResult.err());
                        return false;
                    }
                }

                geode::createQuickPopup(
                    "Success",
                    "Enabling or disabling a mod usually requires a <cy>game restart.</c>\nWould you like to <cy>restart the game?</c>",
                    "No", "Yes",
                    [](auto, bool btn2)
                    {
                        if (btn2) game::restart(true);
                    }
                );
            }
            else
            {
                Notification::create("Bad usage", NotificationIcon::Warning)->show();    
            }

            return true;
        }
    );

    m_registry.registerCommand(
        "help",
        "Displays all commands",
        "help",
        { "lscmd", "commands" },
        [this](const auto&) {
            auto mdString = m_registry.createMarkdownString();
            MDPopup::create("Commands", mdString, "OK")->show();
            return true;
        }
    );

    m_registry.registerCommand(
        "level",
        "Allows you to view/search levels.",
        "level -s|-l <name>\n\n"
        "-s - search an online level\n\n"
        "-l - search a level that you created (local level)",
        { "lvl", "lv", "map" },
        [](const Commands::CommandArgs& args)
        {
            if (args.m_positional.empty())
            {
                log::error("Level name not specified");
                return false;
            }

            auto searchOnline = Commands::CommandRegistry::hasFlag(args, "s");
            auto lvName = string::join(args.m_positional, " ");
            auto searchLocal = Commands::CommandRegistry::hasFlag(args, "l");

            if (searchOnline)
            {
                auto searchObj = GJSearchObject::create(SearchType::Search, lvName);
                auto scene = LevelBrowserLayer::scene(searchObj);
                auto transition = CCTransitionFade::create(0.5f, scene);
                CCDirector::sharedDirector()->pushScene(transition);
            }
            else if (searchLocal)
            {
                auto glm = GameLevelManager::get();
                auto level = glm->getLocalLevelByName(lvName);

                if (level == nullptr)
                {
                    log::error("Level {} not found", lvName);
                    return false;
                }

                auto scene = EditLevelLayer::scene(level);
                auto transition = CCTransitionFade::create(0.5f, scene);
                CCDirector::sharedDirector()->pushScene(transition);
            }
            else
            {
                log::error("Please specify either -s or -l");
                return false;
            }

            return true;
        }
    );
}

void CommandPalette::UI::SearchArea::onClose(CCObject*)
{
    auto isToggled = CommandPalette::Core::SearchAreaManager::getIsToggled();
    if (!isToggled)
        return;

    auto overlayMgr = OverlayManager::get();
    overlayMgr->removeChild(this);
    CommandPalette::Core::SearchAreaManager::setIsToggled(false);
}

void CommandPalette::UI::SearchArea::onSubmit(CCObject*)
{
    auto input = m_textInput->getString();

    if (input.empty())
        return;

    auto trimmed = string::trim(input);
    if (!m_registry.executeCommandFromInput(trimmed))
    {
        Notification::create(
            "The command failed to execute, check console (or logs) for details.",
            NotificationIcon::Error
        )->show();
    }

    onClose(nullptr);
}

void CommandPalette::UI::SearchArea::keyBackClicked()
{
    onClose(nullptr);
}