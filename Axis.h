#pragma once

enum class Axis : unsigned char{
	X = 'x', Y = 'y', Z = 'z'
};

#ifdef glm_glm

namespace AxisVector {
	static const glm::vec3 X = glm::vec3(1.f, 0.f, 0.f);
	static const glm::vec3 Y = glm::vec3(0.f, 1.f, 0.f);
	static const glm::vec3 Z = glm::vec3(0.f, 0.f, 1.f);
};

#endif