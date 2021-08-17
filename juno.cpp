#ifndef JUNO_H
#define JUNO_H

#include <vector>
#include <tuple>
#include <memory>

namespace juno {

enum class Direction
{
    Normal,
    Reverse,
    Alternate,
    AlternateReverse
};

enum class FillMode
{
    None,
    Forwards,
    Backwards,
    Both
};

class Timing
{
public:
    enum class Type
    {
        Linear,
        CubicBezier,
        Steps
    };

    virtual ~Timing() = default;
    virtual double solve(double x) const = 0;

    Type type() const { return m_type; }

protected:
    Timing(Type type);

private:
    Type m_type;
};

class LinearTiming : public Timing
{
public:
    static std::shared_ptr<LinearTiming> create();

    double solve(double x) const;

private:
    LinearTiming();
};

class CubicBezierTiming : public Timing
{
public:
    static std::shared_ptr<CubicBezierTiming> create(double x1, double y1, double x2, double y2);

    static std::shared_ptr<CubicBezierTiming> ease();
    static std::shared_ptr<CubicBezierTiming> easeIn();
    static std::shared_ptr<CubicBezierTiming> easeOut();
    static std::shared_ptr<CubicBezierTiming> easeInOut();

    double solve(double x) const;

private:
    CubicBezierTiming(double x1, double y1, double x2, double y2);

private:
    double ax;
    double bx;
    double cx;
    double ay;
    double by;
    double cy;
    double adx;
    double bdx;
};

class StepsTiming : public Timing
{
public:
    enum class Position
    {
        Start,
        Middle,
        End
    };

    static std::shared_ptr<StepsTiming> create(int steps, Position position);

    static std::shared_ptr<StepsTiming> start();
    static std::shared_ptr<StepsTiming> middle();
    static std::shared_ptr<StepsTiming> end();

    double solve(double x) const;

private:
    StepsTiming(int steps, Position position);

private:
    int m_steps;
    Position m_position;
};

using TimingFunction = std::shared_ptr<Timing>;

double indefinite();
bool isindefinite(double value);

class Animation
{
public:
    enum class Phase
    {
        Before,
        Active,
        After
    };

    Animation(double duration, double delay = 0, double iteration = 1, Direction direction = Direction::Normal, FillMode fill = FillMode::None, TimingFunction timing = nullptr);

    Phase phaseAt(double time) const;
    Phase phase() const { return phaseAt(currentTime()); }

    double progressAt(double time) const;
    double progress() const { return progressAt(currentTime()); }

    double repeatCountAt(double time) const;
    double repeatCount() const { return repeatCountAt(currentTime()); }

    void setCurrentTime(double time);
    double currentTime() const;

    void setPlaybackRate(double rate);
    double playbackRate() const { return m_playbackRate; }

    void pause();
    void play();
    void restart();
    void reverse() { setPlaybackRate(-m_playbackRate); }

    bool running() const;
    bool playing() const { return m_playing; }

    double activeDuration() const;
    double totalDuration() const;

    void setDuration(double duration) { m_duration = duration; }
    double duration() const { return m_duration; }

    void setDelay(double delay) { m_delay = delay; }
    double delay() const { return m_delay; }

    void setIterationCount(double iteration) { m_iterationCount = iteration; }
    double iterationCount() const { return m_iterationCount; }

    void setIterationStart(double start) { m_iterationStart = start; }
    double iterationStart() const { return m_iterationStart; }

    void setPlaybackDirection(Direction direction) { m_playbackDirection = direction; }
    Direction playbackDirection() { return m_playbackDirection; }

    void setFillMode(FillMode fill) { m_fillMode = fill; }
    FillMode fillMode() { return m_fillMode; }

    void setTimingFunction(TimingFunction timing) { m_timingFunction = timing; }
    TimingFunction timingFunction() { return m_timingFunction; }

private:
    double m_duration;
    double m_delay;
    double m_iterationCount;
    double m_iterationStart;
    double m_playbackRate;
    Direction m_playbackDirection;
    FillMode m_fillMode;
    TimingFunction m_timingFunction;

    double m_startTime;
    mutable double m_lastTime;
    double m_pauseTime;
    bool m_playing;
};

template<typename T>
T blend(const T& from, const T& to, double progress);

template<typename T>
class Animate
{
public:
    using ValueType = T;
    using KeyFrame = std::tuple<double, ValueType, TimingFunction>;
    using KeyFrames = std::vector<KeyFrame>;

public:
    Animate(const ValueType& from = ValueType{}, const ValueType& to = ValueType{}, TimingFunction timing = nullptr)
    {
        m_frames.emplace_back(0.0, from, timing);
        m_frames.emplace_back(1.0, to, nullptr);
    }

    Animate<T>& addKeyFrameAt(double step, const ValueType& value, TimingFunction timing = nullptr)
    {
        if(step > 1.0) step = 1.0;
        if(step < 0.0) step = 0.0;

        unsigned int i = 0;
        while(i < m_frames.size() && std::get<0>(m_frames[i]) < step)
            ++i;

        if(i < m_frames.size() && std::get<0>(m_frames[i]) == step)
            m_frames[i] = std::make_tuple(step, value, timing);
        else
            m_frames.emplace(m_frames.begin() + i, step, value, timing);

        return *this;
    }

    Animate<T>& addKeyFrame(const KeyFrame& frame)
    {
        auto& step = std::get<0>(frame);
        auto& value = std::get<1>(frame);
        auto& timing = std::get<2>(frame);

        return addKeyFrameAt(step, value, timing);
    }

    void addKeyFrames(const KeyFrames& frames)
    {
        for(auto& frame : frames)
            addKeyFrame(frame);
    }

    ValueType valueAt(double progress) const
    {
        if(progress > 1.0) progress = 1.0;
        if(progress < 0.0) progress = 0.0;

        unsigned int index = 0;
        while(index < m_frames.size() - 2)
        {
            if(std::get<0>(m_frames[index + 1]) > progress)
                break;
            ++index;
        }

        auto& from = m_frames[index];
        auto& to = m_frames[index + 1];

        auto fromPercent = std::get<0>(from);
        auto toPercent = std::get<0>(to); 
        auto effectivePercent = (progress - fromPercent) / (toPercent - fromPercent);

        auto& fromValue = std::get<1>(from);
        auto& toValue = std::get<1>(to);
        auto& timing = std::get<2>(from);
        if(timing)
            effectivePercent = timing->solve(effectivePercent);
        return blend<T>(fromValue, toValue, effectivePercent);
    }

    Animate<T>& reset(const ValueType& from = ValueType{}, const ValueType& to = ValueType{}, TimingFunction timing = nullptr)
    {
        *this = Animate<T>(from, to, timing);
        return *this;
    }

    void setFromValue(const ValueType& value) { std::get<1>(m_frames.front()) = value; }
    const ValueType& fromValue() const { return std::get<2>(m_frames.front()); }

    void setToValue(const ValueType& value) { std::get<1>(m_frames.back()) = value; }
    const ValueType& toValue() const { return std::get<2>(m_frames.back()); }

    void setTimingFunction(TimingFunction timing) { std::get<2>(m_frames.front()) = timing; }
    TimingFunction timingFunction() const { return std::get<2>(m_frames.front()); }

    const KeyFrames& keyFrames() const { return m_frames; }
private:
    KeyFrames m_frames;
};

class Color;
class Point;
class Rect;
class Path;
class Transform;

using AnimateInteger = Animate<int>;
using AnimateNumber = Animate<double>;
using AnimateColor = Animate<Color>;
using AnimatePoint = Animate<Point>;
using AnimateRect = Animate<Rect>;
using AnimatePath = Animate<Path>;
using AnimateTransform = Animate<Transform>;

} // namespace juno

#endif // JUNO_H
