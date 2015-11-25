#ifndef MENU_HPP
#define MENU_HPP

#include "button.hpp"
#include <initializer_list>
#include <vector>

namespace   mos {
class Menu
{
public:
    using Buttons = std::vector<Button>;
    Menu();
    Menu(const std::initializer_list<Button> & buttons);
    void add(const Button & button);
    mos::Model model();
    void select_next();
    void select_previos();
    ~Menu();
private:
    Buttons buttons_;
};
}
#endif // MENU_HPP
