#ifndef VISUALIZATION_GOOM_STATS_STARS_STATS_H
#define VISUALIZATION_GOOM_STATS_STARS_STATS_H

#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <cstdint>

namespace GOOM
{

class StarsStats
{
public:
  StarsStats() = default;

  void Reset();
  void Log(const GoomStats::LogStatsValueFunc& logVal) const;
  void AddBombButTooManyStars();
  void AddBomb();
  void SoundEventOccurred();
  void NoFxChosen();
  void FireworksFxChosen();
  void RainFxChosen();
  void FountainFxChosen();
  void UpdateStars();
  void DeadStar();
  void RemovedDeadStars(uint32_t val);
  void SetLastNumActive(uint32_t val);
  void SetLastMaxStars(uint32_t val);
  void SetLastMaxStarAge(uint32_t val);

private:
  uint32_t m_numAddBombButTooManyStars = 0;
  uint32_t m_numAddBombs = 0;
  uint32_t m_numSoundEvents = 0;
  uint32_t m_numNoFxChosen = 0;
  uint32_t m_numFireworksFxChosen = 0;
  uint32_t m_numRainFxChosen = 0;
  uint32_t m_numFountainFxChosen = 0;
  uint32_t m_numUpdateStars = 0;
  uint32_t m_numDeadStars = 0;
  uint32_t m_numRemovedStars = 0;
  uint32_t m_lastNumActive = 0;
  uint32_t m_lastMaxStars = 0;
  uint32_t m_lastMaxStarAge = 0;
};

} // namespace GOOM

#endif //VISUALIZATION_GOOM_STATS_STARS_STATS_H
