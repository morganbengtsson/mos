#include <experimental/menu.hpp>
#include <iostream>

namespace mos {

Menu::Menu() {

}

Menu::Menu(const std::initializer_list<Button> &buttons) : buttons_(buttons.begin(), buttons.end()) {

}

Menu::~Menu() {
}

void Menu::add(const Button &button) {
    buttons_.push_back(button);
}

Model Menu::model() {
    mos::Model out;
    for (int i = 0; i < buttons_.size(); i++) {
        auto model = buttons_[i].model();
        model.transform(glm::translate(model.transform(), glm::vec3(0.0f, -i * buttons_[i].height(), 0.0f)));
        out.models.push_back(model);
    }
    return out;
}

Button & Menu::selected() {
    auto it = std::find_if(buttons_.begin(), buttons_.end(), [](mos::Button & button){
            return button.selected();});
    return *it;
}

void Menu::select_next() {
    select_next(buttons_.begin(), buttons_.end());
}

void Menu::select_previos() {
    select_next(buttons_.rbegin(), buttons_.rend());
}
}
