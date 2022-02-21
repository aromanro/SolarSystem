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

	void RotateUp();
	void RotateDown();
	void RotateLeft();
	void RotateRight();

	// seconds
	double TimeToRotate(double startRotation, double targetRotation)
	{
		return abs(targetRotation - startRotation) / rotationSpeed;
	}

	bool RotatingUp() const
	{
		return targetRotationX > rotationX;
	}

	bool RotatingDown() const
	{
		return targetRotationX < rotationX;
	}

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

	bool RotatedVertically() const
	{
		return rotationX != 0;
	}

	bool RotatingVertically() const
	{
		return targetRotationX != rotationX;
	}

	bool Rotating() const
	{
		return targetRotationX != rotationX || targetRotationY != rotationY;
	}

	bool Rotated() const
	{
		return rotationX != 0 || rotationY != 0;
	}

	void ComputeRotations();
};

