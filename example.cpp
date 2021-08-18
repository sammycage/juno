#include <iostream>
#include <thread>

#include "juno.h"

using namespace juno;

int main()
{
    double duration = 3;
    double delay = 0;
    double iterationCount = 3;
    Direction direction = Direction::Alternate;
    FillMode fillMode = FillMode::Freeze;
    TimingFunction timingFunction = CubicBezierTiming::ease();
    Animation animation(duration, delay, iterationCount, direction, fillMode, timingFunction);

    AnimateNumber x;
    x.addKeyFrameAt(0, 60, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 0%
    x.addKeyFrameAt(0.25, 110, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 25%
    x.addKeyFrameAt(0.5, 60, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 50%
    x.addKeyFrameAt(0.75, 10, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 75%
    x.addKeyFrameAt(1, 60); // 100%

    AnimateNumber y;
    y.addKeyFrameAt(0, 10, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 0%
    y.addKeyFrameAt(0.25, 60, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 25%
    y.addKeyFrameAt(0.5, 110, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 50%
    y.addKeyFrameAt(0.75, 60, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 75%
    y.addKeyFrameAt(1, 10); // 100%

    while(animation.running())
    {
        auto currentTime = animation.currentTime();
        auto progress = animation.progressAt(currentTime);
        std::cout << "At " << currentTime << "s x is " << x.valueAt(progress) << " and y is " << y.valueAt(progress) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    return 0;
}
