# Juno - Basic animation library in c++

Juno is a basic animation library in c++ based on [SMIL](https://www.w3.org/TR/REC-smil/smil-animation.html) and [Web Animations](https://drafts.csswg.org/web-animations)

## Basic Usage

```cpp

#include <juno.h>

using namespace juno;

int main()
{
    double duration = 4;
    double delay = 0;
    double iterationCount = 3;
    Direction direction = Direction::Alternate;
    FillMode fillMode = FillMode::Remove;
    TimingFunction timingFunction = LinearTiming::create();
    Animation animation(duration, delay, iterationCount, direction, fillMode, timingFunction);

    AnimateNumber cx;
    cx.addKeyFrameAt(0, 60, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 0%
    cx.addKeyFrameAt(0.25, 110, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 25%
    cx.addKeyFrameAt(0.5, 60, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 50%
    cx.addKeyFrameAt(0.75, 10, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 75%
    cx.addKeyFrameAt(1, 60); // 100%

    AnimateNumber cy;
    cy.addKeyFrameAt(0, 10, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 0%
    cy.addKeyFrameAt(0.25, 60, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 25%
    cy.addKeyFrameAt(0.5, 110, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 50%
    cy.addKeyFrameAt(0.75, 60, CubicBezierTiming::create(0.5, 0, 0.5, 1)); // 75%
    cy.addKeyFrameAt(1, 10); // 100%

    return 0;
}
```

Output :

<p align="center">
  <img src="https://github.com/sammycage/juno/blob/main/hello.gif">
</p>

## Features

- AnimateInteger
- AnimateNumber
- AnimateColor(TODO)
- AnimateTransform(TODO)
- AnimatePoint(TODO)
- AnimateRect(TODO)
- AnimatePath(TODO)
- AnimateMotion(TODO)
