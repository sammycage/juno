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

class Animation
{
public:
    Animation(double duration, double delay = 0, double iteration = 1, Direction direction = Direction::Normal, FillMode fill = FillMode::None);

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

private:
    double m_duration;
    double m_delay;
    double m_iterationCount;
    double m_iterationStart;
    double m_playbackRate;
    Direction m_playbackDirection;
    FillMode m_fillMode;

    double m_startTime;
    double m_lastTime;
    double m_offset;
    bool m_playing;
};
