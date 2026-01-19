#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

using namespace std;
using namespace sf;

const int N = 9;
const int CELL_SIZE = 60;
int grid[N][N] = {
    {5, 3, 0, 0, 7, 0, 0, 0, 0},
    {6, 0, 0, 1, 9, 5, 0, 0, 0},
    {0, 9, 8, 0, 0, 0, 0, 6, 0},
    {8, 0, 0, 0, 6, 0, 0, 0, 3},
    {4, 0, 0, 8, 0, 3, 0, 0, 1},
    {7, 0, 0, 0, 2, 0, 0, 0, 6},
    {0, 6, 0, 0, 0, 0, 2, 8, 0},
    {0, 0, 0, 4, 1, 9, 0, 0, 5},
    {0, 0, 0, 0, 8, 0, 0, 7, 9}
};

Font font;

void drawGrid(RenderWindow &window, int activeRow, int activeCol, bool isBacktracking) {
    window.clear(Color::White);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            RectangleShape cell(Vector2f((float)CELL_SIZE, (float)CELL_SIZE));
            cell.setPosition({(float)j * CELL_SIZE, (float)i * CELL_SIZE});
            cell.setOutlineThickness(2);
            cell.setOutlineColor(Color::Black);

            if (i == activeRow && j == activeCol) {
                cell.setFillColor(isBacktracking ? Color::Red : Color::Green);
            } else {
                cell.setFillColor(Color::Transparent);
            }
            window.draw(cell);

            if (grid[i][j] != 0) {
                Text text(font);
                text.setString(to_string(grid[i][j]));
                text.setCharacterSize(30);
                text.setFillColor(Color::Black);
                text.setPosition({(float)j * CELL_SIZE + 20, (float)i * CELL_SIZE + 10});
                window.draw(text);
            }
        }
    }
    window.display();
}

bool isSafe(int row, int col, int num) {
    for (int x = 0; x < N; x++) if (grid[row][x] == num) return false;
    for (int x = 0; x < N; x++) if (grid[x][col] == num) return false;
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + startRow][j + startCol] == num) return false;
    return true;
}

bool solveVisual(RenderWindow &window) {
    int row, col;
    bool isEmpty = false;
    
    for (row = 0; row < N; row++) {
        for (col = 0; col < N; col++) {
            if (grid[row][col] == 0) {
                isEmpty = true;
                break;
            }
        }
        if (isEmpty) break;
    }

    if (!isEmpty) return true;

    for (int num = 1; num <= 9; num++) {
        if (isSafe(row, col, num)) {
            grid[row][col] = num;

            // Update visualization (Green for trying)
            drawGrid(window, row, col, false);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            
            while (const auto event = window.pollEvent()) {
                if (event->is<Event::Closed>()) window.close();
            }

            if (solveVisual(window)) return true;

            // Backtracking step (Red for error correction)
            grid[row][col] = 0;
            drawGrid(window, row, col, true);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    return false;
}

int main() {
    RenderWindow window(VideoMode({(unsigned int)(N * CELL_SIZE), (unsigned int)(N * CELL_SIZE)}), "Sudoku Visualizer");

    if (!font.openFromFile("arial.ttf")) {
        cout << "Error loading font!" << endl;
        return -1;
    }

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
            
            // Press SPACE to start solving
            if (const auto* keyPress = event->getIf<Event::KeyPressed>()) {
                if (keyPress->code == Keyboard::Key::Space) solveVisual(window);
            }
        }
        drawGrid(window, -1, -1, false);
    }
    return 0;
}
