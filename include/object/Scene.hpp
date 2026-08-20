#pragma once

#include <object/Fog.hpp>
#include <object/Object3D.hpp>

#include <memory>
#include <optional>

namespace blaze::lightEngine {

class Scene : public Object3D {
public:
	Scene();
	~Scene() override = default;
	
	std::optional<int> background;
	std::optional<Fog> fog;
	std::optional<FogExp2> fogExp2;
	Object3D* environment = nullptr;
	bool autoUpdate = true;

	template<typename T, typename... Args>
	T* create(Args&&... args) {
		static_assert(std::is_base_of_v<Object3D, T>, "T must derive from Object3D");
		auto owned = std::make_unique<T>(std::forward<Args>(args)...);
		T* raw = owned.get();
		ownedChildren.push_back(std::move(owned));
		this->add(raw);
		return raw;
	}

	Object3D* adopt(std::unique_ptr<Object3D> object);
	void destroy(Object3D* object);
	void clearOwned();

private:
	std::vector<std::unique_ptr<Object3D>> ownedChildren;
};

} // namespace blaze::lightEngine
