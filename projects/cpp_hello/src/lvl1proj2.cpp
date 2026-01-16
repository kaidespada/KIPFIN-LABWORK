#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iomanip>

using namespace std;

struct Player {
    string name;
    int attempts;
    string date;
};

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    string date = to_string(ltm->tm_mday) + "." + 
                  to_string(1 + ltm->tm_mon) + "." + 
                  to_string(1900 + ltm->tm_year);
    return date;
}

string getAttemptsWord(int num) {
    int lastDigit = num % 10;
    int lastTwoDigits = num % 100;
    
    if (lastTwoDigits >= 11 && lastTwoDigits <= 19) return "попыток";
    if (lastDigit == 1) return "попытку";
    if (lastDigit >= 2 && lastDigit <= 4) return "попытки";
    return "попыток";
}

vector<Player> loadLeaderboard() {
    vector<Player> leaderboard;
    ifstream file("leaderboard.txt");
    
    if (file.is_open()) {
        Player p;
        while (file >> p.name >> p.attempts >> p.date) {
            leaderboard.push_back(p);
        }
        file.close();
    }
    
    return leaderboard;
}

void saveLeaderboard(const vector<Player>& leaderboard) {
    ofstream file("leaderboard.txt");
    
    if (file.is_open()) {
        for (const auto& player : leaderboard) {
            file << player.name << " " << player.attempts << " " << player.date << endl;
        }
        file.close();
    }
}

void addToLeaderboard(vector<Player>& leaderboard, const string& name, int attempts) {
    Player newPlayer;
    newPlayer.name = (name.empty() || name == " ") ? "Аноним" : name;
    newPlayer.attempts = attempts;
    newPlayer.date = getCurrentDate();
    
    leaderboard.push_back(newPlayer);
    
    sort(leaderboard.begin(), leaderboard.end(), 
         [](const Player& a, const Player& b) { return a.attempts < b.attempts; });
    
    if (leaderboard.size() > 10) {
        leaderboard.resize(10);
    }
    
    saveLeaderboard(leaderboard);
}

void displayLeaderboard(const vector<Player>& leaderboard) {
    if (leaderboard.empty()) {
        cout << "\n📊 Таблица рекордов пока пуста.\n";
        return;
    }
    
    cout << "\n╔════════════════════════════════════════════════╗\n";
    cout << "║           🏆 ТАБЛИЦА РЕКОРДОВ 🏆             ║\n";
    cout << "╠════════════════════════════════════════════════╣\n";
    cout << "║  №  │   Имя      │  Попытки  │    Дата       ║\n";
    cout << "╠════════════════════════════════════════════════╣\n";
    
    for (size_t i = 0; i < leaderboard.size(); ++i) {
        cout << "║  " << setw(2) << (i + 1) << " │ " 
             << setw(10) << left << leaderboard[i].name << " │    " 
             << setw(3) << right << leaderboard[i].attempts << "    │  " 
             << setw(10) << left << leaderboard[i].date << "  ║\n";
    }
    
    cout << "╚════════════════════════════════════════════════╝\n";
}

void playBasicGame(int minRange, int maxRange) {
    int targetNumber = rand() % (maxRange - minRange + 1) + minRange;
    int guess;
    int attempts = 0;
    
    cout << "\n🎲 Я загадал число от " << minRange << " до " << maxRange << ". Попробуй угадать!\n\n";
    
    while (true) {
        cout << "Введите число: ";
        cin >> guess;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "❌ Некорректный ввод! Введите число.\n";
            continue;
        }
        
        attempts++;
        
        if (guess == targetNumber) {
            cout << "\n🎉 Поздравляю! Вы угадали число " << targetNumber 
                 << " за " << attempts << " " << getAttemptsWord(attempts) << "!\n";
            break;
        } else if (guess < targetNumber) {
            cout << "⬆️  Больше! Попытка " << attempts << "\n";
        } else {
            cout << "⬇️  Меньше! Попытка " << attempts << "\n";
        }
    }
}

int playGameWithScore(int minRange, int maxRange) {
    int targetNumber = rand() % (maxRange - minRange + 1) + minRange;
    int guess;
    int attempts = 0;
    
    cout << "\n🎲 Я загадал число от " << minRange << " до " << maxRange << ". Попробуй угадать!\n\n";
    
    while (true) {
        cout << "Введите число: ";
        cin >> guess;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "❌ Некорректный ввод! Введите число.\n";
            continue;
        }
        
        attempts++;
        
        if (guess == targetNumber) {
            cout << "\n🎉 ПОБЕДА! Вы угадали число " << targetNumber 
                 << " за " << attempts << " " << getAttemptsWord(attempts) << "!\n";
            break;
        } else if (guess < targetNumber) {
            cout << "⬆️  Больше! Попытка " << attempts << "\n";
        } else {
            cout << "⬇️  Меньше! Попытка " << attempts << "\n";
        }
    }
    
    return attempts;
}

void playFullGame(int minRange, int maxRange, vector<Player>& leaderboard) {
    string playerName;
    cout << "\nВведите ваше имя: ";
    cin.ignore();
    getline(cin, playerName);
    
    int attempts = playGameWithScore(minRange, maxRange);
    
    addToLeaderboard(leaderboard, playerName, attempts);
    cout << "\n✅ Ваш результат сохранён!\n";
}

void showMenu() {
    cout << "\n╔════════════════════════════════════════════════╗\n";
    cout << "║           🎮 УГАДАЙ ЧИСЛО 🎮                 ║\n";
    cout << "╠════════════════════════════════════════════════╣\n";
    cout << "║  1. Этап 1: Базовая игра                     ║\n";
    cout << "║  2. Этап 2: Игра с подсчётом попыток         ║\n";
    cout << "║  3. Этап 3: Полная игра с рекордами          ║\n";
    cout << "║  4. Показать таблицу рекордов                ║\n";
    cout << "║  5. Выход                                    ║\n";
    cout << "╚════════════════════════════════════════════════╝\n";
    cout << "\nВыберите пункт меню: ";
}

int main() {
    
    srand(time(0));
    
    vector<Player> leaderboard = loadLeaderboard();
    
    int choice;
    int minRange = 1;
    int maxRange = 100;
    
    cout << "╔════════════════════════════════════════════════╗\n";
    cout << "║       Добро пожаловать в игру               ║\n";
    cout << "║           'УГАДАЙ ЧИСЛО'!                   ║\n";
    cout << "╚════════════════════════════════════════════════╝\n";
    
    while (true) {
        showMenu();
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n❌ Некорректный ввод!\n";
            continue;
        }
        
        switch (choice) {
            case 1:
                cout << "\n--- ЭТАП 1: БАЗОВАЯ ИГРА ---\n";
                playBasicGame(minRange, maxRange);
                break;
                
            case 2:
                cout << "\n--- ЭТАП 2: ИГРА С ПОДСЧЁТОМ ПОПЫТОК ---\n";
                playGameWithScore(minRange, maxRange);
                break;
                
            case 3:
                cout << "\n--- ЭТАП 3: ПОЛНАЯ ИГРА С РЕКОРДАМИ ---\n";
                playFullGame(minRange, maxRange, leaderboard);
                break;
                
            case 4:
                displayLeaderboard(leaderboard);
                break;
                
            case 5:
                cout << "\n👋 Спасибо за игру! До встречи!\n";
                return 0;
                
            default:
                cout << "\n❌ Неверный выбор! Попробуйте снова.\n";
        }
        
        cout << "\nНажмите Enter для продолжения...";
        cin.ignore();
        cin.get();
    }
    
    return 0;
}