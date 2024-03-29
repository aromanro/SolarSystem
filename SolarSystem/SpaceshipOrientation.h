#pragma once

// this is for adjusting the spaceship rotation while rotating the camera or moving sideways
// not implemented yet
#include <algorithm>
#include <ctime>
#include <chrono>

class SpaceshipOrientation
{
public:
	// degrees / second
	double rotationSpeed = 45;
	double rotationAngleMax = 45;
	double rotationAngleZMax = 30;
	double rotationAngleAccel = 10;
	double delayBeforeUndoingRotation = 0.5;

	// Z is perpendicular / screen
	// X is up/down
	// Y is left/right

	double startRotationX = 0;
	double startRotationY = 0;
	double startRotationZ = 0;

	double rotationX = 0;
	double rotationY = 0;
	double rotationZ = 0;

	double targetRotationX = 0;
	double targetRotationY = 0;
	double targetRotationZ = 0;

	std::chrono::time_point<std::chrono::system_clock> startRotationXtime;
	std::chrono::time_point<std::chrono::system_clock> startRotationYtime;
	std::chrono::time_point<std::chrono::system_clock> startRotationZtime;

	void Tick();


	// seconds

	double TimeToRotate(double startRotation, double targetRotation)
	{
		const double halfway = abs(targetRotation - startRotation) / 2.;

		if (halfway > rotationAngleAccel) return TimeToRotateLarge(startRotation, targetRotation);

		const double acceleration = rotationSpeed * rotationSpeed / (2. * rotationAngleAccel);

		return 2. * sqrt(2. * halfway / acceleration);
	}

	// pitch

	void RotateUp();
	void RotateDown();

	void RotateUpDown(double angle);

	bool RotatingUp() const
	{
		return targetRotationX > rotationX;
	}

	bool RotatingDown() const
	{
		return targetRotationX < rotationX;
	}

	bool RotatedVertically() const
	{
		return rotationX != 0;
	}

	bool RotatingVertically() const
	{
		return targetRotationX != rotationX;
	}

	// yaw

	void RotateLeft();
	void RotateRight();

	void RotateLeftRight(double angle);

	bool RotatingLeft() const
	{
		return targetRotationY < rotationY;
	}

	bool RotatingRight() const
	{
		return targetRotationY > rotationY;
	}

	bool RotatedHorizontally() const
	{
		return rotationY != 0;
	}

	bool RotatingHorizontally() const
	{
		return targetRotationY != rotationY;
	}

	// roll

	void RollLeft();
	void RollRight();

	void RollLeftRight(double angle);

	bool RollingLeft() const
	{
		return targetRotationZ < rotationZ;
	}

	bool RollingRight() const
	{
		return targetRotationZ > rotationZ;
	}

	bool Rolled() const
	{
		return rotationZ != 0;
	}

	bool Rolling() const
	{
		return targetRotationZ != rotationZ;
	}

	// general

	bool Rotating() const
	{
		return targetRotationX != rotationX || targetRotationY != rotationY || targetRotationZ != rotationZ;
	}

	bool Rotated() const
	{
		return rotationX != 0 || rotationY != 0 || rotationZ != 0;
	}

	void ComputeRotations();

private:
	double RotationAngle(double rotTime, double start, double target) const;
	double RotationAngleLarge(double rotTime, double start, double target) const;

	// this could be used directly, but for small angles the spaceship rotates too fast, accelerating to max
	// so it's used now only for large angles (halfway > rotationAngleAccel) where the max can be achieved and sustained for a period of time
	double TimeToRotateLarge(double startRotation, double targetRotation)
	{
		//return abs(targetRotation - startRotation) / rotationSpeed;		
		const double halfway = abs(targetRotation - startRotation) / 2.;
		const double angleAccel = std::min<double>(rotationAngleAccel, halfway);
		const double timeToAccelerate = 4 * angleAccel / rotationSpeed;

		if (angleAccel < halfway)
		{
			const double timeConstantVelocity = 2 * (halfway - angleAccel) / rotationSpeed;

			return timeToAccelerate + timeConstantVelocity;
		}

		return timeToAccelerate;
	}

	void ComputeVerticalRotation(const std::chrono::system_clock::time_point& curTime);
	void ComputeHorizontalRotation(const std::chrono::system_clock::time_point& curTime);
	void ComputeRollRotation(const std::chrono::system_clock::time_point& curTime);
};

