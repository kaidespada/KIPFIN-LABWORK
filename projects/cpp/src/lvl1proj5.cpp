#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Polynomial {
    vector<double> coefficients; 
    
    Polynomial(int degree = 0) {
        coefficients.resize(degree + 1, 0.0);
    }
    
    Polynomial(const vector<double>& coefs) : coefficients(coefs) {
        removeLeadingZeros();
    }
    
    int degree() const {
        return coefficients.size() - 1;
    }
    
    void removeLeadingZeros() {
        while (coefficients.size() > 1 && abs(coefficients.back()) < 1e-10) {
            coefficients.pop_back();
        }
        if (coefficients.empty()) {
            coefficients.push_back(0);
        }
    }
    
    void display() const {
        if (coefficients.empty() || (coefficients.size() == 1 && coefficients[0] == 0)) {
            cout << "0";
            return;
        }
        
        bool first = true;
        
        for (int i = coefficients.size() - 1; i >= 0; i--) {
            double coef = coefficients[i];
            
            if (abs(coef) < 1e-10) continue; 
            
            if (!first) {
                cout << (coef > 0 ? " + " : " - ");
                coef = abs(coef);
            } else if (coef < 0) {
                cout << "-";
                coef = abs(coef);
            }
            
            if (i == 0 || abs(coef - 1.0) > 1e-10) {
                if (abs(coef - floor(coef)) < 1e-10) {
                    cout << (int)coef;
                } else {
                    cout << fixed << setprecision(2) << coef;
                }
            }
            
            if (i > 0) {
                cout << "x";
                if (i > 1) {
                    cout << "^" << i;
                }
            }
            
            first = false;
        }
        
        if (first) cout << "0";
    }
};

Polynomial add(const Polynomial& p1, const Polynomial& p2) {
    int maxDegree = max(p1.degree(), p2.degree());
    Polynomial result(maxDegree);
    
    for (int i = 0; i <= maxDegree; i++) {
        double c1 = (i <= p1.degree()) ? p1.coefficients[i] : 0;
        double c2 = (i <= p2.degree()) ? p2.coefficients[i] : 0;
        result.coefficients[i] = c1 + c2;
    }
    
    result.removeLeadingZeros();
    return result;
}

Polynomial subtract(const Polynomial& p1, const Polynomial& p2) {
    int maxDegree = max(p1.degree(), p2.degree());
    Polynomial result(maxDegree);
    
    for (int i = 0; i <= maxDegree; i++) {
        double c1 = (i <= p1.degree()) ? p1.coefficients[i] : 0;
        double c2 = (i <= p2.degree()) ? p2.coefficients[i] : 0;
        result.coefficients[i] = c1 - c2;
    }
    
    result.removeLeadingZeros();
    return result;
}

Polynomial multiply(const Polynomial& p1, const Polynomial& p2) {
    int resultDegree = p1.degree() + p2.degree();
    Polynomial result(resultDegree);
    
    for (int i = 0; i <= p1.degree(); i++) {
        for (int j = 0; j <= p2.degree(); j++) {
            result.coefficients[i + j] += p1.coefficients[i] * p2.coefficients[j];
        }
    }
    
    result.removeLeadingZeros();
    return result;
}

double hornerRecursive(const vector<double>& coefs, int index, double x) {

    if (index < 0) {
        return 0;
    }
    
    return coefs[index] + x * hornerRecursive(coefs, index - 1, x);
}

double evaluateHorner(const Polynomial& p, double x) {
    if (p.coefficients.empty()) {
        return 0;
    }
    return hornerRecursive(p.coefficients, p.degree(), x);
}

double evaluateDirect(const Polynomial& p, double x) {
    double result = 0;
    double power = 1;
    
    for (int i = 0; i <= p.degree(); i++) {
        result += p.coefficients[i] * power;
        power *= x;
    }
    
    return result;
}

Polynomial inputPolynomial() {
    int degree;
    cout << "Введите степень многочлена: ";
    cin >> degree;
    
    if (degree < 0) {
        cout << "Степень не может быть отрицательной. Установлено 0." << endl;
        degree = 0;
    }
    
    Polynomial p(degree);
    
    cout << "Введите коэффициенты (от свободного члена к старшему):" << endl;
    for (int i = 0; i <= degree; i++) {
        cout << "Коэффициент при x^" << i << ": ";
        cin >> p.coefficients[i];
    }
    
    p.removeLeadingZeros();
    return p;
}

void createTestPolynomials(Polynomial& p1, Polynomial& p2) {

    p1.coefficients = {1, -5, 3, 2};
    
    p2.coefficients = {4, -2, 1};
    
    cout << "✓ Созданы тестовые многочлены:" << endl;
    cout << "P1(x) = ";
    p1.display();
    cout << endl;
    cout << "P2(x) = ";
    p2.display();
    cout << endl;
}

void demonstrateHorner(const Polynomial& p, double x) {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║     ВЫЧИСЛЕНИЕ ПО СХЕМЕ ГОРНЕРА       ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    
    cout << "Многочлен: ";
    p.display();
    cout << endl;
    cout << "Значение x = " << x << endl;
    cout << "───────────────────────────────────────" << endl;
    
    cout << "\nШаги вычисления (справа налево):" << endl;
    vector<double> steps;
    double current = 0;
    
    for (int i = p.degree(); i >= 0; i--) {
        current = p.coefficients[i] + x * current;
        cout << "Шаг " << (p.degree() - i + 1) << ": " 
             << p.coefficients[i] << " + " << x << " * ";
        if (i == p.degree()) {
            cout << "0";
        } else {
            cout << setprecision(4) << (current - p.coefficients[i]) / x;
        }
        cout << " = " << setprecision(4) << current << endl;
    }
    
    cout << "\n───────────────────────────────────────" << endl;
    cout << "Результат (Горнер):  " << fixed << setprecision(6) << current << endl;
    
    double direct = evaluateDirect(p, x);
    cout << "Результат (прямой):  " << fixed << setprecision(6) << direct << endl;
    cout << "Разница:             " << scientific << setprecision(2) 
         << abs(current - direct) << endl;
}

void displayMenu() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║   КАЛЬКУЛЯТОР МНОГОЧЛЕНОВ             ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "1.  Ввести первый многочлен" << endl;
    cout << "2.  Ввести второй многочлен" << endl;
    cout << "3.  Показать многочлены" << endl;
    cout << "4.  Сложить многочлены (P1 + P2)" << endl;
    cout << "5.  Вычесть многочлены (P1 - P2)" << endl;
    cout << "6.  Умножить многочлены (P1 * P2)" << endl;
    cout << "7.  Вычислить значение P1(x)" << endl;
    cout << "8.  Вычислить значение P2(x)" << endl;
    cout << "9.  Демонстрация схемы Горнера" << endl;
    cout << "10. Создать тестовые многочлены" << endl;
    cout << "0.  Выход" << endl;
    cout << "───────────────────────────────────────" << endl;
    cout << "Выберите действие: ";
}

int main() {

    system("chcp 65001 > nul");
    
    Polynomial p1, p2;
    int choice;
    
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "\n=== Ввод первого многочлена ===" << endl;
                p1 = inputPolynomial();
                cout << "P1(x) = ";
                p1.display();
                cout << endl;
                break;
                
            case 2:
                cout << "\n=== Ввод второго многочлена ===" << endl;
                p2 = inputPolynomial();
                cout << "P2(x) = ";
                p2.display();
                cout << endl;
                break;
                
            case 3:
                cout << "\n=== Текущие многочлены ===" << endl;
                cout << "P1(x) = ";
                p1.display();
                cout << endl;
                cout << "P2(x) = ";
                p2.display();
                cout << endl;
                break;
                
            case 4: {
                cout << "\n=== Сложение ===" << endl;
                cout << "P1(x) = ";
                p1.display();
                cout << endl;
                cout << "P2(x) = ";
                p2.display();
                cout << endl;
                
                Polynomial sum = add(p1, p2);
                cout << "───────────────────────────────────────" << endl;
                cout << "Результат: ";
                sum.display();
                cout << endl;
                break;
            }
                
            case 5: {
                cout << "\n=== Вычитание ===" << endl;
                cout << "P1(x) = ";
                p1.display();
                cout << endl;
                cout << "P2(x) = ";
                p2.display();
                cout << endl;
                
                Polynomial diff = subtract(p1, p2);
                cout << "───────────────────────────────────────" << endl;
                cout << "Результат: ";
                diff.display();
                cout << endl;
                break;
            }
                
            case 6: {
                cout << "\n=== Умножение ===" << endl;
                cout << "P1(x) = ";
                p1.display();
                cout << endl;
                cout << "P2(x) = ";
                p2.display();
                cout << endl;
                
                Polynomial prod = multiply(p1, p2);
                cout << "───────────────────────────────────────" << endl;
                cout << "Результат: ";
                prod.display();
                cout << endl;
                break;
            }
                
            case 7: {
                double x;
                cout << "\nВведите значение x: ";
                cin >> x;
                
                double result = evaluateHorner(p1, x);
                cout << "\nP1(" << x << ") = " << fixed << setprecision(6) << result << endl;
                break;
            }
                
            case 8: {
                double x;
                cout << "\nВведите значение x: ";
                cin >> x;
                
                double result = evaluateHorner(p2, x);
                cout << "\nP2(" << x << ") = " << fixed << setprecision(6) << result << endl;
                break;
            }
                
            case 9: {
                int polyChoice;
                cout << "\nВыберите многочлен (1 - P1, 2 - P2): ";
                cin >> polyChoice;
                
                double x;
                cout << "Введите значение x: ";
                cin >> x;
                
                if (polyChoice == 1) {
                    demonstrateHorner(p1, x);
                } else if (polyChoice == 2) {
                    demonstrateHorner(p2, x);
                } else {
                    cout << "Неверный выбор!" << endl;
                }
                break;
            }
                
            case 10:
                createTestPolynomials(p1, p2);
                break;
                
            case 0:
                cout << "\nДо свидания!" << endl;
                break;
                
            default:
                cout << "Неверный выбор! Попробуйте снова." << endl;
        }
        
    } while (choice != 0);
    
    return 0;
}