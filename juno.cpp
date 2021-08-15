#include "juno.h"

#include <limits>
#include <cmath>

namespace juno {

double seconds(double value)
{
    return (value * 1000.0);
}

double indefinite()
{
    return std::numeric_limits<double>::infinity();
}

bool isindefinite(double value)
{
    return std::isinf(value);
}

static inline double now()
{
    return 0;
}

Animation::Animation(double duration, double delay, double iteration, Direction direction, FillMode fill, std::shared_ptr<TimingFunction> timing)
    : m_duration(duration),
      m_delay(delay),
      m_iterationCount(iteration),
      m_iterationStart(0),
      m_playbackRate(1),
      m_playbackDirection(direction),
      m_fillMode(fill),
      m_timingFunction(timing),
      m_startTime(now()),
      m_lastTime(now()),
      m_pauseTime(0),
      m_playing(true)
{
}

void Animation::setCurrentTime(double time)
{
    m_startTime = m_lastTime = now();
    m_pauseTime = time;
}

double Animation::currentTime() const
{
    if(m_playing)
        m_lastTime = now();

    return ((m_lastTime - m_startTime) * m_playbackRate) + m_pauseTime;
}

void Animation::setPlaybackRate(double rate)
{
    m_pauseTime = currentTime();
    m_startTime = m_lastTime = now();
    m_playbackRate = rate;
}

void Animation::pause()
{
    if(!m_playing)
        return;

    m_pauseTime = currentTime();
    m_startTime = m_lastTime = now();
    m_playing = false;
}

void Animation::play()
{
    if(m_playing)
        return;

    m_startTime = m_lastTime = now();
    m_playing = true;
}

void Animation::restart()
{
    m_startTime = m_lastTime = now();
    m_pauseTime = 0;
    m_playing = true;
}

bool Animation::running() const
{
    return (m_playbackRate < 0 && currentTime() > 0) || (m_playbackRate > 0 && currentTime() < totalDuration());
}

double Animation::activeDuration() const
{
    return (m_duration * m_iterationCount);
}

double Animation::totalDuration() const
{
    return activeDuration() + m_delay;
}

} // namespace juno
