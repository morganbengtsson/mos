#pragma once


#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

#include <functional>
#include <chrono>

namespace mos::al {

class Resource
{
public:
  using Gen_function = std::function<void(ALsizei, ALuint *)>;
  using Delete_function = std::function<void(ALsizei, const ALuint *)>;
  using Time_point = std::chrono::time_point<std::chrono::system_clock,
                                             std::chrono::nanoseconds>;

  Resource(const Gen_function &gen_function,
           const Delete_function &delete_function);
  Resource(const Resource &Resource) = delete;
  Resource(Resource &&Resource) noexcept;
  ~Resource();

  Resource &operator=(const Resource &buffer) = delete;
  Resource &operator=(Resource &&buffer) noexcept;

  Gen_function gen_function;
  Delete_function delete_function;

  Time_point modified;
  ALuint id{0};

private:
  void release();
};
}
