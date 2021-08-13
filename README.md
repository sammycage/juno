# Juno - Basic animation library in c++

Juno is a basic animation library in c++ based on [Webs Animations](https://drafts.csswg.org/web-Animations)

## Basic Usage

### CSS

```css
.spinner {
  animation-duration : 5s;
  animation-delay : 2s;
  animation-iteration-count : 2;
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
    double duration = 5000;
    double delay = 2000;
    double iteration_count = 2;
    Direction direction = Direction::Reverse;
    FillMode fill_mode = FillMode::Backwards;

    Transform from = Transform::rotated(0);
    Transform to = Transform::rotated(360);

    Animation spinner(duration, delay, iteration_count, direction, fill_mode);
    AnimateTransform transform(from, to);

    while(spinner.running())
    {
        Transform value = transform.valueAt(spinner.progress());
        // do something useful with the value
    }

    return 0;
}
```
