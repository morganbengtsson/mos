#pragma once
namespace mos::sim {

template<class T>
/** Pid controller. */
class Pid {
public:
  Pid(const T &error = T(0), const float Kp = 1.0F, const float Ki = 0.0F,
      const float Kd = 0.1F)
      : Kp(Kp), Ki(Ki), Kd(Kd), prev_error(error), P(error), I(error),
        D(error) {}
  auto get(const T error, float dt) -> T {
    P = error;
    I += P * dt;
    D = (P - prev_error) / dt;
    if (dt == 0){
      D = T(0);
    }

    prev_error = error;
    return (P * Kp) + (I * Ki) + (D * Kd);
  }
  float Kp, Ki, Kd;
private:
  T P, I, D;
  T prev_error;
};
}


