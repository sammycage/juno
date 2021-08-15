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

TimingFunction::TimingFunction(Type type)
    : m_type(type)
{
}

std::shared_ptr<LinearTimingFunction> LinearTimingFunction::create()
{
    static std::shared_ptr<LinearTimingFunction> timing(new LinearTimingFunction);
    return timing;
}

LinearTimingFunction::LinearTimingFunction()
    : TimingFunction(TimingFunction::Type::Linear)
{
}

double LinearTimingFunction::solve(double x) const
{
    return x;
}

std::shared_ptr<CubicBezierTimingFunction> CubicBezierTimingFunction::create(double x1, double y1, double x2, double y2)
{
    std::shared_ptr<CubicBezierTimingFunction> timing(new CubicBezierTimingFunction(x1, y1, x2, y2));
    return timing;
}

std::shared_ptr<CubicBezierTimingFunction> CubicBezierTimingFunction::ease()
{
    static std::shared_ptr<CubicBezierTimingFunction> timing(new CubicBezierTimingFunction(0.25, 0.1, 0.25, 1.0));
    return timing;
}

std::shared_ptr<CubicBezierTimingFunction> CubicBezierTimingFunction::easeIn()
{
    static std::shared_ptr<CubicBezierTimingFunction> timing(new CubicBezierTimingFunction(0.42, 0.0, 1.0, 1.0));
    return timing;
}

std::shared_ptr<CubicBezierTimingFunction> CubicBezierTimingFunction::easeOut()
{
    static std::shared_ptr<CubicBezierTimingFunction> timing(new CubicBezierTimingFunction(0.0, 0.0, 0.58, 1.0));
    return timing;
}

std::shared_ptr<CubicBezierTimingFunction> CubicBezierTimingFunction::easeInOut()
{
    static std::shared_ptr<CubicBezierTimingFunction> timing(new CubicBezierTimingFunction(0.42, 0.0, 0.58, 1.0));
    return timing;
}

CubicBezierTimingFunction::CubicBezierTimingFunction(double x1, double y1, double x2, double y2)
    : TimingFunction(TimingFunction::Type::CubicBezier)
{
    ax = 3.0 * (x1 - x2) + 1.0;
    bx = 3.0 * (x2 - 2.0 * x1) + 1.0;
    cx = 3.0 * x1;

    ay = 3.0 * (y1 - y2) + 1.0;
    by = 3.0 * (y2 - 2.0 * y1) + 1.0;
    cy = 3.0 * y1;

    adx = 3.0 * ax;
    bdx = 2.0 * bx;
}

double CubicBezierTimingFunction::solve(double x) const
{
    auto curvex = [this](double t) { return (t*(t*(ax*t+bx+cx))); };
    auto curvey = [this](double t) { return (t*(t*(ay*t+by+cy))); };
    auto curvedx = [this](double t) { return (t*(t*(adx*t+bdx+cx))); };

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

    double t1 = 0;
    double t2 = 1;
    t = x;

    while(t1 < t2)
    {
        double dx = curvex(t) - x;
        if(std::abs(dx) < 1e-4)
            break;

        if(dx > 0)
            t2 = t;
        else
            t1 = t;

        t = t1 + 0.5 * (t2 - t1);
    }

    return curvey(t);
}

std::shared_ptr<StepsTimingFunction> StepsTimingFunction::create(int steps, Position position)
{
    std::shared_ptr<StepsTimingFunction> timing(new StepsTimingFunction(steps, position));
    return timing;
}

std::shared_ptr<StepsTimingFunction> StepsTimingFunction::start()
{
    static std::shared_ptr<StepsTimingFunction> timing(new StepsTimingFunction(1, Position::Start));
    return timing;
}

std::shared_ptr<StepsTimingFunction> StepsTimingFunction::middle()
{
    static std::shared_ptr<StepsTimingFunction> timing(new StepsTimingFunction(1, Position::Middle));
    return timing;
}

std::shared_ptr<StepsTimingFunction> StepsTimingFunction::end()
{
    static std::shared_ptr<StepsTimingFunction> timing(new StepsTimingFunction(1, Position::End));
    return timing;
}

StepsTimingFunction::StepsTimingFunction(int steps, Position position)
    : TimingFunction(TimingFunction::Type::Steps),
      m_steps(steps),
      m_position(position)
{
}

double StepsTimingFunction::solve(double x) const
{
    double offset = 0;
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
