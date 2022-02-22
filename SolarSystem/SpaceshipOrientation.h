#pragma once

// this is for adjusting the spaceship rotation while rotating the camera or moving sideways
// not implemented yet

#include <ctime>
#include <chrono>

class SpaceshipOrientation
{
public:
	// degrees / second
	double rotationSpeed = 5;
	double rotationAngleMax = 30;
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
		return abs(targetRotation - startRotation) / rotationSpeed;
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

	// roll - not used yet, but might be in the future

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
	void ComputeVerticalRotation(const std::chrono::system_clock::time_point& curTime);
	void ComputeHorizontalRotation(const std::chrono::system_clock::time_point& curTime);
	void ComputeRollRotation(const std::chrono::system_clock::time_point& curTime);
};

