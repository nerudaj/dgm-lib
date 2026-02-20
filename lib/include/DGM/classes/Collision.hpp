#pragma once

#include <DGM/classes/Objects.hpp>

namespace dgm
{
    class Collision
    {
    public:
        /**
         * 	\brief Tests collision between a rectangle and a point
         *
         * 	\param Existing rectangle object
         *  \param Existing sf::Vector2i serving as point (format returned by
         * mouse position function)
         *
         * 	\return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(const dgm::Rect& rect, const sf::Vector2i& point)
        {
            return basic(rect, sf::Vector2f(point));
        }

        /**
         * 	\brief Tests collision between a rectangle and a point
         *
         * 	\param Existing rectangle object
         *  \param Existing sf::Vector2i serving as point (format returned by
         * mouse position function)
         *
         * 	\return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(const dgm::Rect& rect, const sf::Vector2f& point);

        /**
         * 	\brief Tests collision between a circle and a point
         *
         * 	\param Existing circle object
         *  \param Existing sf::Vector2i serving as point (format returned by
         * mouse position function)
         *
         * 	\return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(const dgm::Circle& A, const dgm::Circle& B);

        /**
         * 	\brief Tests collision between a circle and a circle
         *
         * 	\param Existing circle object
         *  \param Existing circle object
         *
         * 	\return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(const dgm::Circle& circle, const sf::Vector2i& point)
        {
            return basic(circle, sf::Vector2f(point));
        }

        /**
         * 	\brief Tests collision between a circle and a circle
         *
         * 	\param Existing circle object
         *  \param Existing circle object
         *
         * 	\return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(const dgm::Circle& circle, const sf::Vector2f& point);

        /**
         * 	\brief Tests collision between a circle and a rectange
         *
         * 	\param Existing rectangle object
         *  \param Existing circle object
         *
         * 	\return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(const dgm::Rect& A, const dgm::Circle& B);

        /**
         * 	\brief Tests collision between two rectangles
         *
         * 	\param Existing rectangle object
         *  \param Existing rectangle object
         *
         * 	\return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(const dgm::Rect& A, const dgm::Rect& B);

        /**
         *  \brief Tests collision between mesh and a circle
         *
         *  \param Existing mesh object
         *  \param Existing circle object
         *
         *  \return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(
            const dgm::Mesh& A,
            const dgm::Circle& B,
            std::size_t* meshHitPosition = nullptr);

        /**
         *  \brief Tests collision between mesh and a rectangle
         *
         *  \param Existing mesh object
         *  \param Existing rectangle object
         *
         *  \return TRUE if collision was detected. FALSE otherwise
         */
        static bool basic(
            const dgm::Mesh& A,
            const dgm::Rect& B,
            std::size_t* meshHitPosition = nullptr);

        /**
         * \brief Detect presense of a circle within a vision cone
         */
        [[deprecated("Use the method with swapped parameters")]]
        static bool basic(const dgm::Circle& c, const dgm::VisionCone& cone)
        {
            return basic(cone, c);
        }

        /**
         * \brief Detect presense of a circle within a vision cone
         */
        static bool basic(const dgm::VisionCone& cone, const dgm::Circle& c);

        /**
         *  \brief Detect presense of a point within a vision cone
         *
         * \param cone Vision cone to check within
         * \param p Point to check
         * \return TRUE if point is within the cone, FALSE otherwise
         */
        [[deprecated("Use the method with swapped parameters")]]
        static bool basic(const sf::Vector2i& p, const dgm::VisionCone& cone)
        {
            return basic(cone, p);
        }

        /**
         *  \brief Detect presense of a point within a vision cone
         *
         * \param cone Vision cone to check within
         * \param p Point to check
         * \return TRUE if point is within the cone, FALSE otherwise
         */
        static bool basic(const dgm::VisionCone& cone, const sf::Vector2i& p)
        {
            return basic(cone, sf::Vector2f(p));
        }

        /**
         *  \brief Detect presense of a point within a vision cone
         *
         * \param cone Vision cone to check within
         * \param p Point to check
         * \return TRUE if point is within the cone, FALSE otherwise
         */
        static bool basic(const dgm::VisionCone& cone, const sf::Vector2f& p);

        /**
         *  \brief Detect presense of a rectangle within a vision cone
         *
         * \param cone Vision cone to check within
         * \param r Rectange to check
         * \return TRUE if rectangle is within the cone, FALSE otherwise
         */
        [[deprecated("Use the method with swapped parameters")]]
        static bool basic(const dgm::VisionCone& cone, const dgm::Rect& r);

        /**
         *  \brief Elaborates movement of an object within a mesh
         *
         *  \param  [in]     Existing mesh object
         *  \param  [in]     Existing object
         *  \param  [inout]  Movement defining forward vector
         *  \param  [out]    Which tile was hit
         *
         *  Function assumes that mesh is a level and body is an object within
         * it that wants to move using the forward vector. Function tests
         * possible collisions with mesh, modifying forward in such way that
         * applying it to body would not result in collision. If any collision
         * was detected, true will be returned and index of collided mesh tile
         * will be stored in meshHitPosition (if was specified).
         *
         *  \return TRUE if collision was detected. FALSE otherwise
         */
        template<class T>
        static bool advanced(
            const dgm::Mesh& mesh,
            T body,
            sf::Vector2f& forward,
            std::size_t* meshHitPosition = nullptr);

        template<class T>
        static bool
        advanced(const dgm::Rect& anchor, T body, sf::Vector2f& forward);

        template<class T>
        static bool
        advanced(const dgm::Circle& anchor, T body, sf::Vector2f& forward);
    };
} // namespace dgm
