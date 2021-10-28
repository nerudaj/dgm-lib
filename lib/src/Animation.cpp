#include <DGM/classes/Animation.hpp>
#include <DGM/classes/Error.hpp>

#include <Strings.hpp>
#include <json.hpp>

#include <cassert>
#include <fstream>

using dgm::Animation;
using dgm::AnimationStates;

const std::shared_ptr<AnimationStates> NULL_STATES = std::make_shared<AnimationStates>(AnimationStates({
	{ "NullState", dgm::Clip() }
}));

bool Animation::update(const dgm::Time &time) {
	elapsedTime += time.getElapsed();
	if (elapsedTime > timePerFrame) {
		elapsedTime -= timePerFrame;

		currentFrameIndex++;
		if (currentFrameIndex == currentState->second.getFrameCount()) {
			currentFrameIndex = 0;
			if (not isLooping()) {
				return false;
			}
		}

		updateSpriteTextureRect();
	}

	return true;
}

void Animation::setState(const std::string &state, bool shouldLoop) {
	if (isCurrentStateValid() && currentState->first == state) return;

	auto newState = states->find(state);
	if (newState == states->end()) {
		throw dgm::GeneralException("Cannot find animation state '" + state + "'");
	}

	currentState = newState;
	setLooping(shouldLoop);

	reset();
}

void Animation::bindSprite(sf::Sprite *sprite) {
	assert(sprite != nullptr);
	boundSprite = sprite;
}

void Animation::setSpeed(int framePerSecond) {
	timePerFrame = sf::milliseconds(1000 / framePerSecond);
}

Animation::Animation() {
	states = NULL_STATES;

	boundSprite = nullptr;
	elapsedTime = sf::seconds(0);

	setSpeed(30);
	currentFrameIndex = 0;
	currentState = states->begin();

	setLooping(false);
}

Animation::Animation(const std::string &filename, int framesPerSecond) {
	states = Animation::loadStatesFromFile(filename);

	boundSprite = nullptr;
	elapsedTime = sf::seconds(0);

	setSpeed(framesPerSecond);
	currentFrameIndex = 0;
	currentState = states->end();

	setLooping(false);
}


Animation::Animation(const std::shared_ptr<AnimationStates> &states, int framesPerSecond) {
	Animation::states = states;

	boundSprite = nullptr;
	elapsedTime = sf::seconds(0);

	setSpeed(framesPerSecond);
	currentFrameIndex = 0;
	currentState = states->end();

	setLooping(false);
}

sf::Vector2u getVec2iFromJsonArray(const nlohmann::json &json) {
	return { json[0], json[1] };
}

sf::IntRect getIntRectFromJsonArray(const nlohmann::json &json) {
	return { json[0], json[1], json[2], json[3] };
}

std::shared_ptr<AnimationStates> Animation::loadStatesFromFile(const std::string &filename) {
	std::ifstream load(filename);
	nlohmann::json file;
	load >> file;
	load.close();
	load.clear();

	AnimationStates result;

	// Parse defaults from file
	sf::Vector2u defaultFrameSize = { 8, 8 };
	sf::Vector2u defaultFrameOffset = { 0, 0 };
	if (file.contains("defaults")) {
		if (file["defaults"].contains("frame")) {
			auto frame = file["defaults"]["frame"];
			if (frame.contains("size")) {
				defaultFrameSize = getVec2iFromJsonArray(frame["size"]);
			}
			if (frame.contains("offset")) {
				defaultFrameOffset = getVec2iFromJsonArray(frame["offset"]);
			}
		}
	}

	// Parse through states
	for (auto&& state : file["states"].items()) {
		sf::Vector2u frameSize = defaultFrameSize;
		sf::Vector2u frameOffset = defaultFrameOffset;
		unsigned frameCount = 0;

		std::string name = state.key();
		auto data = state.value();

		auto bounds = getIntRectFromJsonArray(data.at("bounds"));
		if (data.contains("frame")) {
			auto frame = data["frame"];
			if (frame.contains("size")) {
				frameSize = getVec2iFromJsonArray(frame["size"]);
			}
			if (frame.contains("offset")) {
				frameOffset = getVec2iFromJsonArray(frame["offset"]);
			}
			if (frame.contains("count")) {
				frameCount = frame["count"];
			}
		}

		result[name].init(frameSize, bounds, frameCount, frameOffset);
	}

	return std::make_shared<AnimationStates>(result);
}
