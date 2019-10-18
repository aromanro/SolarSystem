#pragma once

#include <glm.hpp>

#include <list>

#include "Vector3D.h"

#include <gtc\matrix_transform.hpp>

namespace OpenGL {

	class Camera
	{
	public:
		enum class Movements {
			noMove,
			moveForward,
			moveBackward,
			moveUp,
			moveDown,
			moveLeft,
			moveRight,
			pitchUp,
			pitchDown,
			rollLeft,
			rollRight,
			yawLeft,
			yawRight,
			rotateTowards
		};

	protected:
		double rotateAngle;
		double translateDist;

		struct Movement {
			Movements movement;
			Vector3D<double> towardsVec;
			int ticks;
			bool fromMouse;

			Movement(Movements move, int nrticks = 5, bool mouse = false)
				: movement(move), ticks(nrticks), fromMouse(mouse)
			{}
		};

		std::list<Movement> movements;

	public:

		Camera();
		~Camera();

		
		Vector3D<double> eyePos;
		Vector3D<double> lookAt;
		Vector3D<double> up;

		void MoveForward(double amount);
		void MoveBackward(double amount);

		void MoveUp(double amount);
		void MoveDown(double amount);
		void MoveLeft(double amount);
		void MoveRight(double amount);


		void PitchUp(double amount);
		void PitchDown(double amount);
		void RollLeft(double amount);
		void RollRight(double amount);
		void YawLeft(double amount);
		void YawRight(double amount);

		void ShiftCameraPosition(const Vector3D<double>& shiftVec);

		Vector3D<double> getNormalizedForward() const;
		Vector3D<double> getNormalizedUp() const;

		void Move(Movements movement, bool fromMouse = false);

		double GetRotateAngle() const { return rotateAngle; }
		double GetTranslate() const { return translateDist; }

	public:
		void Tick();
		void ProgressiveMove(Movements movement, int nrticks = 5, bool fromMouse = false);
		void ProgressiveRotate(const Vector3D<double>& towards, int nrticks);

		void RotateTowards(double angle, const Vector3D<double>& towards);


		inline glm::mat4 getMatrix() const {
			return glm::lookAt(glm::vec3(eyePos.X, eyePos.Y, eyePos.Z), glm::vec3(lookAt.X, lookAt.Y, lookAt.Z), glm::vec3(up.X, up.Y, up.Z));
		}

		inline operator glm::mat4() const { return getMatrix(); };
		void SetSpeeds(double translate, double angle);
	};


}
