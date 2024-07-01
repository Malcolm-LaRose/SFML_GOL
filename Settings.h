#pragma once


#ifndef SETTINGS_H
#define SETTINGS_H

#include <random>

struct GoL_Settings {

	static GoL_Settings& getSettings() {
		static GoL_Settings instance;
		return instance;
	}

	static constexpr char borderSize = 0;

	static constexpr char xPos = borderSize;
	static constexpr char yPos = borderSize;

	static constexpr int cols = 640; // 730 --> x dir
	static constexpr int rows = 360; // 410 --> y dir
	static constexpr char cellSpacing = borderSize;
	static constexpr char cellSize = 5;
	static constexpr char cellDist = cellSize + cellSpacing;

	static constexpr char targetFPS = 60;
	static constexpr int initScreenWidth = cols * cellDist + borderSize;
	static constexpr int initScreenHeight = rows * cellDist + borderSize;

	static constexpr char stepsPerSec = 22;

private:

	GoL_Settings() {} // Ensure we don't delete the default constructor

};

// Random number generation

std::mt19937 gen(std::random_device{}());

int genRandomInt(const int& min, const int& max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}

float genRandomFloat(const int& min, const int& max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

bool genRandomBool(const float& pTrue) {
	std::bernoulli_distribution dist(pTrue);
	return dist(gen);
}


#endif