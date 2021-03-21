#ifndef VISUALIZATION_GOOM_GOOM_CONTROL_H
#define VISUALIZATION_GOOM_GOOM_CONTROL_H

#include "goom_config.h"

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

namespace GOOM
{

class AudioSamples;
class PixelBuffer;

class GoomControl
{
public:
  static auto GetRandSeed() -> uint64_t;
  static void SetRandSeed(uint64_t seed);

  GoomControl() noexcept = delete;
  GoomControl(uint32_t resx, uint32_t resy, std::string resourcesDirectory);
  ~GoomControl() noexcept;
  GoomControl(const GoomControl&) noexcept = delete;
  GoomControl(GoomControl&&) noexcept = delete;
  auto operator=(const GoomControl&) -> GoomControl& = delete;
  auto operator=(GoomControl&&) -> GoomControl& = delete;

  [[nodiscard]] auto GetResourcesDirectory() const -> const std::string&;
  void SetResourcesDirectory(const std::string& dirName);

  void SetScreenBuffer(PixelBuffer& buff);
  void Start();

  /*
   * Update the next goom frame
   *
   * songTitle:
   *      - empty if it is not the start of the song
   *      - only have a value at the start of the song
   */
  void Update(const AudioSamples& s,
              float fps,
              const std::string& songTitle,
              const std::string& message);

  void Finish();

private:
  class GoomControlImpl;
  const std::unique_ptr<GoomControlImpl> m_controller;
};

} // namespace GOOM
#endif
