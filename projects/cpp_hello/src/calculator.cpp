#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>

using namespace std;

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) {
        cout << "Ошибка: Деление на ноль!" << endl;
        return 0;
    }
    return a / b;
}

void saveToHistory(double num1, double num2, char op, double result) {
    ofstream file("history.txt", ios::app);
    
    if (file.is_open()) {
 
        time_t now = time(0);
        char* dt = ctime(&now);
        
        file << "[" << dt;
        file.seekp(-1, ios::cur);
        file << "] ";
        file << num1 << " " << op << " " << num2 << " = " << result << endl;
        file.close();
    } else {
        cout << "Ошибка: Не удалось открыть файл истории!" << endl;
    }
}

void showHistory() {
    ifstream file("history.txt");
    
    if (file.is_open()) {
        string line;
        cout << "\n=== ИСТОРИЯ ВЫЧИСЛЕНИЙ ===" << endl;
        
        if (file.peek() == ifstream::traits_type::eof()) {
            cout << "История пуста." << endl;
        } else {
            while (getline(file, line)) {
                cout << line << endl;
            }
        }
        file.close();
        cout << "==========================\n" << endl;
    } else {
        cout << "Файл истории не найден или пуст." << endl;
    }
}

void clearHistory() {
    ofstream file("history.txt", ios::trunc);
    file.close();
    cout << "История очищена." << endl;
}

int main() {

    double num1, num2, result;
    char operation;
    string command;
    
    cout << "========================================" << endl;
    cout << "   КАЛЬКУЛЯТОР С ИСТОРИЕЙ ОПЕРАЦИЙ" << endl;
    cout << "========================================" << endl;
    cout << "Доступные операции: +, -, *, /" << endl;
    cout << "Команды:" << endl;
    cout << "  'история' - показать историю" << endl;
    cout << "  'очистить' - очистить историю" << endl;
    cout << "  'выход' - завершить работу" << endl;
    cout << "========================================\n" << endl;
    
    while (true) {
        cout << "Введите команду или первое число: ";
        cin >> command;
        
        if (command == "выход" || command == "exit") {
            cout << "\nЗавершение работы калькулятора. До свидания!" << endl;
            break;
        }
        
        if (command == "история" || command == "history") {
            showHistory();
            continue;
        }
        
        if (command == "очистить" || command == "clear") {
            clearHistory();
            continue;
        }
        
        try {
            num1 = stod(command);
        } catch (...) {
            cout << "Ошибка: Неверный ввод!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        
        cout << "Введите операцию (+, -, *, /): ";
        cin >> operation;
        
        if (operation != '+' && operation != '-' && operation != '*' && operation != '/') {
            cout << "Ошибка: Неизвестная операция!" << endl;
            continue;
        }
        
        cout << "Введите второе число: ";
        cin >> num2;
        
        if (cin.fail()) {
            cout << "Ошибка: Неверный ввод!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        
        switch (operation) {
            case '+':
                result = add(num1, num2);
                break;
            case '-':
                result = subtract(num1, num2);
                break;
            case '*':
                result = multiply(num1, num2);
                break;
            case '/':
                if (num2 == 0) {
                    cout << "Ошибка: Деление на ноль!" << endl;
                    continue;
                }
                result = divide(num1, num2);
                break;
            default:
                cout << "Ошибка: Неизвестная операция!" << endl;
                continue;
        }
        
        cout << fixed << setprecision(2);
        cout << "\nРезультат: " << num1 << " " << operation << " " << num2 << " = " << result << "\n" << endl;
        
        saveToHistory(num1, num2, operation, result);
    }
    
    return 0;
}