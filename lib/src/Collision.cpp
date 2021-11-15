#include <DGM/classes/Collision.hpp>

#include <SFML/Graphics/Rect.hpp>

bool dgm::Collision::basic(const dgm::Rect &rect, const sf::Vector2i &point) {
	sf::Vector2f pos = rect.getPosition();
	sf::Vector2f sze = rect.getSize();

	return (pos.x <= point.x && point.x <= (pos.x + sze.x) && pos.y <= point.y && point.y <= (pos.y + sze.y));
}
		
bool dgm::Collision::basic(const dgm::Circle &circle, const sf::Vector2i &point) {
	sf::Vector2f cpos = circle.getPosition();
	
	float dX = abs(point.x - cpos.x);
	float dY = abs(point.y - cpos.y);
	
	return (dX * dX + dY * dY) < (circle.getRadius() * circle.getRadius());
}

bool dgm::Collision::basic(const dgm::Circle &A, const dgm::Circle &B) {
	sf::Vector2f cpos1 = A.getPosition();
	sf::Vector2f cpos2 = B.getPosition();
	
	float dX = abs(cpos1.x - cpos2.x);
	float dY = abs(cpos1.y - cpos2.y);

	if ((dX * dX + dY * dY) < ((A.getRadius() + B.getRadius()) * (A.getRadius() + B.getRadius()))) {
		return true;
	}

	return false;
}

bool dgm::Collision::basic(const dgm::Rect &A, const dgm::Circle &B) {
	sf::Vector2f apos = A.getPosition();
	sf::Vector2f size = A.getSize();
	sf::Vector2f bpos = B.getPosition();
	// Closest coordinates to circle on border of the rect
	// Courtesy of lazyfoo.net tutorials
	float cX = 0.f, cY = 0.f;
	
	if (bpos.x < apos.x) {
		cX = apos.x;
	}
	else if (bpos.x > (apos.x + size.x)) {
		cX = apos.x + size.x;
	}
	else {
		cX = bpos.x;
	}

	if (bpos.y < apos.y) {
		cY = apos.y;
	}
	else if (bpos.y > (apos.y + size.y)) {
		cY = apos.y + size.y;
	}
	else {
		cY = bpos.y;
	}

cX = bpos.x - cX;
cY = bpos.y - cY;

	if (sqrt(cX * cX + cY * cY) < B.getRadius()) {
		return true;
	}
	
return false;
}

bool dgm::Collision::basic(const dgm::Rect &A, const dgm::Rect &B) {
	sf::Vector2f cA = A.getPosition() + A.getSize() / 2.f;
	sf::Vector2f cB = B.getPosition() + B.getSize() / 2.f;
	
	float dX = abs(cA.x - cB.x) * 2.f;
	float dY = abs(cA.y - cB.y) * 2.f;
	
	if (dX <= (A.getSize().x + B.getSize().x) && dY <= (A.getSize().y + B.getSize().y)) {
		return true;
	}
	
	return false;
}

static sf::IntRect normalizeBoundaries(const sf::IntRect &src, const dgm::Mesh &mesh) {
	sf::IntRect dst;

	sf::Vector2u meshSize = mesh.getDataSize();
	sf::Vector2u tileSize = mesh.getVoxelSize();
	
	// TODO: revisit
	dst.left	= src.left / tileSize.x;
	dst.top		= src.top  / tileSize.y;
	dst.width	= (src.left + src.width)  / tileSize.x;
	dst.height	= (src.top  + src.height) / tileSize.y;
	
	if (dst.left < 0) dst.left = 0;
	if (unsigned(dst.width)  >= meshSize.x)	dst.width = meshSize.x - 1;
	if (dst.top  < 0) dst.top = 0;
	if (unsigned(dst.height) >= meshSize.y)	dst.height = meshSize.y - 1;

	return dst;
}

bool dgm::Collision::basic(const dgm::Mesh &A, const dgm::Circle &B, std::size_t *meshHitPosition) {
	sf::Vector2f radius(B.getRadius(), B.getRadius());
	sf::IntRect outBody(sf::FloatRect(B.getPosition() - radius - A.getPosition(), radius * 2.f));
	sf::IntRect bounds = normalizeBoundaries(outBody, A);

	sf::Vector2f tileSize(A.getVoxelSize());
	unsigned meshWidth = A.getDataSize().x;

	dgm::Rect box(sf::Vector2f(0.f, 0.f), tileSize);
	
	for (int y = bounds.top; y <= bounds.height; y++) {
		for (int x = bounds.left; x <= bounds.width; x++) {
			if (A[y * meshWidth + x] > 0) {
				box.setPosition(sf::Vector2f(float(x) * tileSize.x, float(y) * tileSize.y) + A.getPosition());
				
				if (dgm::Collision::basic(box, B)) {
					if (meshHitPosition != nullptr) {
						(*meshHitPosition) = y * meshWidth + x;
					}
					return true;
				}
			}
		}
	}
	
	return false;
}

bool dgm::Collision::basic(const dgm::Mesh &A, const dgm::Rect &B, std::size_t *meshHitPosition) {
	sf::IntRect outBody(sf::FloatRect(B.getPosition() - A.getPosition(), B.getSize()));
	sf::IntRect bounds = normalizeBoundaries(outBody, A);

	sf::Vector2u meshSize = A.getDataSize();
	unsigned meshWidth = A.getDataSize().x;

	for (int y = bounds.top; y <= bounds.height; y++) {
		for (int x = bounds.left; x <= bounds.width; x++) {
			if (A[y * meshWidth + x] > 0) {
				if (meshHitPosition != nullptr) {
					(*meshHitPosition) = y * meshWidth + x;
				}
				return true;
			}
		}
	}

	return false;
}

template<typename T>
bool dgm::Collision::advanced(const dgm::Mesh &mesh, T body, sf::Vector2f &forward, std::size_t *meshHitPosition) {
	auto localForward = forward;

	body.move(localForward);
	if (!dgm::Collision::basic(mesh, body)) {
		return false;
	}
	
	body.move(0.f, -localForward.y);
	if (!dgm::Collision::basic(mesh, body, meshHitPosition)) {
		forward.y = 0.f;
		return true;
	}
	
	body.move(-localForward.x, localForward.y);
	if (!dgm::Collision::basic(mesh, body, meshHitPosition)) {
		forward.x = 0.f;
		return true;
	}
	
	forward = sf::Vector2f (0.f, 0.f);
	return true;
}

template bool dgm::Collision::advanced<dgm::Circle>(const dgm::Mesh &mesh, dgm::Circle body, sf::Vector2f &forward, std::size_t *meshHitPosition);
template bool dgm::Collision::advanced<dgm::Rect>(const dgm::Mesh &mesh, dgm::Rect body, sf::Vector2f &forward, std::size_t *meshHitPosition);