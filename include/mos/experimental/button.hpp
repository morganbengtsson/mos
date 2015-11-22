#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../graphics/model.hpp"
#include "../graphics/text.hpp"
namespace mos {
class Button {
public:
    Button(const Text & text);
    ~Button();
    Model model();
    Text text_;
    Model rectangle_;
};
}
#endif // BUTTON_HPP
