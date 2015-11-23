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
    float height() const;
    float width() const;
private:
    Text text_;
    Model rectangle_;
    float padding_;
};
}
#endif // BUTTON_HPP
