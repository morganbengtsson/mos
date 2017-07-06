#ifndef MOS_PID_HPP
#define MOS_PID_HPP
namespace mos {

template <class T>
class Pid {
public:
  Pid(const float Kp = 1.0f, const float Ki = 0.0f, const float Kd = 0.1f) : Kp(Kp), Ki(Ki), Kd(Kd), prev_error(0.0f) {
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

#endif //MOS_PID_HPP
