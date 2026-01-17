#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

class Minesweeper {
private:
    static const int MAX_SIZE = 20;
    
    int rows;
    int cols;
    int mineCount;
    
    char field[MAX_SIZE][MAX_SIZE];
    char display[MAX_SIZE][MAX_SIZE];
    bool revealed[MAX_SIZE][MAX_SIZE];
    bool flagged[MAX_SIZE][MAX_SIZE];
    
    int revealedCount;
    bool gameOver;
    bool won;
    
public:
    Minesweeper(int r = 10, int c = 10, int mines = 10) 
        : rows(r), cols(c), mineCount(mines), revealedCount(0), 
          gameOver(false), won(false) {
        
        if (rows > MAX_SIZE) rows = MAX_SIZE;
        if (cols > MAX_SIZE) cols = MAX_SIZE;
        if (mineCount > rows * cols - 1) mineCount = rows * cols - 1;
        
        initializeField();
    }
    
    // Инициализация игрового поля
    void initializeField() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                field[i][j] = '0';
                display[i][j] = '#';
                revealed[i][j] = false;
                flagged[i][j] = false;
            }
        }
        
        placeMines();
        calculateNumbers();
    }
    
    void placeMines() {
        srand(time(0));
        int placedMines = 0;
        
        while (placedMines < mineCount) {
            int r = rand() % rows;
            int c = rand() % cols;
            
            if (field[r][c] != '*') {
                field[r][c] = '*';
                placedMines++;
            }
        }
    }
    
    void calculateNumbers() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (field[i][j] != '*') {
                    int count = countAdjacentMines(i, j);
                    field[i][j] = '0' + count;
                }
            }
        }
    }
    
    int countAdjacentMines(int row, int col) {
        int count = 0;
        
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                
                int newRow = row + dr;
                int newCol = col + dc;
                
                if (isValid(newRow, newCol) && field[newRow][newCol] == '*') {
                    count++;
                }
            }
        }
        
        return count;
    }
    
    bool isValid(int row, int col) {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }
    
    void revealCell(int row, int col) {
        if (!isValid(row, col) || revealed[row][col] || flagged[row][col]) {
            return;
        }
        
        revealed[row][col] = true;
        revealedCount++;
        display[row][col] = field[row][col];
        
        if (field[row][col] == '*') {
            gameOver = true;
            won = false;
            revealAllMines();
            return;
        }
        
        if (field[row][col] == '0') {
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) continue;
                    revealCell(row + dr, col + dc);
                }
            }
        }
        
        checkWin();
    }
    
    void toggleFlag(int row, int col) {
        if (!isValid(row, col) || revealed[row][col]) {
            return;
        }
        
        flagged[row][col] = !flagged[row][col];
        display[row][col] = flagged[row][col] ? 'F' : '#';
    }
    
    void revealAllMines() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (field[i][j] == '*') {
                    display[i][j] = '*';
                }
            }
        }
    }
    
    void checkWin() {
        int totalCells = rows * cols;
        if (revealedCount == totalCells - mineCount) {
            gameOver = true;
            won = true;
        }
    }
    
    void displayField(bool showMines = false) {
        cout << "\n   ";
        for (int j = 0; j < cols; j++) {
            cout << setw(3) << j;
        }
        cout << endl;
        
        cout << "   ";
        for (int j = 0; j < cols; j++) {
            cout << "───";
        }
        cout << endl;
        
        for (int i = 0; i < rows; i++) {
            cout << setw(2) << i << "│";
            
            for (int j = 0; j < cols; j++) {
                char cell;
                
                if (showMines) {
                    cell = field[i][j];
                } else {
                    cell = display[i][j];
                }
                
                if (cell == '*') {
                    cout << " \033[1;31m*\033[0m ";
                } else if (cell == 'F') {
                    cout << " \033[1;33mF\033[0m ";
                } else if (cell == '#') {
                    cout << " \033[1;37m#\033[0m ";
                } else if (cell == '0') {
                    cout << "   ";
                } else if (cell >= '1' && cell <= '8') {
                    cout << " \033[1;3" << (cell - '0') << "m" << cell << "\033[0m ";
                } else {
                    cout << " " << cell << " ";
                }
            }
            cout << "│" << endl;
        }
        
        cout << "   ";
        for (int j = 0; j < cols; j++) {
            cout << "───";
        }
        cout << endl;
        
        cout << "\nМин осталось: " << mineCount - countFlags() << endl;
    }
    
    // Подсчет установленных флагов
    int countFlags() {
        int count = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (flagged[i][j]) count++;
            }
        }
        return count;
    }
    
    void play() {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║            ИГРА 'САПЁР'               ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "Размер поля: " << rows << "x" << cols << endl;
        cout << "Количество мин: " << mineCount << endl;
        cout << "\nКоманды:" << endl;
        cout << "  O <строка> <столбец> - открыть клетку" << endl;
        cout << "  F <строка> <столбец> - поставить/убрать флаг" << endl;
        cout << "  Q - выход из игры" << endl;
        
        while (!gameOver) {
            displayField();
            
            char command;
            int row, col;
            
            cout << "\nВведите команду: ";
            cin >> command;
            
            command = toupper(command);
            
            if (command == 'Q') {
                cout << "Игра прервана." << endl;
                break;
            }
            
            if (command == 'O' || command == 'F') {
                cin >> row >> col;
                
                if (!isValid(row, col)) {
                    cout << "Неверные координаты! Попробуйте снова." << endl;
                    continue;
                }
                
                if (command == 'O') {
                    revealCell(row, col);
                } else if (command == 'F') {
                    toggleFlag(row, col);
                }
            } else {
                cout << "Неверная команда! Используйте O, F или Q." << endl;
            }
        }
        
        if (gameOver) {
            displayField(true);
            
            if (won) {
                cout << "\n╔════════════════════════════════════════╗" << endl;
                cout << "║          🎉 ПОЗДРАВЛЯЕМ! 🎉           ║" << endl;
                cout << "║         ВЫ ВЫИГРАЛИ!                 ║" << endl;
                cout << "╚════════════════════════════════════════╝" << endl;
            } else {
                cout << "\n╔════════════════════════════════════════╗" << endl;
                cout << "║           💥 БУМ! 💥                  ║" << endl;
                cout << "║         ВЫ ПРОИГРАЛИ!                ║" << endl;
                cout << "╚════════════════════════════════════════╝" << endl;
            }
        }
    }
    
    void debugMode() {
        cout << "\n=== РЕЖИМ ОТЛАДКИ (все мины видны) ===" << endl;
        displayField(true);
    }
};

void selectDifficulty(int& rows, int& cols, int& mines) {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║        ВЫБОР СЛОЖНОСТИ                ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "1. Новичок    (9x9,   10 мин)" << endl;
    cout << "2. Любитель   (16x16, 40 мин)" << endl;
    cout << "3. Профи      (16x30, 99 мин)" << endl;
    cout << "4. Своя сложность" << endl;
    cout << "───────────────────────────────────────" << endl;
    cout << "Выберите уровень: ";
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1:
            rows = cols = 9;
            mines = 10;
            break;
        case 2:
            rows = cols = 16;
            mines = 40;
            break;
        case 3:
            rows = 16;
            cols = 30;
            mines = 99;
            break;
        case 4:
            cout << "Введите количество строк (5-20): ";
            cin >> rows;
            cout << "Введите количество столбцов (5-20): ";
            cin >> cols;
            cout << "Введите количество мин: ";
            cin >> mines;
            
            if (rows < 5) rows = 5;
            if (rows > 20) rows = 20;
            if (cols < 5) cols = 5;
            if (cols > 20) cols = 20;
            if (mines < 1) mines = 1;
            if (mines >= rows * cols) mines = rows * cols - 1;
            break;
        default:
            rows = cols = 9;
            mines = 10;
            cout << "Неверный выбор. Установлен уровень 'Новичок'." << endl;
    }
}

void displayMainMenu() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║            ГЛАВНОЕ МЕНЮ               ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "1. Новая игра" << endl;
    cout << "2. Правила игры" << endl;
    cout << "0. Выход" << endl;
    cout << "───────────────────────────────────────" << endl;
    cout << "Выберите действие: ";
}

void showRules() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║           ПРАВИЛА ИГРЫ                ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "\nЦель: Открыть все клетки без мин." << endl;
    cout << "\nУсловные обозначения:" << endl;
    cout << "  # - Закрытая клетка" << endl;
    cout << "  F - Флаг (отметка возможной мины)" << endl;
    cout << "  * - Мина" << endl;
    cout << "  1-8 - Количество мин вокруг клетки" << endl;
    cout << "  (пусто) - Нет мин вокруг" << endl;
    cout << "\nКак играть:" << endl;
    cout << "  1. Открывайте клетки командой O" << endl;
    cout << "  2. Ставьте флаги на мины командой F" << endl;
    cout << "  3. Используйте числа для логики" << endl;
    cout << "  4. Откройте все клетки без мин для победы" << endl;
    cout << "\nСовет: Начинайте с углов!" << endl;
}

int main() {
    system("chcp 65001 > nul");
    
    int choice;
    
    do {
        displayMainMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                int rows, cols, mines;
                selectDifficulty(rows, cols, mines);
                
                Minesweeper game(rows, cols, mines);
                game.play();
                break;
            }
            case 2:
                showRules();
                break;
            case 0:
                cout << "\nСпасибо за игру! До встречи!" << endl;
                break;
            default:
                cout << "Неверный выбор! Попробуйте снова." << endl;
        }
        
    } while (choice != 0);
    
    return 0;
}