#pragma once

#include "curen_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace Curen {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale {1.0f, 1.0f, 1.0f};
		glm::vec3 rotation{};
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class CurenObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, CurenObject>;
	
		static CurenObject createObject() {
			static id_t currentId = 0;
			return CurenObject{ currentId++ };
		}

		CurenObject(const CurenObject&) = delete;
		CurenObject& operator=(const CurenObject&) = delete;
		CurenObject(CurenObject&&) = default;
		CurenObject& operator=(CurenObject&&) = default;

		id_t getId() { return m_id; }

		std::shared_ptr<CurenModel> model{};
		glm::vec3 color{};
		TransformComponent transformComponent{};

	private:
		CurenObject(id_t objectId) : m_id{ objectId } {};

		id_t m_id;
	};
}