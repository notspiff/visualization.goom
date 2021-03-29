#include "stars_stats.h"

#include "goom/goom_config.h"
#include "goom/goom_stats.h"

#include <cstdint>

namespace GOOM
{

void StarsStats::Reset()
{
  m_numAddBombButTooManyStars = 0;
  m_numAddBombs = 0;
  m_numSoundEvents = 0;
  m_numNoFxChosen = 0;
  m_numFireworksFxChosen = 0;
  m_numRainFxChosen = 0;
  m_numFountainFxChosen = 0;
  m_numUpdateStars = 0;
  m_numDeadStars = 0;
  m_numRemovedStars = 0;
}

void StarsStats::Log(const GoomStats::LogStatsValueFunc& logVal) const
{
  const constexpr char* MODULE = "Stars";

  logVal(MODULE, "lastNumActive", m_lastNumActive);
  logVal(MODULE, "lastMaxStars", m_lastMaxStars);
  logVal(MODULE, "lastMaxStarAge", m_lastMaxStarAge);
  logVal(MODULE, "numUpdateStars", m_numUpdateStars);
  logVal(MODULE, "numSoundEvents", m_numSoundEvents);
  logVal(MODULE, "numAddBombButTooManyStars", m_numAddBombButTooManyStars);
  logVal(MODULE, "numAddBombs", m_numAddBombs);
  logVal(MODULE, "numNoFxChosen", m_numNoFxChosen);
  logVal(MODULE, "numFireworksFxChosen", m_numFireworksFxChosen);
  logVal(MODULE, "numRainFxChosen", m_numRainFxChosen);
  logVal(MODULE, "numFountainFxChosen", m_numFountainFxChosen);
  logVal(MODULE, "numDeadStars", m_numDeadStars);
  logVal(MODULE, "numRemovedStars", m_numRemovedStars);
}

void StarsStats::UpdateStars()
{
  m_numUpdateStars++;
}

void StarsStats::AddBombButTooManyStars()
{
  m_numAddBombButTooManyStars++;
}

void StarsStats::AddBomb()
{
  m_numAddBombs++;
}

void StarsStats::SoundEventOccurred()
{
  m_numSoundEvents++;
}

void StarsStats::NoFxChosen()
{
  m_numNoFxChosen++;
}

void StarsStats::FireworksFxChosen()
{
  m_numFireworksFxChosen++;
}

void StarsStats::RainFxChosen()
{
  m_numRainFxChosen++;
}

void StarsStats::FountainFxChosen()
{
  m_numFountainFxChosen++;
}

void StarsStats::DeadStar()
{
  m_numDeadStars++;
}

void StarsStats::RemovedDeadStars(uint32_t val)
{
  m_numRemovedStars += val;
}

void StarsStats::SetLastNumActive(uint32_t val)
{
  m_lastNumActive = val;
}

void StarsStats::SetLastMaxStars(uint32_t val)
{
  m_lastMaxStars = val;
}

void StarsStats::SetLastMaxStarAge(uint32_t val)
{
  m_lastMaxStarAge = val;
}

} // namespace GOOM
