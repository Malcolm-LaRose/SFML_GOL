#pragma once


#ifndef SETTINGS_H
#define SETTINGS_H

struct GoL_Settings {

	static GoL_Settings& getSettings() {
		static GoL_Settings instance;
		return instance;
	}

	static constexpr char borderSize = 3;

	static constexpr char xPos = borderSize;
	static constexpr char yPos = borderSize;

	static constexpr int rows = 100;
	static constexpr int cols = 100;
	static constexpr char cellSpacing = borderSize;
	static constexpr char cellSize = 15;
	static constexpr char cellDist = cellSize + cellSpacing;

	sf::RectangleShape rectangle;

	static constexpr char targetFPS = 60;
	static constexpr int initScreenWidth = rows * cellDist + borderSize;
	static constexpr int initScreenHeight = rows * cellDist + borderSize;

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


#endif