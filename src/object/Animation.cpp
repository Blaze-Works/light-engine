#include <object/Animation.hpp>
#include <object/Mesh.hpp>

#include <algorithm>
#include <cmath>

namespace blaze::lightEngine {

void AnimationPlayer::findKey(const std::vector<float>& times, float t, int& i0, int& i1, float& alpha) {
	const int n = static_cast<int>(times.size());
	if (n == 0) { i0 = i1 = 0; alpha = 0; return; }
	if (n == 1 || t <= times.front()) { i0 = i1 = 0; alpha = 0; return; }
	if (t >= times.back()) { i0 = i1 = n - 1; alpha = 0; return; }

	int lo = 0, hi = n - 1;
	while (lo + 1 < hi) {
		int mid = (lo + hi) / 2;
		if (times[mid] <= t) lo = mid;
		else hi = mid;
	}

	i0 = lo;
	i1 = hi;
	float dt = times[i1] - times[i0];
	alpha = (dt > 1e-8f) ? (t - times[i0]) / dt : 0.0f;
}

void AnimationPlayer::sampleTrack(const AnimationTrack& track, float t) {
	if (!track.target || track.times.empty() || track.values.empty()) return;

	int i0, i1;
	float alpha;
	findKey(track.times, t, i0, i1, alpha);

	const int comps = (track.path == AnimationPath::Rotation) ? 4 : (track.path == AnimationPath::Weights) ? 0 : 3;
	if (comps > 0) {
		size_t need = static_cast<size_t>(std::max(i0, i1) + 1) * static_cast<size_t>(comps);
		if (track.values.size() < need) return;
	}

	auto atVec3 = [&](int key) -> glm::vec3 {
		size_t o = static_cast<size_t>(key) * 3;
		return glm::vec3(track.values[o], track.values[o + 1], track.values[o + 2]);
	};
	auto atQuat = [&](int key) -> glm::quat {
		size_t o = static_cast<size_t>(key) * 4;
		return glm::normalize(glm::quat(track.values[o + 3], track.values[o], track.values[o + 1], track.values[o + 2]));
	};

	if (track.interpolation == AnimationInterpolation::Step) {
		alpha = 0.0f;
		i1 = i0;
	}

	switch (track.path) {
		case AnimationPath::Translation:
			track.target->position = glm::mix(atVec3(i0), atVec3(i1), alpha);
			break;
		case AnimationPath::Scale:
			track.target->scale = glm::mix(atVec3(i0), atVec3(i1), alpha);
			break;
		case AnimationPath::Rotation: {
			glm::quat a = atQuat(i0);
			glm::quat b = atQuat(i1);
			if (glm::dot(a, b) < 0.0f) b = -b;
			track.target->quaternion = glm::normalize(glm::slerp(a, b, alpha));
			break;
		}
		case AnimationPath::Weights: {
			if (track.times.empty()) break;
			const size_t keyCount = track.times.size();
			const size_t targetCount = track.values.size() / keyCount;
			if (targetCount == 0) break;

			auto atWeights = [&](int key) -> const float* {
				return track.values.data() + static_cast<size_t>(key) * targetCount;
			};

			const float* a = atWeights(i0);
			const float* b = atWeights(i1);

			auto applyToMesh = [&](Mesh* mesh) {
				if (!mesh) return;
				if (mesh->morphTargetInfluences.size() < targetCount) mesh->morphTargetInfluences.resize(targetCount, 0.0f);
				for (size_t i = 0; i < targetCount; i++) mesh->morphTargetInfluences[i] = a[i] * (1.0f - alpha) + b[i] * alpha;
				mesh->setMorphInfluence(0, mesh->morphTargetInfluences[0]);
			};

			if (auto* mesh = dynamic_cast<Mesh*>(track.target)) applyToMesh(mesh);
			else if (track.target) {
				for (Object3D* child : track.target->children) applyToMesh(dynamic_cast<Mesh*>(child));

				track.target->traverse([&](Object3D* obj) {
					applyToMesh(dynamic_cast<Mesh*>(obj));
				});
			}

			break;
		}
	}

	track.target->updateMatrix();
}

void AnimationPlayer::sampleAll() {
	this->lastSampledTracks = 0;
	for (const auto& track : this->tracks) {
		if (!track.target) continue;
		sampleTrack(track, this->time);
		this->lastSampledTracks++;
	}
}

void AnimationPlayer::update(float deltaSeconds) {
	if (!this->playing || this->tracks.empty()) {
		this->lastSampledTracks = 0;
		return;
	}

	this->time += deltaSeconds * this->speed;
	if (this->duration > 0.0f) {
		if (this->looping) {
			this->time = std::fmod(this->time, this->duration);
			if (this->time < 0.0f) this->time += this->duration;
		} else {
			this->time = std::min(std::max(this->time, 0.0f), this->duration);
		}
	}

	this->sampleAll();
}

} // namespace blaze::lightEngine

