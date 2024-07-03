// Game of Life
// 
// Goals for AFTER completion --> don't touch until GOL works first, do roughly in order
// Framerate display optimization --> Okay for now, improve by updating at a lower freq
// Clean up/ refactor code to make more modular/extendable
// Change state to represent colors somehow --> USE CHAR, 0 is off, 1 is on, 2 can be unique, etc....
// GUI --> ImGUI, later
// Drawing tools (lines, etc.) 
// more optimization...
// Menus, controls, etc...
// Options
// More than one type of life --> look at examples


#include "color.h"
#include "Settings.h"

#include <GL/glew.h>

#include <SFML/Graphics.hpp>

#include <array>
#include <cmath>
// #include <iostream>
#include <vector>


const char digit_pairs[201] = {
  "00010203040506070809"
  "10111213141516171819"
  "20212223242526272829"
  "30313233343536373839"
  "40414243444546474849"
  "50515253545556575859"
  "60616263646566676869"
  "70717273747576777879"
  "80818283848586878889"
  "90919293949596979899"
};


// Algorithm provided by user TIMO on SO: https://stackoverflow.com/questions/4351371/c-performance-challenge-integer-to-stdstring-conversion

static const int BUFFER_SIZE = 11;

char buf[BUFFER_SIZE]; // My modification

std::string itostr(unsigned int val)
{

	char* it = (char*)&buf[BUFFER_SIZE - 2];

	int div = val / 100;
	while (div) {
		memcpy(it, &digit_pairs[2 * (val - div * 100)], 2);
		val = div;
		it -= 2;
		div = val / 100;
	}
	memcpy(it, &digit_pairs[2 * val], 2);
	if (val < 10)
		it++;

	return std::string((char*)it, (char*)&buf[BUFFER_SIZE] - (char*)it);
}




const int dist(const sf::Vector2i& pos1, const sf::Vector2i& pos2) {
	return std::sqrt(((pos2.x - pos1.x) * (pos2.x - pos1.x)) + ((pos2.y - pos1.y) * (pos2.y - pos1.y)));
}

GoL_Settings& gols = GoL_Settings::getSettings();



const sf::Color cellStateToColor(const bool& st) {
	if (st) return Color::PHSORNG;
	else if (!st) return Color::DRKGRY;
}


class Cell {
public:
	Cell() : state(false) {}

	void updateCellState(const bool& st) {
		state = st;
	}

	void flipCellState() {
		state = !state;
	}

	const bool& getCellState() const {
		return state;
	}


private:
	bool state; // CHANGE TO A CHAR, DETERMINE PROPERTIRES AS I GO

};

class Grid {
public:

	Grid() {
		grid.resize(gols.rows, std::vector<Cell>(gols.cols));
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

		if (row < 0 || row >= gols.rows || col < 0 || col >= gols.cols) return;

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

	int iterNum = 0;


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
		window.setMouseCursorVisible(false);
		// window.setFramerateLimit(targetFPS);


		// Init background
		setupVertexBuffer(borderAndBGRect, 0, 0, gols.initScreenWidth, gols.initScreenHeight, Color::EIGENGRAU);

		// Calculate cell positions
		if (gols.cellDist == 1) {
			cells.setPrimitiveType(sf::Points);
			cells.resize(gols.rows * gols.cols);
		}
		else {
			cells.setPrimitiveType(sf::Triangles);
			cells.resize(gols.rows * gols.cols * 6);
		}
		calcVertices();

		// Init Font
		initFont();

		// Set framecounter position
		frameText.setPosition(gols.borderSize, gols.borderSize);
	}

	~World() {
		borderAndBGRect.~VertexBuffer();
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
		frameText.setString("F Time (us): " + itostr(frameTime) + "\nAvg FPS: " + itostr(avg));

		window.draw(frameText);
	}

	void mainLoop() {

		float frameTime = 0;
		float totalFrameTime = 0;
		int frameCount = 0;

		const float& delay = 1.0f / gols.stepsPerSec;


		while (running) {

			frameTime = clock.restart().asMicroseconds();
			totalFrameTime += frameTime;
			timer += frameTime / 1000000.0f;

			handleEvents();
			if (timer >= delay) {
				if (!paused) {
					update();
				}
				timer -= delay;
			}
			renderAll();
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
	sf::VertexArray cells;

	GameOfLife game;
	Grid& grid = game.grid;

	bool running = true;
	bool paused = true; // Start paused
	float timer = 0; // For simtime calcs

	sf::Vector2i firstPos;
	sf::Vector2i secondPos;

	void initFont() {
		font.loadFromFile(".\\Montserrat-Regular.ttf");
		frameText.setCharacterSize(24);
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
			case sf::Event::MouseButtonPressed: {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				const int& col = mousePos.x / gols.cellDist;
				const int& row = mousePos.y / gols.cellDist;
				firstPos = sf::Vector2i(row, col);

				if (event.mouseButton.button == sf::Mouse::Left) {
				}
				break;
			}
			case sf::Event::MouseButtonReleased: {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				const int& col = mousePos.x / gols.cellDist;
				const int& row = mousePos.y / gols.cellDist;
				secondPos = sf::Vector2i(row, col);

				if (event.mouseButton.button == sf::Mouse::Left) {
					// DDATool(secondPos, firstPos);
					bresenhamTool(firstPos.x, firstPos.y, secondPos.x, secondPos.y); // Cleaner endpoints than DDA
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					plotCircle(firstPos, secondPos);
				}
				break;
			}
			case sf::Event::MouseMoved: {
				// Draw temporary shapes until mouse is released
				//sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				//const int& col = mousePos.x / gols.cellDist;
				//const int& row = mousePos.y / gols.cellDist;
				//secondPos = sf::Vector2i(row, col);

				//if (event.mouseButton.button == sf::Mouse::Left) {
				//	// DDATool(secondPos, firstPos);
				//	bresenhamTool(firstPos.x, firstPos.y, secondPos.x, secondPos.y); // Cleaner endpoints than DDA
				//}
				//else if (event.mouseButton.button == sf::Mouse::Right) {
				//	plotCircle(firstPos, secondPos);
				//}

				break;
			}
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Space) { // Single step
					update();
				}
				if (event.key.code == sf::Keyboard::P) { // Pause/unpause continuous stepping
					paused = !paused;
					if (!paused) {
						timer = 0.0f;
					}
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

	void DDATool(const sf::Vector2i& pos1, const sf::Vector2i& pos2) { // Digital differential analyzer
		float x1, x2, y1, y2;
		x1 = pos1.x;
		y1 = pos1.y;
		x2 = pos2.x;
		y2 = pos2.y;
		float dx = x2 - x1;
		float dy = y2 - y1;
		float step = 0.0f;
		float x, y;
		int i = 0;

		if (pos1 == pos2) {
			grid.flipCellStateAt(pos1.x, pos1.y);
			return;
		}

		if (std::abs(dx) >= std::abs(dy)) {
			step = std::abs(dx);
		}
		else {
			step = std::abs(dy);
		}

		dx = dx / step;
		dy = dy / step;
		x = x1;
		y = y1;
		i = 0;

		while (i <= step) {
			grid.flipCellStateAt(x, y);
			x += dx;
			y += dy;
			i++;
		}


	}

	void bresenhamTool(int x0, int y0, int x1, int y1)
	{
		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy, e2; /* error value e_xy */

		while (true) {  /* loop */
			grid.flipCellStateAt(x0, y0);
			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
		}
	}

	void plotCircle(const sf::Vector2i& pos1, const sf::Vector2i& pos2)
	{
		int xm, ym;

		xm = pos2.x;
		ym = pos2.y;

		int r = dist(pos1, pos2);

		int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
		do {
			grid.flipCellStateAt(xm - x, ym + y); /*   I. Quadrant */
			grid.flipCellStateAt(xm - y, ym - x); /*  II. Quadrant */
			grid.flipCellStateAt(xm + x, ym - y); /* III. Quadrant */
			grid.flipCellStateAt(xm + y, ym + x); /*  IV. Quadrant */
			r = err;
			if (r <= y) err += ++y * 2 + 1;           /* e_xy+e_y < 0 */
			if (r > x || err > y) err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
		} while (x < 0);
	}

	void update() {
		game.gameOfLife();
	}


	void renderWorld() {
		window.draw(borderAndBGRect);
	}

	void calcVertices() {
		if (gols.cellDist == 1) {

			int i = 0;
			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {
					const int& x = col * (gols.cellDist) + gols.borderSize;
					const int& y = row * (gols.cellDist) + gols.borderSize;

					const bool& cellState = grid.getCellStateAt(row, col);

					sf::Color color = cellState ? Color::PHSORNG : Color::DRKGRY;

					cells[i].position = sf::Vector2f(x, y);
					cells[i].color = color;
					++i;
				}
			}
		}
		else {
			int i = 0;
			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {
					// Position of upper left corner of cell
					const int& x = col * (gols.cellDist) + gols.borderSize;
					const int& y = row * (gols.cellDist) + gols.borderSize;

					const sf::Vector2i& pos = sf::Mouse::getPosition(window);

					const bool& cellState = grid.getCellStateAt(row, col);

					
					sf::Color color = cellStateToColor(cellState);


					// First triangle (top-left, top-right, bottom-right)
					cells[i].position = sf::Vector2f(x, y);
					cells[i + 1].position = sf::Vector2f(x + gols.cellDist, y);
					cells[i + 2].position = sf::Vector2f(x + gols.cellDist, y + gols.cellDist);

					// Second triangle (top-left, bottom-right, bottom-left)
					cells[i + 3].position = sf::Vector2f(x, y);
					cells[i + 4].position = sf::Vector2f(x + gols.cellDist, y + gols.cellDist);
					cells[i + 5].position = sf::Vector2f(x, y + gols.cellDist);

					// Set the color of the vertices
					for (int j = 0; j < 6; ++j) {
						cells[i + j].color = color; // Change this to the desired color
					}

					i += 6; // Move to the next set of vertices

				}
			}
		}
	}

	void vertexRenderGrid() {

		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		const int& mouseCol = mousePos.x / gols.cellDist;
		const int& mouseRow = mousePos.y / gols.cellDist;

		if (gols.cellDist == 1) {

			int i = 0;
			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {

					const bool& cellState = grid.getCellStateAt(row, col);

					sf::Color color = cellStateToColor(cellState);

					if ((row == mouseRow) && (col == mouseCol)) {
						color = Color::CYAN;
					}


					cells[i].color = color;
					++i;
				}
			}

			window.draw(cells);
		}
		else {
			int i = 0;
			for (int row = 0; row < gols.rows; ++row) {
				for (int col = 0; col < gols.cols; ++col) {


					const bool& cellState = grid.getCellStateAt(row, col);

					sf::Color color = cellStateToColor(cellState);

					if ((row == mouseRow) && (col == mouseCol)) {
						color = Color::CYAN;
					}

					// Set the color of the vertices
					for (int j = 0; j < 6; ++j) {
						cells[i + j].color = color; // Change this to the desired color
					}

					i += 6; // Move to the next set of vertices
					
				}
			}
			window.draw(cells);
		}
	}


	void renderAll() {
		renderWorld();
		vertexRenderGrid();
	}

};


int main() {

	World world;

	world.mainLoop();

	return 0;
}