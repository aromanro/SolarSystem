#include "stdafx.h"

#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries


#include <gtc\type_ptr.hpp>

#include "Options.h"
#include "SolarSystem.h"
#include "Camera.h"

#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace OpenGL {

	Camera::Camera()
		: rotateAngle(0), translateDist(0), eyePos(60., 220., 160.), up(0, 0, 1), lookAt(0,0,0)		
	{
		SetSpeeds(theApp.options.translationSpeed, theApp.options.rotationSpeed);
	}

	Camera::~Camera()
	{
	}

	void Camera::SetSpeeds(double translate, double angle)
	{
		translateDist = translate / 100. * translateDistStep;
		rotateAngle = angle / 100. * rotateAngleStep;
	}

	void Camera::MoveForward(double amount)
	{
		ShiftCameraPosition(getNormalizedForward()*amount);
	}


	void Camera::MoveBackward(double amount)
	{
		MoveForward(-amount);
	}


	void Camera::MoveUp(double amount)
	{
		ShiftCameraPosition(getNormalizedUp()*amount);
	}


	void Camera::MoveDown(double amount)
	{
		MoveUp(-amount);
	}


	void Camera::MoveLeft(double amount)
	{
		const Vector3D<double> left = getNormalizedUp() % getNormalizedForward();

		ShiftCameraPosition(left*amount);
	}


	void Camera::MoveRight(double amount)
	{
		MoveLeft(-amount);
	}


	void Camera::ShiftCameraPosition(const Vector3D<double>& shiftVec)
	{
		eyePos += shiftVec;
		lookAt += shiftVec;
	}


	void Camera::PitchUp(double amount)
	{
		PitchDown(-amount);
	}

	void Camera::PitchDown(double amount)
	{
		const Vector3D<double> left = getNormalizedUp() % getNormalizedForward();
		Vector3D<double> forward = lookAt - eyePos;

		up = up.RotateAround(left, amount);
		forward = forward.RotateAround(left, amount);

		lookAt = eyePos + forward;
	}

	void Camera::RollLeft(double amount)
	{
		RollRight(-amount);
	}

	void Camera::RollRight(double amount)
	{
		const Vector3D<double> forward = lookAt - eyePos;

		up = up.RotateAround(forward, amount);
	}

	void Camera::YawLeft(double amount)
	{
		YawRight(-amount);
	}

	void Camera::YawRight(double amount)
	{
		Vector3D<double> forward = lookAt - eyePos;

		const Vector3D<double> left = getNormalizedUp() % forward.Normalize();
		up = (forward % left).Normalize();

		forward = forward.RotateAround(up, -amount);
		lookAt = eyePos + forward;
	}


	Vector3D<double> Camera::getNormalizedForward() const
	{
		const Vector3D<double> forward = lookAt - eyePos;

		return forward.Normalize();
	}


	Vector3D<double> Camera::getNormalizedUp() const
	{
		const Vector3D<double> forward = getNormalizedForward();
		Vector3D<double> upVec(up);

		const double projection = upVec * forward;

		upVec -= projection * forward;

		return upVec.Normalize();
	}

	void Camera::Move(Movements movement, bool fromMouse)
	{
		switch (movement)
		{
		case pitchUp:
			PitchUp(rotateAngle);
			break;
		case moveUp:
			MoveUp(translateDist);
			break;
		case moveForward:
			MoveForward(translateDist * (fromMouse ? theApp.options.scrollSpeed / 100. : 1.));
			break;
		case pitchDown:
			PitchDown(rotateAngle);
			break;
		case moveDown:
			MoveDown(translateDist);
			break;
		case moveBackward:
			MoveBackward(translateDist * (fromMouse ? theApp.options.scrollSpeed / 100. : 1.));
			break;
		case yawLeft:
			YawLeft(rotateAngle);
			break;
		case rollLeft:
			RollLeft(rotateAngle);
			break;
		case moveLeft:
			MoveLeft(translateDist);
			break;
		case yawRight:
			YawRight(rotateAngle);
			break;
		case rollRight:
			RollRight(rotateAngle);
			break;
		case moveRight:
			MoveRight(translateDist);
			break;		
		case rotateTowards:
		case noMove:
			break;
		}
	}

}


void OpenGL::Camera::Tick()
{
	if (movements.size() > 0)
	{
		if (movements.front().movement == rotateTowards) RotateTowards(rotateAngle, movements.front().towardsVec);
		else Move(movements.front().movement, movements.front().fromMouse);
		
		--movements.front().ticks;
		if (movements.front().ticks <= 0)
			movements.pop_front();
	}
}


void OpenGL::Camera::ProgressiveMove(Movements movement, int nrticks, bool fromMouse)
{
	if (movements.size() < 10)
		movements.emplace_back(Movement(movement, nrticks, fromMouse));
}

void OpenGL::Camera::ProgressiveRotate(const Vector3D<double>& towards, int nrticks)
{
	if (movements.size() == 0)
	{
		movements.emplace_back(Movement(rotateTowards, nrticks));
		movements.back().towardsVec = towards;
	}
}


void OpenGL::Camera::RotateTowards(double angle, const Vector3D<double>& towards)
{
	Vector3D<double> forward = lookAt - eyePos;

	forward = forward.RotateTowards(towards,angle);

	lookAt = eyePos + forward;
	
	const Vector3D<double> left = getNormalizedUp() % forward.Normalize();
	up = (forward % left).Normalize();
}



