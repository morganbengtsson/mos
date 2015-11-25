#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <functional>
#include "../graphics/model.hpp"
#include "../graphics/text.hpp"

namespace mos {
class Button {
public:
    using Callback = std::function<void()>;
    enum class State {IDLE, SELECTED, CLICKED};
    Button(const Text & text, const State & state = State::IDLE);
    ~Button();
    Model model();
    void state(const State & state);
    float height() const;
    float width() const;
    State state() const;
    void click_callback(const Callback & callback);
private:
    Text text_;
    Model rectangle_;
    float padding_;
    State state_;
    std::shared_ptr<mos::Material> light_material_;
    std::shared_ptr<mos::Material> dark_material_;
    std::function<void()> click_callback_;
};
}
#endif // BUTTON_HPP
