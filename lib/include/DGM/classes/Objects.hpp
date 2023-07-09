/**
 *  \file 			Objects.hpp
 *  \author			doomista
 *  \description	Declarations of basic dmg logic objects
 */

#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace dgm
{
    class Window;

    /**
     *  \brief Abstract class with interface
     *  common to all objects
     */
    class Object
    {
    public:
        virtual const sf::Vector2f& getPosition() const = 0;

        virtual void setPosition(const float x, const float y) = 0;

        virtual void setPosition(const sf::Vector2f& position) = 0;

        virtual void move(const float x, const float y) = 0;

        virtual void move(const sf::Vector2f& position) = 0;

        virtual ~Object() = default;
    };

    /**
     *  \brief Logic class for circles
     *
     *  \details This class is basis for most of
     *  your characters and collidable objects in
     *  top-down games. Circle is defined by position
     *  of its center and radius.
     */
    class Circle final : public Object
    {
    protected:
        sf::Vector2f position = { 0.f, 0.f }; ///< Position of the center
        float radius = 0.f;                   ///< Radius of circle

    public:
        /**
         *  \brief Displays object in the window
         *
         *  This method is slow and is reserved for debug purposes
         */
        void debugRender(
            dgm::Window& window, sf::Color color = sf::Color::Yellow) const;

        /**
         *  \brief Returns position of circle
         */
        [[nodiscard]] constexpr const sf::Vector2f& getPosition() const noexcept
        {
            return position;
        }

        /**
         *  \brief Returns radius of circle
         */
        [[nodiscard]] constexpr const float getRadius() const noexcept
        {
            return radius;
        }

        /**
         *  \brief Sets position of circle by setting new XY coordinates
         */
        void setPosition(const float x, const float y)
        {
            position.x = x;
            position.y = y;
        }

        /**
         *  \brief Sets new position of circle with SFML vector
         */
        void setPosition(const sf::Vector2f& newPosition) noexcept
        {
            position = newPosition;
        }

        /**
         *  \brief Sets radius
         */
        void setRadius(const float newRadius) noexcept
        {
            radius = newRadius;
        }

        /**
         *  \brief Moves object
         */
        constexpr void move(const float x, const float y) noexcept
        {
            position.x += x;
            position.y += y;
        }

        /**
         *  \brief Moves object
         */
        void move(const sf::Vector2f& forward) noexcept
        {
            position += forward;
        }

        [[nodiscard]] constexpr Circle() noexcept = default;

        [[nodiscard]] Circle(
            const float x, const float y, const float radius) noexcept
            : position(x, y), radius(radius)
        {
        }

        [[nodiscard]] constexpr Circle(
            const sf::Vector2f& position, const float radius) noexcept
            : position(position), radius(radius)
        {
        }
    };

    /**
     *  \brief Logic class for rectangle
     *
     *  \details Rectangle is defined by position
     *  of its top left corner and its dimesions.
     */
    class Rect final : public Object
    {
    protected:
        sf::Vector2f position = { 0.f,
                                  0.f };  ///< Position of the topleft corner
        sf::Vector2f size = { 0.f, 0.f }; ///< Dimensions of the rectangle

    public:
        /**
         *  \brief Displays object in the window
         *
         *  This method is slow and is reserved for debug purposes
         */
        void debugRender(
            dgm::Window& window, sf::Color color = sf::Color::Yellow) const;

        /**
         *  \brief Returns position of topleft vertex of rectangle
         */
        [[nodiscard]] constexpr const sf::Vector2f& getPosition() const noexcept
        {
            return position;
        }

        /**
         *  \brief Returns dimensions of rectangle
         */
        [[nodiscard]] constexpr const sf::Vector2f& getSize() const noexcept
        {
            return size;
        }

        [[nodiscard]] const sf::Vector2f& getCenter() const noexcept
        {
            return getPosition() + getSize() / 2.f;
        }

        /**
         *  \brief Set position of top-left corner
         */
        void setPosition(const float x, const float y);

        /**
         *  \brief Set position of top-left corner
         */
        constexpr void setPosition(const sf::Vector2f& newPosition) noexcept
        {
            position = newPosition;
        }

        /**
         *  \brief Moves object
         */
        constexpr void move(const float x, const float y) noexcept
        {
            position.x += x;
            position.y += y;
        }

        /**
         *  \brief Moves object
         */
        void move(const sf::Vector2f& forward) noexcept
        {
            position += forward;
        }

        /**
         *  \brief Set dimensions of rectangle
         */
        void setSize(const float width, const float height);

        /**
         *  \brief Set dimensions of rectangle
         */
        constexpr void setSize(const sf::Vector2f& newSize) noexcept
        {
            size = newSize;
        }

        [[nodiscard]] constexpr Rect() noexcept = default;

        [[nodiscard]] Rect(
            const float x,
            const float y,
            const float width,
            const float height) noexcept
            : position({ x, y }), size({ width, height })
        {
        }

        [[nodiscard]] constexpr Rect(
            const sf::Vector2f& position, const sf::Vector2f& size) noexcept
            : position(position), size(size)
        {
        }
    };

    /**
     *  \brief Representation of a simple vision cone
     *
     *  Vision cone is represented by an isosceles triangle and has following
     * properties:
     *
     *             #####  ^
     *        ##########  |
     *   ###############  width
     *        ##########  |
     *             #####  v
     *   <-- length  -->
     *
     *  Leftmost point on the image is an origin of the triange, so that is the
     * point that is considered to be the 'position' and also the point around
     * which cone rotates.
     *
     *  Currently, only collisions between circles and cones are supported
     */
    class VisionCone final : public Object
    {
        sf::Vector2f position = { 0.f, 0.f };
        sf::Vector2f forward = { 0.f, 0.f };
        float width = 0.f;
        float rotation = 0.f;

    public:
        /**
         *  \brief Displays object in the window
         *
         *  This method is slow and is reserved for debug purposes
         */
        void debugRender(
            dgm::Window& window, sf::Color color = sf::Color::Yellow) const;

        virtual constexpr const sf::Vector2f& getPosition() const override
        {
            return position;
        }

        virtual void setPosition(const float x, const float y) override
        {
            position = { x, y };
        }

        virtual constexpr void
        setPosition(const sf::Vector2f& newPosition) override
        {
            position = newPosition;
        }

        virtual void move(const float x, const float y) override;

        virtual void move(const sf::Vector2f& direction) override
        {
            position += direction;
        }

        [[nodiscard]] constexpr float getRotation() const noexcept
        {
            return rotation;
        }

        void setRotation(const float angle);
        void rotate(const float angle);

        [[nodiscard]] float getLength() const;

        [[nodiscard]] constexpr float getWidth() const noexcept
        {
            return width;
        }

        [[nodiscard]] constexpr VisionCone() noexcept = default;
        [[nodiscard]] VisionCone(const float length, const float width);
    };

    /**
     *  \brief Logic class for representing level geometry or per-pixel
     * collision hitboxes
     *
     *  Mesh is basically an integer array where in DGM logic each cell with
     * value
     *  <= 0 is considered to be 'air' and any cell with value > 0 is
     * impassable. If this object is send to DGM collision functions, it will be
     * compared to the other object based on set voxelSize and dataSize. If your
     * mesh represents a whole level, then voxelSize is size of a single block
     * (64x64px for example). If mesh represents a per-pixel hitbox, then
     * voxelSize should be 1 (each cell is single pixel). Access cell at [x, y]
     * by y * dataSize.x + x.
     */
    class Mesh : public Object
    {
    protected:
        std::vector<int> data = {}; ///< Array for holding collision data
        sf::Vector2f position = { 0.f, 0.f }; ///< Position of top-left corner
        sf::Vector2u dataSize = {
            0u, 0u
        }; ///< dataSize.x * dataSize.y is size of data array
        sf::Vector2u voxelSize = {
            0u, 0u
        }; ///< How big rectangle does single cell of data represents

    public:
        [[nodiscard]] constexpr inline auto&&
        operator[](this auto&& self, std::size_t index) noexcept
        {
            return self.data[index];
        }

        [[nodiscard]] constexpr inline auto&&
        at(this auto&& self, unsigned x, unsigned y) noexcept
        {
            return self[y * self.dataSize.x + x];
        }

        [[nodiscard]] constexpr inline auto&&
        at(this auto&& self, const sf::Vector2u& pos) noexcept
        {
            return self.at(pos.x, pos.y);
        }

        /**
         *  \brief get position of top-left corner
         */
        [[nodiscard]] constexpr inline const sf::Vector2f&
        getPosition() const noexcept
        {
            return position;
        }

        /**
         *  \brief get dimensions of *data array
         */
        [[nodiscard]] constexpr inline const sf::Vector2u&
        getDataSize() const noexcept
        {
            return dataSize;
        }

        /**
         *  \brief get dimensions of single voxel
         */
        [[nodiscard]] constexpr inline const sf::Vector2u&
        getVoxelSize() const noexcept
        {
            return voxelSize;
        }

        /**
         *  \brief Set position of top-left corner
         */
        constexpr void setPosition(const float x, const float y) noexcept
        {
            position.x = x;
            position.y = y;
        }

        /**
         *  \brief Set position of top-left corner
         */
        constexpr void setPosition(const sf::Vector2f& newPosition) noexcept
        {
            position = newPosition;
        }

        /**
         *  \brief Set dimensions of single voxel
         */
        constexpr void
        setVoxelSize(const unsigned width, const unsigned height) noexcept
        {
            voxelSize.x = width;
            voxelSize.y = height;
        }

        /**
         *  \brief Set dimensions of single voxel
         */
        constexpr void setVoxelSize(const sf::Vector2u& size)
        {
            voxelSize = size;
        }

        /**
         *  \brief Set dimensions of data array
         *
         *  \details This will allocate memory for data.
         *  Any data stored here prior to this call will be lost.
         */
        void setDataSize(const unsigned width, const unsigned height)
        {
            setDataSize(sf::Vector2u(width, height));
        }

        /**
         *  \brief Set dimensions of data array
         *
         *  \details This will allocate memory for data.
         *  Any data stored here prior to this call will be lost.
         *
         */
        void setDataSize(const sf::Vector2u& size);

        /**
         *  \brief Moves object
         */
        void move(const float x, const float y);

        /**
         *  \brief Moves object
         */
        void move(const sf::Vector2f& forward);

        [[nodiscard]] constexpr Mesh() noexcept = default;

        /**
         *  \brief Construct mesh object from data array
         *
         *  Data array is supposed to be row-major, meaning that
         *  for 2D map with items
         *
         *  1 2 3
         *  4 5 6
         *  7 8 9
         *
         *  the data will be layed out like: 1 2 3 4 5 6 7 8 9.
         *
         *  Size of data should be dataSize.x * dataSize.y.
         */
        [[nodiscard]] Mesh(
            const std::vector<int>& data,
            const sf::Vector2u& dataSize,
            const sf::Vector2u& voxelSize);

        [[nodiscard]] Mesh(
            const std::vector<bool>& data,
            const sf::Vector2u& dataSize,
            const sf::Vector2u& voxelSize)
            : Mesh(
                std::vector<int>(data.begin(), data.end()), dataSize, voxelSize)
        {
        }

        virtual ~Mesh() = default;
    };
} // namespace dgm