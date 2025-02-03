# `dgm::TextureAtlas`

Texture atlas is a rendering-optimization technique that can help immensely with speeding up your games. It works by creating one huge texture and copying all other smaller textures into it. Thus, all entities like `sf::Sprite` can share the same texture and just use a different subset from it.

SFML works in an odd way, so just using `sf::Sprite` with a texture atlas wouldn't help at all. As the second step, every instance of a `sf::Sprite` must be replaced with a `sf::VertexArray` and each entity needs to be inserted into it manually as a sequence of four vertices. See [related SFML tutorial](https://www.sfml-dev.org/tutorials/3.0/graphics/vertex-array) for more info.

As a result, instead of having N tiny drawcalls for rendering N entities, you can have just one big drawcall, rendering everything, which is much more convenient for the graphics card.

## Spritesheets vs Tilesets

DGM has a concept of animations and tilemaps. The building block of everything is a `dgm::Clip` which maps a sequence of "frames" - a bunch of rectangles with coordinates mapped to a particular texture.

Then there is the `dgm::TileMap` class that can render grid-based maps efficiently (based on [SFML tutorial](https://www.sfml-dev.org/tutorials/3.0/graphics/vertex-array/#example-tile-map)). This class requires a texture with a tileset and a clip, mapping each tile in the tileset.

There is also a `dgm::Animation` which represents an animation system over an entity and it uses `dgm::AnimationStates` which is just a map mapping string names to `dgm::Clip`. It effectively represents the relationship "animation 'idle' is comprised of clip of N frames pointing to a particular texture".

## How to use the atlas

When constructing the atlas, you must specify the size of the atlas texture, this number will be dependent on the scale of your project. You can then register tilesets - pairs of texture and clips, or spritesheets - pairs of texture and animation states.

The atlas then returns a location token - you can later retrieve clip/animation states that has been adjusted based on where the source texture was copied into the atlas texture.

Simple usage:

```cpp
auto&& atlas = dgm::TextureAtlas(512, 512);
auto&& loc = atlas.addTileset(texture, clip);
sprite.setTexture(atlas.getTexture);
sprite.setTextureRect(atlas.getClip(loc.value()).getFrame(0));
```

Full example showing texture atlas can be found [here](../examples/example-07-texture-atlas).