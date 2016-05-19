#ifndef MOS_BOXES_BATCH_HPP
#define MOS_BOXES_BATCH_HPP

#include <mos/graphics/render_box.hpp>
#include <vector>

namespace mos {
class BoxesBatch {
public:
  template <class T>
  BoxesBatch(T begin, T end, const glm::mat4 &view, const glm::mat4 &projection)
      : boxes(begin, end), view(view), projection(projection) {}
  const std::vector<RenderBox> boxes;
  const glm::mat4 view;
  const glm::mat4 projection;
};
}

#endif // MOS_BOXES_BATCH_HPP
