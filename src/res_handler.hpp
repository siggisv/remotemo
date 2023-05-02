#ifndef REMOTEMO_SRC_RES_HANDLER_HPP
#define REMOTEMO_SRC_RES_HANDLER_HPP

#include <utility>
#include <SDL.h>

namespace remotemo {
template<typename T> class Res_handler {
public:
  constexpr Res_handler() noexcept = default;
  constexpr explicit Res_handler(T* resource, bool is_owned = true) noexcept
      : m_resource(resource), m_is_owned(is_owned)
  {}
  virtual ~Res_handler() noexcept;
  constexpr Res_handler(Res_handler&& other) noexcept;
  constexpr Res_handler& operator=(Res_handler&& other) noexcept;

  Res_handler(const Res_handler&) = delete;
  Res_handler& operator=(const Res_handler&) = delete;

  [[nodiscard]] T* const& res() const { return m_resource; }
  [[nodiscard]] bool is_owned() const { return m_is_owned; }

protected:
  void res(T* resource) { m_resource = resource; }
  void is_owned(bool is_owned) { m_is_owned = is_owned; }

private:
  void destroy_res(SDL_Window* window) { SDL_DestroyWindow(window); }
  void destroy_res(SDL_Renderer* renderer) { SDL_DestroyRenderer(renderer); }
  void destroy_res(SDL_Texture* texture) { SDL_DestroyTexture(texture); }

  T* m_resource {nullptr};
  bool m_is_owned {false};
};

template<typename T> Res_handler<T>::~Res_handler() noexcept
{
  if (m_is_owned && m_resource != nullptr) {
    destroy_res(m_resource);
  }
}

template<typename T>
constexpr Res_handler<T>::Res_handler(Res_handler&& other) noexcept
    : m_resource(other.m_resource), m_is_owned(other.m_is_owned)
{
  other.m_is_owned = false;
  other.m_resource = nullptr;
}

template<typename T>
constexpr Res_handler<T>& Res_handler<T>::operator=(
    Res_handler&& other) noexcept
{
  std::swap(m_is_owned, other.m_is_owned);
  std::swap(m_resource, other.m_resource);
  return *this;
}
} // namespace remotemo
#endif // REMOTEMO_SRC_RES_HANDLER_HPP
