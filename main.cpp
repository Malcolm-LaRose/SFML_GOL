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
constexpr char borderSize = 3;

constexpr char xPos = borderSize;
constexpr char yPos = borderSize;

constexpr int rows = 100;
constexpr int cols = 100;
constexpr char cellSpacing = borderSize;
constexpr char cellSize = 15;
constexpr char cellDist = cellSize + cellSpacing;

sf::RectangleShape rectangle;

constexpr char targetFPS = 60;
constexpr int initScreenWidth = rows * cellDist + borderSize;
constexpr int initScreenHeight = rows * cellDist + borderSize;



class Cell {
public:
	Cell() : state(false) {}

	virtual void updateCellState(const bool& st) {
		state = st;
	}

	void flipCellState() {
		state = !state;
	}

	const bool& getCellState() const {
		return state;
	}


private:
	bool state;

};

class Grid {
public:

	Grid() { // Don't need any special constructors, just change settings
		grid.resize(rows, std::vector<Cell>(cols));
		rectangle.setSize(sf::Vector2f(cellSize, cellSize));
	}

	const std::vector<std::vector<Cell>> getGrid() const {
		return grid;
	}

	const bool& getCellStateAt(const int& row, const int& col) const {
		const Cell& cell = grid[row][col];
		return cell.getCellState();
	}

	void updateCellStateAt(const int& row, const int& col, const bool& st) {
		grid[row][col].updateCellState(st);
	}

	void flipCellStateAt(const int& row, const int& col) {
		grid[row][col].flipCellState();
	}

	void resetGrid() {
		for (int row = 0; row < rows; row++) {
			for (int col = 0; col < cols; col++) {
				updateCellStateAt(row, col, false);
			}
		}
	}

	void incIterNum() {
		++iterNum;
	}


private:

	std::vector<std::vector<Cell>> grid;

	int iterNum = 0; // No need for special init here, all initial time steps can be zero


};

class GameOfLife {
public:
	GameOfLife() {}

	void gameOfLife() {
		Grid updatedGrid;
		bool gridChanged = false;

		for (int row = 0; row < rows; row++) { // Make sure not to check edges
			for (int col = 0; col < cols; col++) {
				int numAlive = checkMooreNeighborhoodFor(row, col, true);

				const bool& currentCellState = grid.getCellStateAt(row, col);

				// Actual GoL
				if (currentCellState) {
					if (numAlive < 2 || numAlive > 3) {
						updatedGrid.updateCellStateAt(row, col, false);
					if (!gridChanged) gridChanged = true;
					}
					else {
						updatedGrid.updateCellStateAt(row, col, true);
					}
				}
				else {
					if (numAlive == 3) {
						updatedGrid.updateCellStateAt(row, col, true);
						if (!gridChanged) gridChanged = true;
					}
				}


			}
		}

		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < cols; ++col) {
				bool nextState = updatedGrid.getCellStateAt(row, col);
				grid.updateCellStateAt(row, col, nextState);
			}
		}
		grid.incIterNum();

	}

	Grid grid;

private:

	const bool isInBounds(const int& row, const int& col) {
		if (row < 0 || col < 0) {
			return false;
		}
		if (row >= rows || col >= cols) {
			return false;
		}

		else {
			return true;
		}
	}

	const int& checkMooreNeighborhoodFor(const int& row, const int& col, bool st) {
		int count = 0;
		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				if (i == 0 && j == 0) continue;
				if (isInBounds(row + i, col + j) && (grid.getCellStateAt(row + i, col + j) == st)) {
					count++;
				}
			}
		}

		return count;
	}

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
		setupVertexBuffer(borderAndBGRect, 0, 0, initScreenWidth, initScreenHeight, Color::EIGENGRAU);
		

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

	sf::RenderWindow window;
	sf::Clock clock;
	sf::Font font;
	sf::Text frameText;

	sf::VertexBuffer borderAndBGRect; 

	GameOfLife game;
	Grid& grid = game.grid;

	bool running = true;

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
					const int& col = mousePos.x / cellDist;
					const int& row = mousePos.y / cellDist;

					grid.flipCellStateAt(row, col);

				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					
				}
				break;
			case sf::Event::MouseMoved:
				
				break;

				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Space) {
						game.gameOfLife();
					}
					break;
				
			}
		}
	}


	

	void updateLogic() {

		
	}


	void renderWorld() {
		window.draw(borderAndBGRect);
	}

	void renderGrid() {

		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < cols; ++col) {
				const bool& cellState = grid.getCellStateAt(row, col);

				sf::Color color;
				if (cellState) {
					color = Color::PHSORNG;
				}
				else {
					color = Color::DRKGRY;
				}

				// Position of upper left corner on screen
				const int x = col * (cellDist)+borderSize;
				const int y = row * (cellDist)+borderSize;

				rectangle.setPosition(x, y);
				rectangle.setFillColor(color);
				window.draw(rectangle);
			}
		}
	}


	void renderAll() {
		renderWorld();
		renderGrid();
	}

};


int main() {

	World world;

	world.mainLoop();

	return 0;
}