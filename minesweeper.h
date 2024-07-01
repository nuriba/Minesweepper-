#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QVector>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap> // For using images

// The Minesweeper class inherits from QWidget and represents the game board.
class Minesweeper : public QWidget {
    Q_OBJECT

public:
    // Constructor to initialize the game with a given number of rows, columns, and mines.
    explicit Minesweeper(QWidget *parent = nullptr, int rows = 10, int cols = 10, int mines = 10);
    // Destructor to clean up any allocated resources.
    ~Minesweeper();

    // Function to change the grid size and number of mines.
    void changeGridSize(int newRows, int newCols, int newMines);

private slots:
    // Slot to start a new game.
    void startGame();
    // Slot to reveal a cell when clicked.
    void revealCell();
    // Slot to flag a cell as a potential mine.
    void flagCell();
    // Slot to give a hint to the player.
    void giveHint();
    // Slot to update the scoreboard.
    void updateScoreboard();

private:
    // Function to count the number of mines around a given cell.
    int countMines(int row, int col);
    // Function to reveal all neighboring cells of a given cell.
    void revealNeighbors(int row, int col);
    // Function to check if the player has won the game.
    bool checkWin();
    // Function to handle the end of the game, either win or lose.
    void gameOver(bool win);

    int rows; // Number of rows in the grid.
    int cols; // Number of columns in the grid.
    int mines; // Number of mines in the grid.
    int flags; // Number of flags placed by the player.
    int score; // Player's score.
    QVector<QVector<int>> grid; // 2D vector to store the grid values.
    QVector<QVector<bool>> gridMine; // 2D vector to store mine locations.
    QVector<QVector<QPushButton*>> buttons; // 2D vector to store the button widgets.
    QVector<int> mineLocations; // Vector to store the mine locations.

    QGridLayout *gridLayout; // Layout to arrange the grid buttons.
    QPushButton *restartButton; // Button to restart the game.
    QLabel *scoreLabel; // Label to display the score.
    QPushButton *hintButton; // Button to get a hint.
    bool hintActive; // Flag to indicate if hint is active.
    QPushButton *hintCell; // Pointer to the cell that will be revealed as a hint.

    // QPixmap members for images used in the game.
    QPixmap hintPixmap;
    QPixmap minePixmap;
    QPixmap flagPixmap;
    QPixmap emptyPixmap;
    QVector<QPixmap> numberPixmaps; // Vector of pixmaps for number images.
};

#endif // MINESWEEPER_H
