#ifndef SEARCHAREA_HPP
#define SEARCHAREA_HPP

#include <Geode/Geode.hpp>
#include "../core/CommandRegistry.hpp"

namespace CommandPalette::UI {
    class SearchArea : public cocos2d::CCNode {
        public:
            CREATE_FUNC(SearchArea);

        protected:
            CCTextInputNode* m_textInput;
            LoadingCircle* m_loadingCircle;

            bool init();
            void registerCommands();
            
            void onClose(CCObject* sender);
            void onSubmit(CCObject* sender);

        private:
            CommandPalette::Core::CommandRegistry m_registry;
    };
}

#endif