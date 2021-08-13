#include <vector>
#include <tuple>

namespace juno {

enum class Direction {
    Normal,
    Reverse,
    Alternate,
    AlternateReverse
};

enum class FillMode {
    None,
    Forwards,
    Backwards,
    Both
};

class Bezier
{
public:
    Bezier() = default;
    Bezier(double x1, double y1, double x2, double y2);

    double solve(double progress) const;

    static const Bezier Linear;
    static const Bezier Ease;
    static const Bezier EaseIn;
    static const Bezier EaseOut;
    static const Bezier EaseInOut;

public:
    double x1{0};
    double y1{0};
    double x2{1};
    double y2{1};
};

class Animation
{
public:
    Animation(double duration, double delay = 0, double iteration = 1, Direction direction = Direction::Normal, FillMode fill = FillMode::None, const Bezier& easing = Bezier::Linear);

    double progressAt(double time) const;
    double progress() const { return progressAt(currentTime()); }

    double repeatCountAt(double time) const;
    double repeatCount() const { return repeatCountAt(currentTime()); }

    void setCurrentTime(double offset);
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

    void setEasingCurve(Bezier easing) { m_easingCurve = easing; }
    const Bezier& easingCurve() { return m_easingCurve; }

private:
    double m_duration;
    double m_delay;
    double m_iterationCount;
    double m_iterationStart;
    double m_playbackRate;
    Direction m_playbackDirection;
    FillMode m_fillMode;
    Bezier m_easingCurve;

    double m_startTime;
    double m_lastTime;
    double m_offset;
    bool m_playing;
};

template<typename T>
T blend(const T& from, const T& to, double progress);

template<typename T>
class Animate
{
public:
    using KeyValue = T;
    using KeyFrame = std::tuple<double, KeyValue, Bezier>;
    using KeyFrames = std::vector<KeyFrame>;

public:
    Animate(const KeyValue& from = KeyValue{}, const KeyValue& to = KeyValue{}, const Bezier& easing = Bezier::Linear)
    {
        m_frames.emplace_back(0.0, from, easing);
        m_frames.emplace_back(1.0, to, Bezier::Linear);
    }

    void setFromValue(const KeyValue& value)
    {
        std::get<1>(m_frames.front()) = value;
    }

    void setToValue(const KeyValue& value)
    {
        std::get<1>(m_frames.back()) = value;
    }

    void setEasingCurve(const Bezier& easing)
    {
        std::get<2>(m_frames.front()) = easing;
    }

    Animate<T>& addKeyFrameAt(double step, const KeyValue& value, const Bezier& easing = Bezier::Linear)
    {
        if(step > 1.0) step = 1.0;
        if(step < 0.0) step = 0.0;

        unsigned int i = 0;
        while(i < m_frames.size() && std::get<0>(m_frames[i]) < step)
            ++i;

        if(i < m_frames.size() && std::get<0>(m_frames[i]) == step)
            m_frames[i] = std::make_tuple(step, value, easing);
        else
            m_frames.emplace(m_frames.begin() + i, step, value, easing);

        return *this;
    }

    Animate<T>& addKeyFrame(const KeyFrame& frame)
    {
        auto& step = std::get<0>(frame);
        auto& value = std::get<1>(frame);
        auto& easing = std::get<2>(frame);

        return addKeyFrameAt(step, value, easing);
    }

    void addKeyFrames(const KeyFrames& frames)
    {
        for(auto& frame : frames)
            addKeyFrame(frame);
    }

    KeyValue valueAt(double progress) const
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
        auto& easing = std::get<2>(from);

        return blend<T>(fromValue, toValue, easing.solve(effectivePercent));
    }

    Animate<T>& reset(const KeyValue& from = KeyValue{}, const KeyValue& to = KeyValue{}, const Bezier& easing = Bezier::Linear)
    {
        *this = Animate<T>(from, to, easing);
        return *this;
    }

    const KeyValue& fromValue() const { return std::get<2>(m_frames.front()); }
    const KeyValue& toValue() const { return std::get<2>(m_frames.back()); }
    const Bezier& easing() const { return std::get<2>(m_frames.front()); }
    const KeyFrames& keyFrames() const { return m_frames; }

private:
    KeyFrames m_frames;
};

} // namespace juno
