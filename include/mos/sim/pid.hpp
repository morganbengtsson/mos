#pragma once
namespace mos {
namespace sim {

template<class T>
class Pid {
public:
  Pid(const T &error, const float Kp = 1.0f, const float Ki = 0.0f, const float Kd = 0.1f)
      : Kp(Kp), Ki(Ki), Kd(Kd), prev_error(error), P(error), I(error), D(error) {
  }
  T get(const T error, float dt) {
    P = error;
    I += P * dt;
    D = (P - prev_error) / dt;

    prev_error = error;
    return P * Kp + I * Ki + D * Kd;
  }
  float Kp, Ki, Kd;
private:
  T P, I, D;
  T prev_error;
};
}
}

