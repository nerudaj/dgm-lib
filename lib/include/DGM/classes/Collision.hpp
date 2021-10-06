/**
 * \file Collision.hpp
 * \author doomista
 */
 
#pragma once

#include <DGM/classes/Objects.hpp>

namespace dgm {
	class Collision {
	public:
		/**
		 * 	\brief Tests collision between a rectangle and a point
		 * 
		 * 	\param Existing rectangle object
		 *  \param Existing sf::Vector2i serving as point (format returned by mouse position function)
		 * 
		 * 	\return TRUE if collision was detected. FALSE otherwise
		 */
		static bool basic (const dgm::Rect &rect, const sf::Vector2i &point);
		
		/**
		 * 	\brief Tests collision between a circle and a point
		 * 
		 * 	\param Existing circle object
		 *  \param Existing sf::Vector2i serving as point (format returned by mouse position function)
		 * 
		 * 	\return TRUE if collision was detected. FALSE otherwise
		 */
		static bool basic (const dgm::Circle &A, const dgm::Circle &B);
		
		/**
		 * 	\brief Tests collision between a circle and a circle
		 * 
		 * 	\param Existing circle object
		 *  \param Existing circle object
		 * 
		 * 	\return TRUE if collision was detected. FALSE otherwise
		 */
		static bool basic (const dgm::Circle &circle, const sf::Vector2i &point);
		
		/**
		 * 	\brief Tests collision between a circle and a rectange
		 * 
		 * 	\param Existing rectangle object
		 *  \param Existing circle object
		 * 
		 * 	\return TRUE if collision was detected. FALSE otherwise
		 */
		static bool basic (const dgm::Rect &A, const dgm::Circle &B);
		
		/**
		 * 	\brief Tests collision between two rectangles
		 * 
		 * 	\param Existing rectangle object
		 *  \param Existing rectangle object
		 * 
		 * 	\return TRUE if collision was detected. FALSE otherwise
		 */
		static bool basic (const dgm::Rect &A, const dgm::Rect &B);
		
		/**
		 *  \brief Tests collision between mesh and a circle
		 * 
		 *  \param Existing mesh object
		 *  \param Existing circle object
		 * 
		 *  \return TRUE if collision was detected. FALSE otherwise
		 */
		static bool basic (const dgm::Mesh &A, const dgm::Circle &B, std::size_t *meshHitPosition = nullptr);
		
		/**
		 *  \brief Tests collision between mesh and a rectangle
		 * 
		 *  \param Existing mesh object
		 *  \param Existing rectangle object
		 * 
		 *  \return TRUE if collision was detected. FALSE otherwise
		 */
		static bool basic (const dgm::Mesh &A, const dgm::Rect &B, std::size_t *meshHitPosition = nullptr);
		
		/**
		 *  \brief Elaborates movement of an object within a mesh
		 * 
		 *  \param  [in]     Existing mesh object
		 *  \param  [in]     Existing object
		 *  \param  [inout]  Movement defining forward vector
		 *  \param  [out]    Which tile was hit
		 * 
		 *  Function assumes that mesh is a level and body is an object within it
		 *  that wants to move using the forward vector. Function tests possible
		 *  collisions with mesh, modifying forward in such way that applying it to
		 *  body would not result in collision. If any collision was detected, true
		 *  will be returned and index of collided mesh tile will be stored in
		 *  meshHitPosition (if was specified).
		 * 
		 *  \return TRUE if collision was detected. FALSE otherwise
		 */
		template<class T>
		static bool advanced(const dgm::Mesh &mesh, T body, sf::Vector2f &forward, std::size_t *meshHitPosition = nullptr);
	};
}