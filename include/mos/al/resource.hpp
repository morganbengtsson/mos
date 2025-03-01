#pragma once

#include <AL/al.h>

#include <chrono>
#include <functional>

namespace mos::al {

/** General resource living on the APU */
class Resource {
public:
  using Gen_function = std::function<void(ALsizei, ALuint *)>;
  using Delete_function = std::function<void(ALsizei, const ALuint *)>;
  using Time_point = std::chrono::time_point<std::chrono::system_clock,
                                             std::chrono::nanoseconds>;

  Resource(const Gen_function &gen_function, Delete_function delete_function);
  Resource(const Resource &Resource) = delete;
  Resource(Resource &&Resource) noexcept;
  ~Resource();

  auto operator=(const Resource resource) -> Resource & = delete;
  auto operator=(Resource &&resource) noexcept -> Resource &;

  Gen_function gen_function;
  Delete_function delete_function;

  Time_point modified;
  ALuint id{0};

private:
  void release();
};
} // namespace mos::al
