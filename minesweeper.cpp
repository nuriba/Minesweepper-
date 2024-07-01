#include "minesweeper.h"

Minesweeper::Minesweeper(QWidget *parent, int rows, int cols, int mines)
    : QWidget(parent), rows(rows), cols(cols), mines(mines), score(0), hintActive(false), hintCell(nullptr) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Add a horizontal layout for the score label and buttons
    QHBoxLayout *topLayout = new QHBoxLayout();

    scoreLabel = new QLabel("Score: 0", this); // Initialize the scoreboard
    topLayout->addWidget(scoreLabel);

    // Add stretch to push the buttons to the right
    topLayout->addStretch();

    restartButton = new QPushButton("Restart", this);
    connect(restartButton, &QPushButton::clicked, this, &Minesweeper::startGame);
    topLayout->addWidget(restartButton);

    hintButton = new QPushButton("Hint", this); // Ensure this member is initialized
    connect(hintButton, &QPushButton::clicked, this, &Minesweeper::giveHint);
    topLayout->addWidget(hintButton);

    mainLayout->addLayout(topLayout);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0); // Set the spacing between cells to 0
    mainLayout->addLayout(gridLayout);

    // Load images from assets in the resources.qrc
    hintPixmap.load(":/assets/hint.png");
    minePixmap.load(":/assets/mine.png");
    flagPixmap.load(":/assets/flag.png");
    emptyPixmap.load(":/assets/empty.png");
    for (int i = 0; i <= 8; ++i) {
        QPixmap numberPixmap;
        numberPixmap.load(QString(":/assets/%0.png").arg(i));
        numberPixmaps.append(numberPixmap);
    }

    startGame(); // Initialize the game for the first time
}

Minesweeper::~Minesweeper() {}

void Minesweeper::startGame() {
    // Clear existing buttons
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget(); // delete the QPushButton
        delete item; // delete the QLayoutItem
    }

    // Resize and initialize grid and buttons
    grid.resize(rows);
    gridMine.resize(rows);
    buttons.resize(rows);
    for (int r = 0; r < rows; ++r) {
        grid[r].resize(cols);
        gridMine[r].resize(cols);
        buttons[r].resize(cols);
        grid[r].fill(0); // reset the grid cells to 0
        gridMine[r].fill(false);
    }
    mineLocations.clear();

    // Place mines
    QSet<int> mineSet;
    while (mineSet.size() < mines) {
        int randomIndex = QRandomGenerator::global()->bounded(rows * cols);
        mineSet.insert(randomIndex);
    }
    mineLocations = QVector<int>::fromList(mineSet.values());

    // Reset flags and score
    flags = 0;
    score = 0;
    updateScoreboard();

    // Initialize grid and buttons
    for (int idx : mineLocations) {
        int row = idx / cols;
        int col = idx % cols;
        grid[row][col] = -1;
    }
    // replaces the grids with the pngs
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] != -1) {
                grid[r][c] = countMines(r, c);
            }
            QPushButton *button = new QPushButton("", this);
            button->setFixedSize(15, 25); // Adjusted to match the PNG size
            button->setIcon(emptyPixmap);
            button->setIconSize(button->size());
            connect(button, &QPushButton::clicked, this, &Minesweeper::revealCell);
            button->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(button, &QPushButton::customContextMenuRequested, this, &Minesweeper::flagCell);
            gridLayout->addWidget(button, r, c);
            buttons[r][c] = button;
        }
    }
}
// counts the number of mines about a grid.
int Minesweeper::countMines(int row, int col) {
    int count = 0;
    for (int r = row - 1; r <= row + 1; ++r) {
        for (int c = col - 1; c <= col + 1; ++c) {
            if (r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c] == -1) {
                ++count;
            }
        }
    }
    return count;
}

// checks the grid after button pressed and makes the necessary operation.
void Minesweeper::revealCell() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int row = -1, col = -1;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (buttons[r][c] == button) {
                row = r;
                col = c;
                break;
            }
        }
        if (row != -1) break;
    }

    if (row == -1 || col == -1) return; // safety check

    if (button == hintCell) {
        hintCell->setStyleSheet("");
        hintActive = false;
        hintCell = nullptr;
    }

    if (grid[row][col] == -1) {
        button->setIcon(minePixmap);
        button->setIconSize(button->size());
        gameOver(false);
    } else {
        button->setIcon(numberPixmaps[grid[row][col]]);
        button->setIconSize(button->size());
        button->setEnabled(false);
        score++; // Increment the score when a cell is revealed
        updateScoreboard(); // Update the scoreboard
        if (grid[row][col] == 0) {
            revealNeighbors(row, col);
        }
        if (checkWin()) {
            gameOver(true);
        }
    }
}
// If the user press the left click sign the grid as flagged
void Minesweeper::flagCell() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    if (button == hintCell) {
        hintCell->setStyleSheet("");
        hintActive = false;
        hintCell = nullptr;
    }

    if (button->icon().cacheKey() == flagPixmap.cacheKey()) {
        button->setIcon(emptyPixmap);
        button->setIconSize(button->size());
        --flags;
    } else {
        button->setIcon(flagPixmap);
        button->setIconSize(button->size());
        ++flags;
    }
}


void Minesweeper::revealNeighbors(int row, int col) {
    for (int r = row - 1; r <= row + 1; ++r) {
        for (int c = col - 1; c <= col + 1; ++c) {
            if (r >= 0 && r < rows && c >= 0 && c < cols && buttons[r][c]->isEnabled()) {
                buttons[r][c]->click(); // This will call revealCell()
            }
        }
    }
}

// Checks whether the user win or not
bool Minesweeper::checkWin() {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] != -1 && buttons[r][c]->isEnabled()) {
                return false;
            }
        }
    }
    return true;
}

// Checks whether user open a mine or not to set the game win or lose
void Minesweeper::gameOver(bool win) {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == -1) {
                buttons[r][c]->setIcon(minePixmap);
                buttons[r][c]->setIconSize(buttons[r][c]->size());
            }
            buttons[r][c]->setEnabled(false);
        }
    }
    if (win) {
        QMessageBox::information(this, "Minesweeper", "You win!");
    } else {
        QMessageBox::information(this, "Minesweeper", "You lose!");
    }
}
//Gives hint the user
void Minesweeper::giveHint() {
    if (hintActive) {
        // Reveal the suggested cell
        if (hintCell && hintCell->isEnabled()) {
            hintCell->click();
        }
        hintActive = false;
        hintCell = nullptr;
    } else {
        // Iterate through all cells to find a safe cell
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (grid[r][c] > 0 && !buttons[r][c]->isEnabled()) {
                    int numMines = grid[r][c];
                    int numFlags = 0;
                    int numUnrevealed = 0;
                    QVector<QPair<int, int>> neighbors;  // Use QPair to store coordinates

                    // Check neighbors
                    for (int dr = -1; dr <= 1; ++dr) {
                        for (int dc = -1; dc <= 1; ++dc) {
                            int nr = r + dr;
                            int nc = c + dc;
                            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && (dr != 0 || dc != 0)) {
                                if (buttons[nr][nc]->isEnabled()) {
                                    numUnrevealed++;
                                    neighbors.append(qMakePair(nr, nc));  // Store the coordinates
                                }
                            }
                        }
                    }

                    // If the number of unrevealed cells plus the number of flags equals the number of mines,
                    // all remaining neighbors that are not flagged are safe
                    if (numUnrevealed == numMines) {
                        for (const QPair<int, int>& neighbor : neighbors) {
                            int nr = neighbor.first;
                            int nc = neighbor.second;
                            if (buttons[nr][nc]->text().isEmpty()) { // Ensure it's not flagged
                                gridMine[nr][nc] = true;
                            }
                        }
                    }
                }
            }
        }

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (grid[r][c] > 0 && !buttons[r][c]->isEnabled()) {
                    int numMines = grid[r][c];
                    int numDeterminedMines = 0;
                    int numUnrevealed = 0;
                    QVector<QPair<int, int>> neighbors;
                    QVector<QPair<int, int>> canBeHint;
                    QVector<QPushButton*> neighborsButton;

                    // Check neighbors
                    for (int dr = -1; dr <= 1; ++dr) {
                        for (int dc = -1; dc <= 1; ++dc) {
                            int nr = r + dr;
                            int nc = c + dc;
                            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && (dr != 0 || dc != 0)) {
                                if (buttons[nr][nc]->isEnabled()) {
                                    if (gridMine[nr][nc] == true) {
                                        numDeterminedMines++;
                                    } else {
                                        canBeHint.append(qMakePair(nr, nc));
                                        neighborsButton.append(buttons[nr][nc]);
                                    }
                                    numUnrevealed++;
                                    neighbors.append(qMakePair(nr, nc));
                                }
                            }
                        }
                    }

                    if (numDeterminedMines == numMines) {
                        for (QPushButton* neighbor : neighborsButton) {
                            if (neighbor->text().isEmpty()) { // Ensure it's not flagged
                                hintCell = neighbor;
                                hintCell-> setIcon(hintPixmap);
                                hintActive = true;
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}
// Uptades the score boards and print it the grid
void Minesweeper::updateScoreboard() {
    scoreLabel->setText("Score: " + QString::number(score));
}

// If the size of grid is changed,It change the size. Otherwise, the number of rows,columns and mines is 10 default.
void Minesweeper::changeGridSize(int newRows, int newCols, int newMines) {
    rows = newRows;
    cols = newCols;
    mines = newMines;
    startGame();
}
