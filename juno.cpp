#include "juno.h"

#include <limits>
#include <cmath>
#include <chrono>

namespace juno {

double indefinite()
{
    return std::numeric_limits<double>::infinity();
}

bool isindefinite(double value)
{
    return std::isinf(value);
}

std::shared_ptr<LinearTiming> LinearTiming::create()
{
    static std::shared_ptr<LinearTiming> timing(new LinearTiming);
    return timing;
}

LinearTiming::LinearTiming()
{
}

double LinearTiming::solve(double x) const
{
    return x;
}

std::shared_ptr<CubicBezierTiming> CubicBezierTiming::create(double x1, double y1, double x2, double y2)
{
    std::shared_ptr<CubicBezierTiming> timing(new CubicBezierTiming(x1, y1, x2, y2));
    return timing;
}

std::shared_ptr<CubicBezierTiming> CubicBezierTiming::ease()
{
    static std::shared_ptr<CubicBezierTiming> timing(new CubicBezierTiming(0.25, 0.1, 0.25, 1.0));
    return timing;
}

std::shared_ptr<CubicBezierTiming> CubicBezierTiming::easeIn()
{
    static std::shared_ptr<CubicBezierTiming> timing(new CubicBezierTiming(0.42, 0.0, 1.0, 1.0));
    return timing;
}

std::shared_ptr<CubicBezierTiming> CubicBezierTiming::easeOut()
{
    static std::shared_ptr<CubicBezierTiming> timing(new CubicBezierTiming(0.0, 0.0, 0.58, 1.0));
    return timing;
}

std::shared_ptr<CubicBezierTiming> CubicBezierTiming::easeInOut()
{
    static std::shared_ptr<CubicBezierTiming> timing(new CubicBezierTiming(0.42, 0.0, 0.58, 1.0));
    return timing;
}

CubicBezierTiming::CubicBezierTiming(double x1, double y1, double x2, double y2)
{
    ax = 3.0 * (x1 - x2) + 1.0;
    bx = 3.0 * (x2 - 2.0 * x1);
    cx = 3.0 * x1;

    ay = 3.0 * (y1 - y2) + 1.0;
    by = 3.0 * (y2 - 2.0 * y1);
    cy = 3.0 * y1;

    adx = 3.0 * ax;
    bdx = 2.0 * bx;
}

double CubicBezierTiming::solve(double x) const
{
    auto curvex = [this](double t) { return t*(t*(ax*t+bx)+cx); };
    auto curvey = [this](double t) { return t*(t*(ay*t+by)+cy); };
    auto curvedx = [this](double t) { return t*(t*adx+bdx)+cx; };

    double t = x;
    for(int i = 0;i < 10;i++)
    {
        double dx = curvex(t) - x;
        if(std::abs(dx) < 1e-4)
        {
            if(t < 0) t = 0;
            else if(t > 1) t = 1;

            return curvey(t);
        }

        double dxdt = curvedx(t);
        if(dxdt < 1e-6)
            break;

        t = t - dx / dxdt;
    }

    double t0 = 0;
    double t1 = 1;
    t = x;

    while(t0 < t1)
    {
        double dx = curvex(t) - x;
        if(std::abs(dx) < 1e-4)
            break;

        if(dx > 0)
            t1 = t;
        else
            t0 = t;

        t = t0 + 0.5 * (t1 - t0);
    }

    return curvey(t);
}

std::shared_ptr<StepsTiming> StepsTiming::create(int steps, Position position)
{
    std::shared_ptr<StepsTiming> timing(new StepsTiming(steps, position));
    return timing;
}

std::shared_ptr<StepsTiming> StepsTiming::start()
{
    static std::shared_ptr<StepsTiming> timing(new StepsTiming(1, Position::Start));
    return timing;
}

std::shared_ptr<StepsTiming> StepsTiming::middle()
{
    static std::shared_ptr<StepsTiming> timing(new StepsTiming(1, Position::Middle));
    return timing;
}

std::shared_ptr<StepsTiming> StepsTiming::end()
{
    static std::shared_ptr<StepsTiming> timing(new StepsTiming(1, Position::End));
    return timing;
}

StepsTiming::StepsTiming(int steps, Position position)
    : m_steps(steps),
      m_position(position)
{
}

double StepsTiming::solve(double x) const
{
    double offset;
    switch(m_position) {
    case Position::Start:
        offset = 1.0;
        break;
    case Position::Middle:
        offset = 0.5;
        break;
    case Position::End:
        offset = 0.0;
        break;
    }

    return (std::floor(m_steps * x) + offset) / m_steps;
}

static inline double now()
{
    using duration_t = std::chrono::duration<double>;
    auto now = std::chrono::steady_clock::now();
    auto epoch = now.time_since_epoch();
    return std::chrono::duration_cast<duration_t>(epoch).count();
}

Animation::Animation(double duration, double delay, double iteration, Direction direction, FillMode fill, TimingFunction timing)
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

Animation::Phase Animation::phaseAt(double time) const
{
    double activeDuration = this->activeDuration();
    double totalDuration = this->totalDuration();

    double beforeActiveBoundaryTime = std::max(std::min(m_delay, totalDuration), 0.0);
    if(time < beforeActiveBoundaryTime || (time == beforeActiveBoundaryTime && m_playbackRate < 0.0))
        return Phase::Before;

    double activeAfterBoundaryTime = std::max(std::min(m_delay + activeDuration, totalDuration), 0.0);
    if(time > activeAfterBoundaryTime || (time == activeAfterBoundaryTime && m_playbackRate >= 0.0))
        return Phase::After;

    return Phase::Active;
}

double Animation::repeatCountAt(double time) const
{
    return 0.0;
}

double Animation::progressAt(double time) const
{
    double activeDuration = this->activeDuration();
    double activeTime;
    Phase phase = phaseAt(time);
    switch(phase) {
    case Phase::Before:
        if(m_fillMode == FillMode::Forwards || m_fillMode == FillMode::None)
            return 0.0;
        activeTime = std::max(time - m_delay, 0.0);
        break;
    case Phase::Active:
        activeTime = time - m_delay;
        break;
    case Phase::After:
        if(m_fillMode == FillMode::Backwards || m_fillMode == FillMode::None)
            return 0.0;
        activeTime = std::max(std::min(time - m_delay, activeDuration), 0.0);
        break;
    }

    double overallProgress;
    if(m_duration == 0.0)
        overallProgress = (phase == Phase::Before) ? 0.0 : m_iterationCount;
    else
        overallProgress = activeTime / m_duration;

    if(!isindefinite(overallProgress))
        overallProgress += m_iterationStart;

    double simpleIterationProgress;
    if(isindefinite(overallProgress))
        simpleIterationProgress = std::fmod(m_iterationStart, 1.0);
    else
        simpleIterationProgress = std::fmod(overallProgress, 1.0);

    if(simpleIterationProgress == 0.0 && (phase == Phase::Active || phase == Phase::After) && (activeTime == activeDuration) && m_iterationCount != 0.0)
        simpleIterationProgress = 1.0;

    double currentIteration;
    if(phase == Phase::After && isindefinite(m_iterationCount))
        currentIteration = indefinite();
    else if(simpleIterationProgress == 1.0)
        currentIteration = std::max(0.0, std::floor(overallProgress) - 1);
    else
        currentIteration = std::floor(overallProgress);

    bool isCurrentIterationEven = currentIteration == 0.0 || isindefinite(currentIteration) || std::fmod(currentIteration, 2.0) == 0.0;
    bool isCurrentIterationForwards;
    switch(m_playbackDirection) {
    case Direction::Normal:
        isCurrentIterationForwards = true;
        break;
    case Direction::Reverse:
        isCurrentIterationForwards = false;
        break;
    case Direction::Alternate:
        isCurrentIterationForwards = isCurrentIterationEven;
        break;
    case Direction::AlternateReverse:
        isCurrentIterationForwards = !isCurrentIterationEven;
        break;
    }

    double directedProgress = isCurrentIterationForwards ? simpleIterationProgress : 1.0 - simpleIterationProgress;
    if(m_timingFunction == nullptr)
        return directedProgress;

    return m_timingFunction->solve(directedProgress);
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
    return (m_playbackRate < 0 && currentTime() >= 0) || (m_playbackRate > 0 && currentTime() <= totalDuration());
}

double Animation::activeDuration() const
{
    return (m_duration * m_iterationCount);
}

double Animation::totalDuration() const
{
    return std::max(m_delay + activeDuration(), 0.0);;
}

template<> double blend<double>(const double& from, const double& to, double progress)
{
    return (to - from) * progress + from;
}

template<> int blend<int>(const int& from, const int& to, double progress)
{
    double value = blend<double>(from, to, progress);
    return static_cast<int>(std::round(value));
}

} // namespace juno
