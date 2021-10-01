#pragma once

#include <DGM\dgm.hpp>
#include <cassert>

namespace dgm {
	/**
	 *  \brief Class for rendering tile based maps which has only one texture (so-called tileset)
	 *
	 *  In order for object to work, you need a sf::Texture (tileset), dgm::Clip (which contains
	 *  coordinates for each tile in the texture) and an array of data (your map, where each cell
	 *  is an index of a tile).
	 */
	class TileMap : public sf::Drawable, public sf::Transformable {
	private:
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	protected:
		sf::Texture *texturePtr = nullptr;
		sf::VertexArray vertices;
		sf::Vector2f tileSize;
		sf::Vector2u dataSize;
		dgm::Clip clip;

		void changeTile(float x, float y, uint32_t tileIndex, uint32_t tileValue);

	public:
		/**
		 *  \brief Change appearance of a tile
		 *
		 *  \param [in] tileX Tile X position
		 *  \param [in] tileY Tile Y position
		 *  \param [in] tileValue New frame index for a tile
		 *
		 *  \details Will change image displayed on a tile[x, y] to image with index
		 *  value of tileValue. tileX and tileY must lie within the tileset and tileValue
		 *  must index valid clip frame. New value is stored and will be exported by
		 *  SaveToFile() method.
         *
		 *  \pre build was called
		 */
		void changeTile(uint32_t tileX, uint32_t tileY, uint32_t tileValue) {
			assert(texturePtr);
			assert(tileX < uint32_t(dataSize.x) && tileY < uint32_t(dataSize.y));
			assert(clip.getFrameCount() > tileValue);
			changeTile(float(tileX), float(tileY), tileY * dataSize.x + tileX, tileValue);
		}

		/**
		 *  \brief Build the internal vertex array
		 *
		 *  \param [in] tileSize Dimensions of a tile (can differ from clip frame size)
		 *  \param [in] imageData Array of tile frame indices
		 *  \param [in] dataSize Size of the map [Width, Height]
		 *
		 *  \details You can call this function repeatedly without need to deinitialize this
		 *  object. If you only need to change appearance of a couple of tiles, use changeTile()
		 *  instead.
		 *
		 *  \pre You have to call init method prior to calling this one!
		 *
		 *  \see changeTile
		 */
		void build(const sf::Vector2u tileSize, const std::vector<int> &imageData, const sf::Vector2u &dataSize);

		/**
		 *  \brief Build the internal vertex array
		 *
		 *  \param [in] lvd  Initialized LevelD object
		 *
		 *  \details You can call this function repeatedly without need to deinitialize this
		 *  object. If you only need to change appearance of a couple of tiles, use changeTile()
		 *  instead.
		 *
		 *  \pre You have to call init method prior to calling this one!
		 *
		 *  \see changeTile
		 */
		void build(const LevelD::Mesh &mesh, unsigned layerIndex = 0) {
			if (mesh.layers.size() <= layerIndex) {
				throw dgm::EnvironmentException("Requesting layer " + std::to_string(layerIndex) +
					" in mesh which has only " + std::to_string(mesh.layers.size()) + " layers");
			}

			build(
				{ mesh.tileWidth, mesh.tileHeight },
				std::vector<int>(mesh.layers[layerIndex].tiles.begin(), mesh.layers[layerIndex].tiles.end()),
				{ mesh.layerWidth, mesh.layerHeight }
			);
		}

		void init(sf::Texture &texture, const dgm::Clip &clip) {
			texturePtr = &texture;
			TileMap::clip = clip;
		}

		const dgm::Clip& getClip() const {
			return clip;
		}

		TileMap();
		~TileMap();
	};
}