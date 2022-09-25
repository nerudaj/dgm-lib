#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Error.hpp>

#include <Strings.hpp>
#include <json.hpp>

#include <cassert>
#include <fstream>

using dgm::Animation;
using dgm::AnimationStates;

bool Animation::update(const dgm::Time& time)
{
	elapsedTime += time.getElapsed();
	if (elapsedTime > timePerFrame)
	{
		elapsedTime -= timePerFrame;

		currentFrameIndex++;
		if (currentFrameIndex == currentState->second.getFrameCount())
		{
			currentFrameIndex = 0;
			if (not isLooping())
			{
				return false;
			}
		}

		updateSpriteTextureRect();
	}

	return true;
}

void Animation::setState(const std::string& state, bool shouldLoop)
{
	if (isCurrentStateValid() && currentState->first == state) return;

	auto newState = states->find(state);
	if (newState == states->end())
	{
		throw dgm::GeneralException("Cannot find animation state '" + state + "'");
	}

	currentState = newState;
	setLooping(shouldLoop);

	reset();
}

void Animation::bindSprite(sf::Sprite& sprite)
{
	boundSprite = &sprite;
}

void Animation::setSpeed(unsigned framesPerSecond)
{
	assert(framesPerSecond > 0);
	timePerFrame = sf::milliseconds(1000 / framesPerSecond);
}

Animation::Animation()
{
	//states = NULL_STATES;

	boundSprite = nullptr;
	elapsedTime = sf::seconds(0);

	setSpeed(30);
	currentFrameIndex = 0;
	currentState = states->begin();

	setLooping(false);
}

Animation::Animation(const std::string& filename, int framesPerSecond)
{
	// FIXME: states = Animation::loadStatesFromFile(filename);

	boundSprite = nullptr;
	elapsedTime = sf::seconds(0);

	setSpeed(framesPerSecond);
	currentFrameIndex = 0;
	currentState = states->end();

	setLooping(false);
}


Animation::Animation(const std::shared_ptr<AnimationStates>& states, int framesPerSecond)
{
	Animation::states = states;

	boundSprite = nullptr;
	elapsedTime = sf::seconds(0);

	setSpeed(framesPerSecond);
	currentFrameIndex = 0;
	currentState = states->end();

	setLooping(false);
}

