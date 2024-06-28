// Game of Life
// 
// Goals for AFTER completion --> don't touch until GOL works first, do roughly in order
// Automatic updating
// Menus, controls, etc...
// Options
// More than one type of life --> look at examples


#include "color.h"
#include "Settings.h"

#include <GL/glew.h>

#include <SFML/Graphics.hpp>

#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>


GoL_Settings& gols = GoL_Settings::getSettings();



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
		grid.resize(gols.rows, std::vector<Cell>(gols.cols));
		gols.rectangle.setSize(sf::Vector2f(gols.cellSize, gols.cellSize));
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
		for (int row = 0; row < gols.rows; row++) {
			for (int col = 0; col < gols.cols; col++) {
				updateCellStateAt(row, col, false);
				iterNum = 0;
			}
		}
	}

	void randomizeGrid() {
		for (int row = 0; row < gols.rows; row++) {
			for (int col = 0; col < gols.cols; col++) {
				const bool& randBool = genRandomBool(0.5f);
				updateCellStateAt(row, col, randBool);
				iterNum = 0;
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

		for (int row = 0; row < gols.rows; row++) { // Make sure not to check edges
			for (int col = 0; col < gols.cols; col++) {
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

		for (int row = 0; row < gols.rows; ++row) {
			for (int col = 0; col < gols.cols; ++col) {
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
		if (row >= gols.rows || col >= gols.cols) {
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
				if (!isInBounds(row + i, col + j)) {
					continue;
				}
				if ((grid.getCellStateAt(row + i, col + j) == st)) {
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
		window.create(sf::VideoMode(gols.initScreenWidth, gols.initScreenHeight), "Physics Box", sf::Style::Titlebar | sf::Style::Close);
		window.setKeyRepeatEnabled(false);
		window.setVerticalSyncEnabled(false); // Probably ignored by driver
		// window.setFramerateLimit(targetFPS);


		// Init background
		setupVertexBuffer(borderAndBGRect, 0, 0, gols.initScreenWidth, gols.initScreenHeight, Color::EIGENGRAU);
		

		// Init Font
		initFont();

		// Set framecounter position
		frameText.setPosition(gols.borderSize, gols.borderSize);
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
					const int& col = mousePos.x / gols.cellDist;
					const int& row = mousePos.y / gols.cellDist;

					grid.flipCellStateAt(row, col);

				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {

				}
				break;

			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Space) {
					game.gameOfLife();
				}
				if (event.key.code == sf::Keyboard::R) {
					game.grid.resetGrid(); // This is why we wanted grid to be public (private seems overused!)
				}
				if (event.key.code == sf::Keyboard::Z) {
					game.grid.randomizeGrid(); 
				}
				break;
			}
		}
	}


	void renderWorld() {
		window.draw(borderAndBGRect);
	}

	void renderGrid() {

		for (int row = 0; row < gols.rows; ++row) {
			for (int col = 0; col < gols.cols; ++col) {
				const bool& cellState = grid.getCellStateAt(row, col);

				sf::Color color;
				if (cellState) {
					color = Color::PHSORNG;
				}
				else {
					color = Color::DRKGRY;
				}

				// Position of upper left corner on screen
				const int x = col * (gols.cellDist) + gols.borderSize;
				const int y = row * (gols.cellDist) + gols.borderSize;

				gols.rectangle.setPosition(x, y);
				gols.rectangle.setFillColor(color);
				window.draw(gols.rectangle);
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