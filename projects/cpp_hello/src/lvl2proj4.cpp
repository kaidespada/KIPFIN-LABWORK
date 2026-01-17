#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cctype>

using namespace std;

template <typename T>
struct Node {
    T data;
    Node* next;
    
    Node(T value) : data(value), next(nullptr) {}
};

template <typename T>
class Stack {
private:
    Node<T>* top;
    int size;
    
public:
    Stack() : top(nullptr), size(0) {}
    
    ~Stack() {
        clear();
    }
    
    void push(T value) {
        Node<T>* newNode = new Node<T>(value);
        newNode->next = top;
        top = newNode;
        size++;
    }
    
    T pop() {
        if (isEmpty()) {
            throw runtime_error("Stack underflow");
        }
        
        Node<T>* temp = top;
        T value = top->data;
        top = top->next;
        delete temp;
        size--;
        return value;
    }
    
    T peek() const {
        if (isEmpty()) {
            throw runtime_error("Stack is empty");
        }
        return top->data;
    }
    
    bool isEmpty() const {
        return top == nullptr;
    }
    
    int getSize() const {
        return size;
    }
    
    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }
};

class RPNCalculator {
private:
    
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
    }
    
    int getPrecedence(char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        if (op == '^') return 3;
        return 0;
    }
    
    bool isRightAssociative(char op) {
        return op == '^';
    }
    
    double applyOperator(double a, double b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/':
                if (b == 0) throw runtime_error("Division by zero");
                return a / b;
            case '^': return pow(a, b);
            default: throw runtime_error("Unknown operator");
        }
    }
    
public:
    
    double evaluateRPN(const string& expression) {
        Stack<double> stack;
        stringstream ss(expression);
        string token;
        
        while (ss >> token) {
            if (isdigit(token[0]) || (token.length() > 1 && token[0] == '-')) {
                stack.push(stod(token));
            }
            else if (token.length() == 1 && isOperator(token[0])) {
                if (stack.getSize() < 2) {
                    throw runtime_error("Invalid expression: not enough operands");
                }
                
                double b = stack.pop();
                double a = stack.pop();
                double result = applyOperator(a, b, token[0]);
                stack.push(result);
            }
            else {
                throw runtime_error("Invalid token: " + token);
            }
        }
        
        if (stack.getSize() != 1) {
            throw runtime_error("Invalid expression: too many operands");
        }
        
        return stack.pop();
    }
    
    string infixToRPN(const string& expression) {
        Stack<char> operators;
        stringstream output;
        
        for (size_t i = 0; i < expression.length(); i++) {
            char c = expression[i];
            
            if (isspace(c)) continue;
            
            if (isdigit(c) || c == '.') {
                output << c;
                
                while (i + 1 < expression.length() && 
                       (isdigit(expression[i + 1]) || expression[i + 1] == '.')) {
                    output << expression[++i];
                }
                output << ' ';
            }
            else if (c == '(') {
                operators.push(c);
            }
            else if (c == ')') {
                while (!operators.isEmpty() && operators.peek() != '(') {
                    output << operators.pop() << ' ';
                }
                
                if (operators.isEmpty()) {
                    throw runtime_error("Mismatched parentheses");
                }
                
                operators.pop();
            }
            else if (isOperator(c)) {
                while (!operators.isEmpty() && operators.peek() != '(' &&
                       (getPrecedence(operators.peek()) > getPrecedence(c) ||
                        (getPrecedence(operators.peek()) == getPrecedence(c) && 
                         !isRightAssociative(c)))) {
                    output << operators.pop() << ' ';
                }
                operators.push(c);
            }
            else {
                throw runtime_error(string("Invalid character: ") + c);
            }
        }
        
        while (!operators.isEmpty()) {
            char op = operators.pop();
            if (op == '(' || op == ')') {
                throw runtime_error("Mismatched parentheses");
            }
            output << op << ' ';
        }
        
        return output.str();
    }
    
    double evaluateInfix(const string& expression) {
        string rpn = infixToRPN(expression);
        return evaluateRPN(rpn);
    }
};

void displayMenu() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║    КАЛЬКУЛЯТОР RPN (ОПН)              ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "1. Вычислить постфиксное выражение (RPN)" << endl;
    cout << "2. Вычислить инфиксное выражение" << endl;
    cout << "3. Конвертировать инфикс → постфикс" << endl;
    cout << "4. Примеры использования" << endl;
    cout << "0. Выход" << endl;
    cout << "───────────────────────────────────────" << endl;
    cout << "Выберите действие: ";
}

void showExamples() {
    cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║              ПРИМЕРЫ ИСПОЛЬЗОВАНИЯ                    ║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝" << endl;
    
    cout << "\n--- ПОСТФИКСНАЯ ЗАПИСЬ (RPN) ---" << endl;
    cout << "Формат: операнды идут перед оператором" << endl;
    cout << "Примеры:" << endl;
    cout << "  5 3 +          →  8" << endl;
    cout << "  10 2 /         →  5" << endl;
    cout << "  5 3 + 2 *      →  16" << endl;
    cout << "  15 7 1 1 + - / 3 * 2 1 1 + + -  →  5" << endl;
    cout << "  2 3 ^          →  8 (возведение в степень)" << endl;
    
    cout << "\n--- ИНФИКСНАЯ ЗАПИСЬ (обычная) ---" << endl;
    cout << "Формат: оператор между операндами" << endl;
    cout << "Примеры:" << endl;
    cout << "  5 + 3          →  8" << endl;
    cout << "  (5 + 3) * 2    →  16" << endl;
    cout << "  10 / 2 + 3     →  8" << endl;
    cout << "  2 ^ 3          →  8" << endl;
    cout << "  (2 + 3) * (4 - 1)  →  15" << endl;
    
    cout << "\nПоддерживаемые операторы: + - * / ^" << endl;
    cout << "Можно использовать скобки и дробные числа" << endl;
}

int main() {
    system("chcp 65001 > nul");
    
    RPNCalculator calc;
    int choice;
    
    do {
        displayMenu();
        cin >> choice;
        cin.ignore();
        
        try {
            switch (choice) {
                case 1: {
                    string expr;
                    cout << "\nВведите постфиксное выражение: ";
                    getline(cin, expr);
                    
                    double result = calc.evaluateRPN(expr);
                    cout << "\n╔════════════════════════════════════════╗" << endl;
                    cout << "║           РЕЗУЛЬТАТ                   ║" << endl;
                    cout << "╚════════════════════════════════════════╝" << endl;
                    cout << "Выражение (RPN): " << expr << endl;
                    cout << "Результат:       " << result << endl;
                    break;
                }
                
                case 2: {
                    string expr;
                    cout << "\nВведите инфиксное выражение: ";
                    getline(cin, expr);
                    
                    string rpn = calc.infixToRPN(expr);
                    double result = calc.evaluateRPN(rpn);
                    
                    cout << "\n╔════════════════════════════════════════╗" << endl;
                    cout << "║           РЕЗУЛЬТАТ                   ║" << endl;
                    cout << "╚════════════════════════════════════════╝" << endl;
                    cout << "Исходное выражение: " << expr << endl;
                    cout << "Постфиксная форма:  " << rpn << endl;
                    cout << "Результат:          " << result << endl;
                    break;
                }
                
                case 3: {
                    string expr;
                    cout << "\nВведите инфиксное выражение: ";
                    getline(cin, expr);
                    
                    string rpn = calc.infixToRPN(expr);
                    
                    cout << "\n╔════════════════════════════════════════╗" << endl;
                    cout << "║          КОНВЕРТАЦИЯ                  ║" << endl;
                    cout << "╚════════════════════════════════════════╝" << endl;
                    cout << "Инфикс:    " << expr << endl;
                    cout << "Постфикс:  " << rpn << endl;
                    break;
                }
                
                case 4:
                    showExamples();
                    break;
                
                case 0:
                    cout << "\nДо свидания!" << endl;
                    break;
                
                default:
                    cout << "Неверный выбор!" << endl;
            }
        }
        catch (const exception& e) {
            cout << "\n✗ ОШИБКА: " << e.what() << endl;
        }
        
    } while (choice != 0);
    
    return 0;
}