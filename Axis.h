#pragma once

enum class Axis : unsigned char{
	None = 0,
	X = 'x', Y = 'y', Z = 'z',
	Pitch = 'p', Roll = 'r', Yaw = 'w',
	WorldPitch = 'P', WorldRoll = 'R', WorldYaw = 'W'
};

namespace AxisType {
	enum Type{
		Translational, Rotational
	};

	static inline AxisType::Type GetType(Axis in){
		switch (in){
		case Axis::X:
		case Axis::Y:
		case Axis::Z:
			return Type::Translational;
		case Axis::Pitch:
		case Axis::Roll:
		case Axis::Yaw:
		case Axis::WorldPitch:
		case Axis::WorldRoll:
		case Axis::WorldYaw:
			return Type::Rotational;
		}
	}

	static inline bool IsWorldAxis(Axis in){
		switch (in){
			case Axis::WorldPitch:
			case Axis::WorldRoll:
			case Axis::WorldYaw:
				return true;
			default:
				return false;
		}
	}
};

#ifdef glm_glm

namespace AxisVector {
	static const glm::vec3 X = glm::vec3(1.f, 0.f, 0.f);
	static const glm::vec3 Y = glm::vec3(0.f, 1.f, 0.f);
	static const glm::vec3 Z = glm::vec3(0.f, 0.f, 1.f);

	static const glm::vec3 Pitch = glm::vec3(1.f, 0.f, 0.f);
	static const glm::vec3 Roll = glm::vec3(0.f, 1.f, 0.f);
	static const glm::vec3 Yaw = glm::vec3(0.f, 0.f, 1.f);

	static const glm::vec3 Zero = glm::vec3(0.f, 0.f, 0.f);

	static const inline glm::vec3 FromAxis(Axis in){
		switch (in){
		case Axis::X:
			return AxisVector::X;
		case Axis::Y:
			return AxisVector::Y;
		case Axis::Z:
			return AxisVector::Z;
		case Axis::Pitch:
		case Axis::WorldPitch:
			return AxisVector::Pitch;
		case Axis::Roll:
		case Axis::WorldRoll:
			return AxisVector::Roll;
		case Axis::Yaw:
		case Axis::WorldYaw:
			return AxisVector::Yaw;
		case Axis::None:
		default:
			return AxisVector::Zero;
		}
	}
};

#endif