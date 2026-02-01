#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTime>
#include <QTimer>
#include <vector>
#include <algorithm>
#include <random>

class GameBoard {
private:
    std::vector<int> board;
    int size;
    int emptyPos;
    int moves;
    
public:
    GameBoard(int gridSize = 4) : size(gridSize), moves(0) {
        board.resize(size * size);
        reset();
    }
    
    void reset() {
        moves = 0;
        for (int i = 0; i < size * size; i++) {
            board[i] = i;
        }
        emptyPos = size * size - 1;
    }
    
    void shuffle() {
        std::random_device rd;
        std::mt19937 gen(rd());
        
        for (int i = 0; i < 1000; i++) {
            std::vector<int> validMoves = getValidMoves();
            if (!validMoves.empty()) {
                std::uniform_int_distribution<> dis(0, validMoves.size() - 1);
                int movePos = validMoves[dis(gen)];
                swap(emptyPos, movePos);
                emptyPos = movePos;
            }
        }
        moves = 0;
    }
    
    bool canMove(int pos) {
        int emptyRow = emptyPos / size;
        int emptyCol = emptyPos % size;
        int row = pos / size;
        int col = pos % size;
        
        if (row == emptyRow && std::abs(col - emptyCol) == 1) return true;
        if (col == emptyCol && std::abs(row - emptyRow) == 1) return true;
        return false;
    }
    
    bool move(int pos) {
        if (canMove(pos)) {
            swap(pos, emptyPos);
            emptyPos = pos;
            moves++;
            return true;
        }
        return false;
    }
    
    bool isSolved() {
        for (int i = 0; i < size * size - 1; i++) {
            if (board[i] != i) return false;
        }
        return true;
    }
    
    int getValue(int pos) const {
        return board[pos];
    }
    
    int getSize() const {
        return size;
    }
    
    int getMoves() const {
        return moves;
    }
    
    int getEmptyPos() const {
        return emptyPos;
    }
    
private:
    void swap(int pos1, int pos2) {
        std::swap(board[pos1], board[pos2]);
    }
    
    std::vector<int> getValidMoves() {
        std::vector<int> moves;
        int row = emptyPos / size;
        int col = emptyPos % size;
        
        if (row > 0) moves.push_back((row - 1) * size + col);
        if (row < size - 1) moves.push_back((row + 1) * size + col);
        if (col > 0) moves.push_back(row * size + (col - 1));
        if (col < size - 1) moves.push_back(row * size + (col + 1));
        
        return moves;
    }
};

class FifteenPuzzle : public QWidget {
    Q_OBJECT
    
private:
    GameBoard* game;
    std::vector<QPushButton*> buttons;
    QLabel* movesLabel;
    QLabel* timeLabel;
    QTimer* timer;
    int elapsedSeconds;
    QMenuBar* menuBar;
    
public:
    FifteenPuzzle(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("Игра Пятнашки");
        setMinimumSize(500, 600);
        
        game = new GameBoard(4);
        elapsedSeconds = 0;
        
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &FifteenPuzzle::updateTime);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        
        createMenuBar();
        mainLayout->setMenuBar(menuBar);
        
        QWidget* centralWidget = new QWidget();
        QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
        centralLayout->setContentsMargins(10, 10, 10, 10);
        
        QLabel* titleLabel = new QLabel("<h2>Игра Пятнашки</h2>");
        titleLabel->setAlignment(Qt::AlignCenter);
        centralLayout->addWidget(titleLabel);
        
        QWidget* infoWidget = new QWidget();
        QHBoxLayout* infoLayout = new QHBoxLayout(infoWidget);
        
        movesLabel = new QLabel("Ходов: 0");
        movesLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");
        infoLayout->addWidget(movesLabel);
        
        infoLayout->addStretch();
        
        timeLabel = new QLabel("Время: 00:00");
        timeLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; }");
        infoLayout->addWidget(timeLabel);
        
        centralLayout->addWidget(infoWidget);
        centralLayout->addSpacing(10);
        
        QGridLayout* gridLayout = new QGridLayout();
        gridLayout->setSpacing(5);
        
        int size = game->getSize();
        buttons.resize(size * size);
        
        for (int i = 0; i < size * size; i++) {
            int row = i / size;
            int col = i % size;
            
            QPushButton* btn = new QPushButton();
            btn->setMinimumSize(100, 100);
            btn->setMaximumSize(100, 100);
            btn->setProperty("position", i);
            
            QString styleSheet = 
                "QPushButton {"
                "   background-color: #4CAF50;"
                "   color: white;"
                "   border: 2px solid #45a049;"
                "   border-radius: 8px;"
                "   font-size: 32px;"
                "   font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "   background-color: #45a049;"
                "}"
                "QPushButton:pressed {"
                "   background-color: #3d8b40;"
                "}";
            
            btn->setStyleSheet(styleSheet);
            
            connect(btn, &QPushButton::clicked, this, &FifteenPuzzle::onTileClicked);
            
            buttons[i] = btn;
            gridLayout->addWidget(btn, row, col);
        }
        
        centralLayout->addLayout(gridLayout);
        centralLayout->addStretch();
        
        QPushButton* newGameButton = new QPushButton("Новая игра");
        newGameButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #2196F3;"
            "   color: white;"
            "   padding: 10px;"
            "   border-radius: 5px;"
            "   font-size: 14px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #0b7dda;"
            "}"
        );
        connect(newGameButton, &QPushButton::clicked, this, &FifteenPuzzle::newGame);
        centralLayout->addWidget(newGameButton);
        
        mainLayout->addWidget(centralWidget);
        
        updateBoard();
        newGame();
    }
    
    ~FifteenPuzzle() {
        delete game;
    }
    
private:
    void createMenuBar() {
        menuBar = new QMenuBar(this);
        
        QMenu* gameMenu = menuBar->addMenu("Игра");
        
        QAction* newGameAction = new QAction("Новая игра", this);
        newGameAction->setShortcut(QKeySequence("Ctrl+N"));
        connect(newGameAction, &QAction::triggered, this, &FifteenPuzzle::newGame);
        gameMenu->addAction(newGameAction);
        
        gameMenu->addSeparator();
        
        QAction* exitAction = new QAction("Выход", this);
        exitAction->setShortcut(QKeySequence("Ctrl+Q"));
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        gameMenu->addAction(exitAction);
        
        QMenu* helpMenu = menuBar->addMenu("Справка");
        
        QAction* aboutAction = new QAction("О программе", this);
        connect(aboutAction, &QAction::triggered, this, &FifteenPuzzle::showAbout);
        helpMenu->addAction(aboutAction);
    }
    
    void updateBoard() {
        int size = game->getSize();
        
        for (int i = 0; i < size * size; i++) {
            int value = game->getValue(i);
            
            if (value == 0) {
                buttons[i]->setText("");
                buttons[i]->setStyleSheet(
                    "QPushButton {"
                    "   background-color: #e0e0e0;"
                    "   border: 2px solid #cccccc;"
                    "   border-radius: 8px;"
                    "}"
                );
                buttons[i]->setEnabled(false);
            } else {
                buttons[i]->setText(QString::number(value));
                buttons[i]->setEnabled(true);
                
                QString color = (value <= 8) ? "#4CAF50" : "#FF9800";
                QString hoverColor = (value <= 8) ? "#45a049" : "#e68900";
                
                buttons[i]->setStyleSheet(
                    QString("QPushButton {"
                    "   background-color: %1;"
                    "   color: white;"
                    "   border: 2px solid %2;"
                    "   border-radius: 8px;"
                    "   font-size: 32px;"
                    "   font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "   background-color: %2;"
                    "}"
                    "QPushButton:pressed {"
                    "   background-color: %2;"
                    "   border: 3px solid #333;"
                    "}").arg(color).arg(hoverColor)
                );
            }
        }
        
        movesLabel->setText(QString("Ходов: %1").arg(game->getMoves()));
    }
    
    void updateTime() {
        elapsedSeconds++;
        int minutes = elapsedSeconds / 60;
        int seconds = elapsedSeconds % 60;
        timeLabel->setText(QString("Время: %1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0')));
    }
    
private slots:
    void onTileClicked() {
        QPushButton* btn = qobject_cast<QPushButton*>(sender());
        if (!btn) return;
        
        int pos = btn->property("position").toInt();
        
        if (game->move(pos)) {
            updateBoard();
            
            if (game->isSolved()) {
                timer->stop();
                
                int minutes = elapsedSeconds / 60;
                int seconds = elapsedSeconds % 60;
                
                QMessageBox::information(this, "Победа!", 
                    QString("Поздравляем! Вы решили головоломку!\n\n"
                           "Количество ходов: %1\n"
                           "Время: %2:%3")
                    .arg(game->getMoves())
                    .arg(minutes, 2, 10, QChar('0'))
                    .arg(seconds, 2, 10, QChar('0')));
                
                newGame();
            }
        }
    }
    
    void newGame() {
        game->reset();
        game->shuffle();
        elapsedSeconds = 0;
        timeLabel->setText("Время: 00:00");
        updateBoard();
        timer->start(1000);
    }
    
    void showAbout() {
        QMessageBox::about(this, "О программе",
            "<h3>Игра Пятнашки</h3>"
            "<p>Классическая головоломка 15-Puzzle</p>"
            "<p><b>Цель игры:</b> расположить числа по порядку от 1 до 15</p>"
            "<p><b>Управление:</b> кликайте на плитки рядом с пустым местом</p>"
            "<p><b>Меню:</b> Ctrl+N - новая игра, Ctrl+Q - выход</p>");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    FifteenPuzzle window;
    window.show();
    
    return app.exec();
}

#include "lvl3proj3.moc"