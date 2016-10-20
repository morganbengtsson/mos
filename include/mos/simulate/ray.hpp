#ifndef RAY_HPP
#define RAY_HPP

#include <glm/glm.hpp>
namespace mos{
class Ray {
public:
  Ray(const glm::vec3 &origin,
      const glm::vec3 &direction);
  glm::vec3 origin;
  glm::vec3 direction() const;
  void direction(const glm::vec3 &direction);
private:
  glm::vec3 direction_;
};
}
#endif // RAY_HPP
