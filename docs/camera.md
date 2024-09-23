# dgm::Camera

The Camera class is there to help you control the `sf::View` to achieve various effects. The most obvious is scrolling the view, keeping it fixed to a character in a 2D video game. The less obvious are rotations, screen shakes, and animation effects - you can use this class to move/rotate/zoom/shake over time, with a selected easing function. It can also figure out, whether the objects are visible

## How to construct camera from Window

The camera needs to be given a `sf::Viewport` to operate in and the intended resolution of that viewport.

```cpp
// TODO: how to get it from the current window
auto&& camera = dgm::Camera(viewport, resolution);
```

## Immediate effects

```cpp
auto&& camera = dgm::Camera(viewport, resolution);

// update loop
character.update(deltaTime);
camera.setPosition(character.getPosition());
camera.setRotation(character.getRotation());

if (character.isShooting())
{
  // The second parameter is shake amount
  camera.shake(sf::seconds(0.5f), 20.f);
}
```

## Regular view with minimap

TODO

```cpp
// TODO
```

## Over-time effects

TODO

```cpp
// TODO
```

## Object culling

You can only draw objects that are visible on screen. This takes position, zoom, and rotations into account.

```cpp
window.beginDraw();

for (auto&& object : game.objects)
{
  // Accepted hitboxes are either dgm::Rect or dgm::Circle
  camera.isObjectVisible(object.getHitbox());
}

window.endDraw();
```
