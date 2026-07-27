#ifndef SEARCHAREAMANAGER_HPP
#define SEARCHAREAMANAGER_HPP

#include <Geode/Geode.hpp>

namespace CommandPalette::Core {
    class SearchAreaManager {
        private:
            static bool s_isToggled;

        public:
            static bool getIsToggled();
            static void setIsToggled(bool value);
    };
}

#endif