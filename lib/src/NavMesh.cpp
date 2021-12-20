#include "DGM/classes/NavMesh.hpp"

void dgm::NavMesh::connectTwoJumpPoints(const sf::Vector2u& a, const sf::Vector2u& b) {
    const float dx = (static_cast<float>(a.x) - b.x) * mesh.getVoxelSize().x;
    const float dy = (static_cast<float>(a.y) - b.y) * mesh.getVoxelSize().y;
    const float distance = std::sqrt(dx * dx + dy * dy);
    jumpPointConnections[a].push_back(Connection(a, distance));
    jumpPointConnections[b].push_back(Connection(b, distance));
}

void dgm::NavMesh::discoverJumpPoints() {
    auto isCornerPoint = [&] (const sf::Vector2u& point) -> bool {
        /**
         *  Test if this point is at the tip of some impassable tile, eg:
         *  #   #
         *  # p     <-- there the p is corner point
         *  # # #
         *
         *
         *  #   #
         *  # p #   <-- there p is not a corner point
         *  #   #
         *
         *  #   #
         *    p     <-- multiple corners
         *  # # #
         */
        const bool northWestCorner = mesh.at(point.x - 1, point.y - 1) > 0 && mesh.at(point.x - 1, point.y) <= 0 && mesh.at(point.x, point.y - 1);
        const bool northEastCorner = mesh.at(point.x + 1, point.y - 1) > 0 && mesh.at(point.x + 1, point.y) <= 0 && mesh.at(point.x, point.y - 1);
        const bool southWestCorner = mesh.at(point.x - 1, point.y + 1) > 0 && mesh.at(point.x - 1, point.y) <= 0 && mesh.at(point.x, point.y + 1);
        const bool southEastCorner = mesh.at(point.x + 1, point.y + 1) > 0 && mesh.at(point.x + 1, point.y) <= 0 && mesh.at(point.x, point.y + 1);

        return northWestCorner || northEastCorner || southWestCorner || southEastCorner;
    };

    for (unsigned y = 1; y < mesh.getDataSize().y - 1; y++) {
        for (unsigned x = 1; x < mesh.getDataSize().x - 1; x++) {
            // Skip impassable blocks
            if (mesh.at(x, y) > 0) continue;

            sf::Vector2u point(x, y);
            if (isCornerPoint(point)) {
                jumpPointConnections[point] = {};
            }
        }
    }
}

dgm::NavMesh::NavMesh(const dgm::Mesh& mesh) : mesh(mesh) {
    discoverJumpPoints();

    for (auto&& [point, _] : jumpPointConnections)
        discoverConnectionsForJumpPoint(point);
}
