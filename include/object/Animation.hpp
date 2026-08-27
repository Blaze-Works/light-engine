#pragma once

#include <object/Object3D.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

namespace blaze::lightEngine {

enum class AnimationInterpolation {
	Linear,
	Step,
	CubicSpline
};

enum class AnimationPath {
	Translation,
	Rotation,
	Scale,
	Weights
};

struct AnimationTrack {
	Object3D* target = nullptr;
	AnimationPath path = AnimationPath::Translation;
	AnimationInterpolation interpolation = AnimationInterpolation::Linear;
	std::vector<float> times;
	std::vector<float> values;
};

struct AnimationClip {
	std::string name;
	float duration = 0.0f;
	std::vector<AnimationTrack> tracks;
};

class AnimationPlayer {
public:
	float time = 0.0f;
	float speed = 1.0f;
	bool looping = true;
	bool playing = true;

	int lastSampledTracks = 0;

	void clear() {
		this->tracks.clear();
		this->name.clear();
		this->duration = 0.0f;
		this->time = 0.0f;
		this->lastSampledTracks = 0;
	}

	void setClip(const AnimationClip& clip) {
		this->tracks = clip.tracks;
		this->name= clip.name;
		this->duration = clip.duration;
		if (this->duration <= 0.0f) {
			for (const auto& tr : this->tracks) {
				if (!tr.times.empty()) this->duration = std::max(this->duration, tr.times.back());
			}
		}
		this->time = 0.0f;
		this->lastSampledTracks = 0;
	}

	void setClip(const AnimationClip* clip) {
		if (!clip) { this->clear(); return; }
		this->setClip(*clip);
	}

	const std::string& clipName() const { return this->name; }
	float getDuration() const { return this->duration; }
	int trackCount() const { return static_cast<int>(this->tracks.size()); }

	void play() { this->playing = true; }
	void pause() { this->playing = false; }
	void stop() { this->playing = false; this->time = 0.0f; }

	void evaluate(float t) {
		this->time = t;
		sampleAll();
	}

	void update(float deltaSeconds);

private:
	std::vector<AnimationTrack> tracks;
	std::string name;
	float duration = 0.0f;

	void sampleAll();
	static void sampleTrack(const AnimationTrack& track, float t);
	static void findKey(const std::vector<float>& times, float t, int& i0, int& i1, float& alpha);
};

} // namespace blaze::lightEngine

