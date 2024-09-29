#pragma once

#include <DGM/dgm.hpp>
#include <include/LevelD.hpp>

class Level
{
protected:
	dgm::Mesh mesh;
	dgm::TileMap tilemap;

public:
	void draw(dgm::Window& window)
	{
		window.draw(tilemap);
	}

	const dgm::Mesh& getMesh() const
	{
		return mesh;
	}

	void changeTileToVoid(unsigned x, unsigned y);

	void loadFromLvd(const LevelD& lvd);

	Level(const sf::Texture& texture);
};
