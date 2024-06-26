// Game of Life
// 
// Goals for AFTER completion --> don't touch until GOL works first
// More than one type of life --> look at examples
// Automatic updating
// Menus, controls, etc...


#include "color.h"

#include <GL/glew.h>

#include <SFML/Graphics.hpp>

#include <array>
#include <random>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

std::mt19937 gen(std::random_device{}());
constexpr float PI = 3.141592;
const sf::Color fillColor = Color::BLUEPRINT;

int genRandomInt(const int& min, const int& max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}

float genRandomFloat(const int& min, const int& max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}


// Subject to change and deletion --> window size determined by cells
// Char smaller than short (-1 byte)
constexpr char borderSize = 4;

constexpr char xPos = borderSize;
constexpr char yPos = borderSize;

constexpr int rows = 100;
constexpr int cols = 100;
constexpr char cellSpacing = borderSize;
constexpr char cellSize = 4 * borderSize;

constexpr char targetFPS = 60;
constexpr int initScreenWidth = 1920;
constexpr int initScreenHeight = 1080;

constexpr int width = initScreenWidth - (2 * borderSize);
constexpr int height = initScreenHeight - (2 * borderSize);


class Cell {
public:
	Cell() : state(false) {}

	virtual void updateCellState(const bool& st) {
		state = st;
	}

	void flipCellState() { // Ignore IDE, can't be made const
		state != state;
	}

	const bool& getCellState() const {
		return state;
	}


private:
	bool state;

};

class Grid {
public:

	const bool& getCellStateAt(const int& row, const int& col) {
		Cell& cell = grid[row][col];

	}

	void updateCellStateAt(const int& row, const int& col, const bool& st) {
		grid[row][col].updateCellState(st);
	}


private:

	std::vector<std::vector<Cell>> grid;

	int iterNum;


};



class World {
public:

	World() {

		// Init SFML
		window.create(sf::VideoMode(initScreenWidth, initScreenHeight), "Physics Box", sf::Style::Titlebar | sf::Style::Close);
		window.setKeyRepeatEnabled(false);
		window.setVerticalSyncEnabled(false); // Probably ignored by driver
		// window.setFramerateLimit(targetFPS);


		// Init background
		setupVertexBuffer(borderAndBGRect, xPos, yPos, width, height, Color::EIGENGRAU);
		

		// Init Font
		initFont();

		// Set framecounter position
		frameText.setPosition(borderSize, borderSize);
	}

	void setupVertexBuffer(sf::VertexBuffer& vertexBuffer, const int& xPos, const int& yPos, const int& width, const int& height, const sf::Color& color) {

		sf::Vertex vertices[6];

		// First triangle (top-left, top-right, bottom-right)
		vertices[0].position = sf::Vector2f(xPos, yPos);
		vertices[1].position = sf::Vector2f(xPos + width, yPos);
		vertices[2].position = sf::Vector2f(xPos + width, yPos + height);

		// Second triangle (top-left, bottom-right, bottom-left)
		vertices[3].position = sf::Vector2f(xPos, yPos);
		vertices[4].position = sf::Vector2f(xPos + width, yPos + height);
		vertices[5].position = sf::Vector2f(xPos, yPos + height);

		for (int i = 0; i < 6; ++i) {
			vertices[i].color = color;
		}

		// Create the vertex buffer
		vertexBuffer.create(6);
		vertexBuffer.setPrimitiveType(sf::Triangles);
		vertexBuffer.update(vertices);


	}


	void frameCounterDisplay(const int& frameTime, const int& avg) {
		frameText.setString("FrameTime (us): " + std::to_string(frameTime) + "\nAvg. FPS: " + std::to_string(avg));

		window.draw(frameText);
	}

	void mainLoop() {

		float frameTime = 0;
		float totalFrameTime = 0;
		int frameCount = 0;


		while (running) {

			handleEvents();
			updateLogic();
			renderAll();

			frameTime = clock.restart().asMicroseconds();
			totalFrameTime += frameTime;

			frameCounterDisplay(frameTime, frameCount / (totalFrameTime / 1000000));
			frameCount++;
			window.display();

		}
	}



private:

	sf::Text frameText;

	bool running = true;

	sf::VertexBuffer borderAndBGRect;

	sf::RenderWindow window;

	sf::Clock clock;

	sf::Font font;

	void initFont() {
		font.loadFromFile(".\\Montserrat-Regular.ttf");
		frameText.setCharacterSize(34);
		frameText.setFillColor(Color::WHITE);
		frameText.setFont(font);
	}


	void handleEvents() {
		sf::Event event;


		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				running = false;
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2i mousePos = sf::Mouse::getPosition(window);
					
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					
				}
				break;
			case sf::Event::MouseMoved:
				
				break;

				case sf::Event::KeyPressed:
					
					break;
				
			}
		}
	}


	

	void updateLogic() {

		
	}





	void renderWorld() {
		window.draw(borderAndBGRect);
	}

	


	void renderAll() {
		renderWorld();
	}

};


int main() {

	World world;

	world.mainLoop();

	return 0;
}