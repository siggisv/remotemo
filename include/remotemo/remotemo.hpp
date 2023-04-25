#ifndef REMOTEMO_REMOTEMO_HPP
#define REMOTEMO_REMOTEMO_HPP

#include <optional>
#include <memory>

#include "remotemo/common_types.hpp"
#include "remotemo/config.hpp"

#include <SDL.h>

namespace remotemo {
class Engine;
class Remotemo {
  friend std::optional<Remotemo> create(const Config& config);

public:
  ~Remotemo() noexcept;
  Remotemo(Remotemo&& other) noexcept;
  Remotemo& operator=(Remotemo&& other) noexcept;

  Remotemo(const Remotemo&) = delete;
  Remotemo& operator=(const Remotemo&) = delete;

private:
  constexpr Remotemo() noexcept = default;
  bool initialize(const Config& config);

  std::unique_ptr<Engine> m_engine {};
};

std::optional<Remotemo> create();
std::optional<Remotemo> create(const Config& config);
} // namespace remotemo
#endif // REMOTEMO_REMOTEMO_HPP
