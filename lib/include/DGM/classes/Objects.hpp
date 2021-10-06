/**
 *  \file 			Objects.hpp
 *  \author			doomista
 *  \description	Declarations of basic dmg logic objects
 */

#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <LevelD.hpp>

#include <vector>

namespace dgm {
	class Window;

	/**
	 *  \brief Abstract class with interface
	 *  common to all objects
	 */
	class Object {
	public:
		virtual const sf::Vector2f &getPosition() const =0;
		
		virtual void setPosition(const float x, const float y) =0;
		
		virtual void setPosition(const sf::Vector2f &position) =0;
		
		virtual void move(const float x, const float y) =0;
		
		virtual void move(const sf::Vector2f &position) =0;

		virtual ~Object() {}
	};
	
	/**
	 *  \brief Logic class for circles
	 *  
	 *  \details This class is basis for most of
	 *  your characters and collidable objects in
	 *  top-down games. Circle is defined by position
	 *  of its center and radius.
	 */
	class Circle : public Object {
	protected:
		sf::Vector2f position; ///< Position of the center
		float radius; ///< Radius of circle
		
	public:
		/**
		 *  \brief Displays object in the window
		 *
		 *  This method is slow and is reserved for debug purposes
		 */
		void debugRender(dgm::Window &window, sf::Color color = sf::Color::Yellow) const;

		/**
		 *  \brief Returns position of circle
		 */
		[[nodiscard]] const sf::Vector2f& getPosition() const noexcept {
			return position;
		}
		
		/**
		 *  \brief Returns radius of circle
		 */
		[[nodiscard]] constexpr const float getRadius() const noexcept {
			return radius;
		}
		
		/**
		 *  \brief Sets position of circle by setting new XY coordinates
		 */
		void setPosition(const float x, const float y) {
			position.x = x;
			position.y = y;
		}
		
		/**
		 *  \brief Sets new position of circle with SFML vector
		 */
		void setPosition(const sf::Vector2f& newPosition) noexcept {
			position = newPosition;
		}
		
		/**
		 *  \brief Sets radius
		 */
		void setRadius(const float newRadius) noexcept {
			radius = newRadius;
		}
		
		/**
		 *  \brief Moves object
		 */
		void move(const float x, const float y) noexcept {
			position.x += x;
			position.y += y;
		}
		
		/**
		 *  \brief Moves object
		 */
		void move(const sf::Vector2f& forward) noexcept {
			position += forward;
		}
		
		Circle();
		Circle(const float x, const float y);
		Circle(const sf::Vector2f &position);
		virtual ~Circle() {}
	};
	
	/**
	 *  \brief Logic class for rectangle
	 *  
	 *  \details Rectangle is defined by position
	 *  of its top left corner and its dimesions.
	 */
	class Rect : public Object {
	protected:
		sf::Vector2f position = { 0.f, 0.f }; ///< Position of the topleft corner
		sf::Vector2f size = { 0.f, 0.f }; ///< Dimensions of the rectangle

	public:
		/**
		 *  \brief Displays object in the window
		 *
		 *  This method is slow and is reserved for debug purposes
		 */
		void debugRender(dgm::Window& window, sf::Color color = sf::Color::Yellow) const;

		/**
		 *  \brief Returns position of topleft vertex of rectangle
		 */
		[[nodiscard]] const sf::Vector2f& getPosition() const noexcept {
			return position;
		}

		/**
		 *  \brief Returns dimensions of rectangle
		 */
		[[nodiscard]] const sf::Vector2f& getSize() const noexcept {
			return size;
		}

		[[nodiscard]] const sf::Vector2f& getCenter() const noexcept {
			return getPosition() + getSize() / 2.f;
		}

		/**
		 *  \brief Set position of top-left corner
		 */
		void setPosition(const float x, const float y);

		/**
		 *  \brief Set position of top-left corner
		 */
		void setPosition(const sf::Vector2f& newPosition) noexcept {
			position = newPosition;
		}

		/**
		 *  \brief Moves object
		 */
		void move(const float x, const float y) noexcept {
			position.x += x;
			position.y += y;
		}

		/**
		 *  \brief Moves object
		 */
		void move(const sf::Vector2f& forward) noexcept {
			position += forward;
		}

		/**
		 *  \brief Set dimensions of rectangle
		 */
		void setSize(const float width, const float height);

		/**
		 *  \brief Set dimensions of rectangle
		 */
		constexpr void setSize(const sf::Vector2f& newSize) noexcept {
			size = newSize;
		}

		Rect() {}
		Rect(const float x, const float y, const float width, const float height) : position({ x, y }), size({ width, height }) {}
		Rect(const sf::Vector2f& position, const sf::Vector2f& size) : position(position), size(size) {}
		virtual ~Rect() {}
	};
	
	/**
	 *  \brief Logic class for representing level geometry or per-pixel collision hitboxes
	 *  
	 *  Mesh is basically an integer array where in DGM logic each cell with value
	 *  <= 0 is considered to be 'air' and any cell with value > 0 is impassable.
	 *  If this object is send to DGM collision functions, it will be compared
	 *  to the other object based on set voxelSize and dataSize. If your mesh
	 *  represents a whole level, then voxelSize is size of a single block (64x64px for example).
	 *  If mesh represents a per-pixel hitbox, then voxelSize should be 1 (each cell is single pixel).
	 *  Access cell at [x, y] by y * dataSize.x + x.
	 */
	class Mesh : public Object {
	protected:
		std::vector<int> data;	///< Array for holding collision data
		sf::Vector2f position;	///< Position of top-left corner
		sf::Vector2u dataSize; ///< dataSize.x * dataSize.y is size of data array
		sf::Vector2u voxelSize; ///< How big rectangle does single cell of data represents
		
	public:
		/**
		 *  \brief Read and Write access to *data
		 *  
		 *  Access cell at [x, y] by y * dataSize.x + x.
		 */
		int &operator[] (std::size_t index) {
			return data[index];
		}
		
		/**
		 *  \brief Read-only access to *data
		 *  
		 *  Access cell at [x, y] by y * dataSize.x + x.
		 */
		const int &operator[] (std::size_t index) const {
			return data[index];
		}

		int& at(unsigned x, unsigned y) {
			return (*this)[y * dataSize.x + x];
		}

		const int &at(unsigned x, unsigned y) const {
			return (*this)[y * dataSize.x + x];
		}

		int &at(const sf::Vector2u &pos) {
			return at(pos.x, pos.y);
		}

		const int &at(const sf::Vector2u &pos) const {
			return at(pos.x, pos.y);
		}
	
		/**
		 *  \brief get position of top-left corner
		 */
		const sf::Vector2f &getPosition() const {
			return position;
		}
		
		/**
		 *  \brief get dimensions of *data array
		 */
		const sf::Vector2u &getDataSize() const {
			return dataSize;
		}
		
		/**
		 *  \brief get dimensions of single voxel
		 */
		const sf::Vector2u &getVoxelSize() const {
			return voxelSize;
		}
	
		/**
		 *  \brief Set position of top-left corner
		 */
		void setPosition(const float x, const float y) noexcept {
			position.x = x;
			position.y = y;
		}
		
		/**
		 *  \brief Set position of top-left corner
		 */
		void setPosition(const sf::Vector2f &newPosition) noexcept {
			position = newPosition;
		}
		
		/**
		 *  \brief Set dimensions of single voxel
		 */
		void setVoxelSize(const unsigned width, const unsigned height) noexcept {
			voxelSize.x = width;
			voxelSize.y = height;
		}
		
		/**
		 *  \brief Set dimensions of single voxel
		 */
		void setVoxelSize(const sf::Vector2u &size) { 
			voxelSize = size; 
		}
		
		/**
		 *  \brief Set dimensions of data array
		 *  
		 *  \details This will allocate memory for data.
		 *  Any data stored here prior to this call will be lost.
		 */
		void setDataSize(const unsigned width, const unsigned height) {
			setDataSize(sf::Vector2u(width, height)); 
		}
		
		/**
		 *  \brief Set dimensions of data array
		 *  
		 *  \details This will allocate memory for data.
		 *  Any data stored here prior to this call will be lost.
		 *  
		 */
		void setDataSize(const sf::Vector2u &size);
		
		/**
		 *  \brief Moves object
		 */
		void move(const float x, const float y);
		
		/**
		 *  \brief Moves object
		 */
		void move(const sf::Vector2f &forward);
		
		Mesh();

		/**
		 *  \brief Construct mesh object from LevelD
         *
		 *  This method uses LevelD::Mesh::tileWidth and tileHeight as voxelSize,
		 *  LevelD::Mesh::layerWidth and layerHeight as dataSize and
		 *  LevelD::TileLayer::blocks from selected layer as data (collision information)
		 */
		Mesh(const LevelD::Mesh& mesh, unsigned layerIndex = 0);

		virtual ~Mesh() {}
	};
}