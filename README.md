# Juno - Basic animation library in c++

Juno is a basic animation library in c++ based on [Webs Animations](https://drafts.csswg.org/web-Animations)

## Basic Usage

### CSS

```css
.spinner {
  animation-duration : 5s;
  animation-delay : 2s;
  animation-iteration-count : indefinite;
  animation-direction : reverse;
  animation-fill-mode : backwards
}

@keyframes spinner {
  from {
    transform: rotate(0);
  }
  to {
    transform: rotate(360);
  }
}
```

## C++

```cpp

#include <juno.h>

using namespace juno;

int main()
{
    double duration = seconds(5);
    double delay = seconds(2);
    double iterationCount = indefinite();
    Direction direction = Direction::Reverse;
    FillMode fillMode = FillMode::Backwards;

    Transform from = rotate(0);
    Transform to = rotate(360);

    Animation spinner(duration, delay, iterationCount, direction, fillMode);
    AnimateTransform transform(from, to);

    while(spinner.running())
    {
        Transform value = transform.valueAt(spinner.progress());
        // do something useful with the value
    }

    return 0;
}
```

## Features

- AnimateNumber(TODO)
- AnimateColor(TODO)
- AnimateTransform(TODO)
- AnimatePoint(TODO)
- AnimateRect(TODO)
- AnimatePath(TODO)
- AnimateMotion(TODO)
