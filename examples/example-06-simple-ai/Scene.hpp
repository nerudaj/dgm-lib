#pragma once

class Level;
class Actor;

struct Scene
{
    Level& level;
    Actor& player;
    Actor& npc;
};
