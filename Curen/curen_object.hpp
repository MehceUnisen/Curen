#pragma once

#include "curen_model.hpp"

#include <memory>

namespace Curen {

	struct Transform2DComponent {
		glm::vec2 translation{};
		glm::vec2 scale {1.0f, 1.0f};
		float rotation;
		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);

			glm::mat2 rotationMatrix {{c, s}, { -s, c }};
			glm::mat2 scaleMatrix({scale.x, 0.0f}, { 0.0f , scale.y});
			
			return rotationMatrix * scaleMatrix;
		}
	};

	class CurenObject {
	public:
		using id_t = unsigned int;

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
		Transform2DComponent transform2DComponent{};

	private:
		CurenObject(id_t objectId) : m_id{ objectId } {};

		id_t m_id;
	};
}