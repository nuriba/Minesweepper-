#include "minesweeper.h"
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

// The MainWindow class inherits from QWidget and represents the main application window.
class MainWindow : public QWidget {
    Q_OBJECT

public:
    // Constructor to initialize the main window.
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        // Create a vertical layout for the main window.
        QVBoxLayout *layout = new QVBoxLayout(this);

        // Create a Minesweeper widget and add it to the layout.
        minesweeper = new Minesweeper(this);
        layout->addWidget(minesweeper);

        // Set initial grid size and number of mines.
        int rows = 10;
        int cols = 10;
        int mines = 15;
        minesweeper->changeGridSize(rows, cols, mines);
    }

private:
    Minesweeper *minesweeper; // Pointer to the Minesweeper widget.
};

// Main function to run the application.
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create and show the main window.
    MainWindow window;
    window.show();

    // Execute the application event loop.
    return app.exec();
}

// Include this line if using Qt's meta-object compiler.
#include "main.moc"
