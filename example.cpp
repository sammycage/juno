#include <iostream>
#include <thread>

#include "juno.h"

using namespace juno;

int main()
{
    double duration = 5;
    double delay = 2;
    double iterationCount = 2;
    Direction direction = Direction::Reverse;
    FillMode fillMode = FillMode::None;

    Animation animation(duration, delay, iterationCount, direction, fillMode);
    while(animation.running())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(125));
        std::cout << animation.progress() << std::endl;
    }

    return 0;
}
