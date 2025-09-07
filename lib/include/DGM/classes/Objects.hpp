/**
 *  \file 			Objects.hpp
 *  \author			doomista
 *  \description	Declarations of basic dmg logic objects
 */

#pragma once

#include <DGM/classes/Compatibility.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cassert>
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
        virtual const sf::Vector2f& getPosition() const noexcept = 0;

        virtual void setPosition(const sf::Vector2f& position) noexcept = 0;

        virtual void move(const sf::Vector2f& position) noexcept = 0;

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
    class [[nodiscard]] Circle final : public Object
    {
    public:
        constexpr Circle(
            const sf::Vector2f& position, const float radius) noexcept
            : position(position), radius(radius)
        {
        }

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
        [[nodiscard]] constexpr const sf::Vector2f&
        getPosition() const noexcept override
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
         *  \brief Sets new position of circle with SFML vector
         */
        constexpr void
        setPosition(const sf::Vector2f& newPosition) noexcept override
        {
            position = newPosition;
        }

        /**
         *  \brief Sets radius
         */
        constexpr void setRadius(const float newRadius) noexcept
        {
            radius = newRadius;
        }

        /**
         *  \brief Moves object
         */
        void move(const sf::Vector2f& forward) noexcept override
        {
            position += forward;
        }

    protected:
        sf::Vector2f position = { 0.f, 0.f }; ///< Position of the center
        float radius = 0.f;                   ///< Radius of circle
    };

    /**
     *  \brief Logic class for rectangle
     *
     *  \details Rectangle is defined by position
     *  of its top left corner and its dimesions.
     */
    class [[nodiscard]] Rect final : public Object
    {
    public:
        constexpr Rect(
            const sf::Vector2f& position, const sf::Vector2f& size) noexcept
            : position(position), size(size)
        {
        }

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
        [[nodiscard]] constexpr const sf::Vector2f&
        getPosition() const noexcept override
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

        [[nodiscard]] constexpr sf::Vector2f getCenter() const noexcept
        {
            return getPosition() + getSize() / 2.f;
        }

        /**
         *  \brief Set position of top-left corner
         */
        constexpr void
        setPosition(const sf::Vector2f& newPosition) noexcept override
        {
            position = newPosition;
        }

        /**
         *  \brief Moves object
         */
        void move(const sf::Vector2f& forward) noexcept override
        {
            position += forward;
        }

        /**
         *  \brief Set dimensions of rectangle
         */
        constexpr void setSize(const sf::Vector2f& newSize) noexcept
        {
            size = newSize;
        }

    protected:
        sf::Vector2f position = { 0.f,
                                  0.f };  ///< Position of the topleft corner
        sf::Vector2f size = { 0.f, 0.f }; ///< Dimensions of the rectangle
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
    class [[nodiscard]] VisionCone final : public Object
    {
    public:
        VisionCone(const float length, const float width);

    public:
        /**
         *  \brief Displays object in the window
         *
         *  This method is slow and is reserved for debug purposes
         */
        void debugRender(
            dgm::Window& window, sf::Color color = sf::Color::Yellow) const;

        virtual constexpr const sf::Vector2f&
        getPosition() const noexcept override
        {
            return position;
        }

        virtual constexpr void
        setPosition(const sf::Vector2f& newPosition) noexcept override
        {
            position = newPosition;
        }

        virtual void move(const sf::Vector2f& direction) noexcept override
        {
            position += direction;
        }

        [[nodiscard]] constexpr sf::Angle getRotation() const noexcept
        {
            return rotation;
        }

        void setRotation(const sf::Angle angle) noexcept;
        void rotate(const sf::Angle angle) noexcept;

        [[nodiscard]] float getLength() const noexcept
        {
            return forward.length();
        }

        [[nodiscard]] constexpr float getWidth() const noexcept
        {
            return width;
        }

    private:
        sf::Vector2f position = { 0.f, 0.f };
        sf::Vector2f forward = { 0.f, 0.f };
        float width = 0.f;
        sf::Angle rotation = sf::Angle::Zero;
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
    template<class T>
    class [[nodiscard]] GenericMesh final : public Object
    {
    public:
        using DataType = T;

    public:
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
        GenericMesh(
            const std::vector<DataType>& data,
            const sf::Vector2u& dataSize,
            const sf::Vector2u& voxelSize)
            : data(data), dataSize(dataSize), voxelSize(voxelSize)
        {
            assert(
                data.size() == dataSize.x * dataSize.y
                && "Mesh data.size() must equal dataSize.x * dataSize.y");
        }

        /**
         *  \brief Construct mesh populated with default values
         */
        GenericMesh(const sf::Vector2u& dataSize, const sf::Vector2u& voxelSize)
            : GenericMesh(
                  std::vector<DataType>(dataSize.x * dataSize.y, DataType {}),
                  dataSize,
                  voxelSize)
        {
        }

        GenericMesh(GenericMesh<T>&&) = default;
        GenericMesh(const GenericMesh<T>&) = delete;

        [[nodiscard]] GenericMesh clone() const
        {
            return GenericMesh<T>(data, dataSize, voxelSize);
        }

    public:
#ifdef ANDROID
        [[nodiscard]] constexpr inline DataType&
        operator[](std::size_t index) noexcept
        {
            return data[index];
        }

        [[nodiscard]] constexpr inline const DataType&
        operator[](std::size_t index) const noexcept
        {
            return data[index];
        }

        [[nodiscard]] constexpr inline DataType&
        operator[](const sf::Vector2u& pos) noexcept
        {
            return data[pos.y * dataSize.x + pos.x];
        }

        [[nodiscard]] constexpr inline const DataType&
        operator[](const sf::Vector2u& pos) const noexcept
        {
            return data[pos.y * dataSize.x + pos.x];
        }
#else
        [[nodiscard]] constexpr inline auto&&
        operator[](this auto&& self, std::size_t index) noexcept
        {
            return self.data[index];
        }

        [[nodiscard]] constexpr inline auto&&
        operator[](this auto&& self, const sf::Vector2u& pos) noexcept
        {
            return self.data[pos.y * self.dataSize.x + pos.x];
        }
#endif

        [[nodiscard]] constexpr inline std::vector<DataType>&
        getRawData() noexcept
        {
            return data;
        }

        [[nodiscard]] constexpr inline const std::vector<DataType>&
        getRawConstData() const noexcept
        {
            return data;
        }

        /**
         *  \brief get position of top-left corner
         */
        [[nodiscard]] constexpr inline const sf::Vector2f&
        getPosition() const noexcept override
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
        constexpr void
        setPosition(const sf::Vector2f& newPosition) noexcept override
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
        constexpr void setVoxelSize(const sf::Vector2u& size) noexcept
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
        void setDataSize(const sf::Vector2u& size)
        {
            data.clear();
            data.resize(size.x * size.y, 0);
            dataSize = size;
        }

        /**
         *  \brief Moves object
         */
        constexpr void move(const sf::Vector2f& forward) noexcept override
        {
            position += forward;
        }

    protected:
        std::vector<DataType> data = {}; ///< Array for holding collision data
        sf::Vector2f position = { 0.f, 0.f }; ///< Position of top-left corner
        sf::Vector2u dataSize = {
            0u, 0u
        }; ///< dataSize.x * dataSize.y is size of data array
        sf::Vector2u voxelSize = {
            0u, 0u
        }; ///< How big rectangle does single cell of data represents
    };

    using Mesh = GenericMesh<int>;
} // namespace dgm