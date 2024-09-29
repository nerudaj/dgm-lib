#include "Level.hpp"

void Level::loadFromLvd(const LevelD& lvd)
{
	mesh = dgm::Mesh(
		lvd.mesh.layers[0].blocks,
		sf::Vector2u(lvd.mesh.layerWidth, lvd.mesh.layerHeight),
		sf::Vector2u(lvd.mesh.tileWidth, lvd.mesh.tileHeight));

	tilemap.build(
		sf::Vector2u(lvd.mesh.tileWidth, lvd.mesh.tileHeight),
		std::vector<int>(lvd.mesh.layers[0].tiles.begin(), lvd.mesh.layers[0].tiles.end()),
		sf::Vector2u(lvd.mesh.layerWidth, lvd.mesh.layerHeight));
}

void Level::changeTileToVoid(unsigned x, unsigned y)
{
	tilemap.changeTile(x, y, 0);
	mesh[y * mesh.getDataSize().x + x] = 0;
}

Level::Level(const sf::Texture& texture)
{
	constexpr unsigned TILE_SIZE = 32;
	auto clip = dgm::Clip({ TILE_SIZE, TILE_SIZE },
		{ 0, 0, int(texture.getSize().x), int(texture.getSize().y) });

	tilemap = dgm::TileMap(texture, std::move(clip));
}
