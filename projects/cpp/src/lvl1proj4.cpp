#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <vector>
#include <iomanip>

using namespace std;

class TextAnalyzer {
private:
    string filename;
    string content;
    int lineCount;
    int wordCount;
    int charCount;
    map<string, int> wordFrequency;
    
public:
    TextAnalyzer() : lineCount(0), wordCount(0), charCount(0) {}
    
    bool readFile(const string& fname) {
        filename = fname;
        ifstream file(filename);
        
        if (!file.is_open()) {
            cout << "Ошибка: не удалось открыть файл '" << filename << "'" << endl;
            return false;
        }
        
        content.clear();
        string line;
        
        while (getline(file, line)) {
            content += line + "\n";
        }
        
        file.close();
        
        if (content.empty()) {
            cout << "Предупреждение: файл пустой!" << endl;
            return false;
        }
        
        cout << "✓ Файл успешно загружен: " << filename << endl;
        return true;
    }
    
    void analyze() {
        lineCount = 0;
        wordCount = 0;
        charCount = 0;
        wordFrequency.clear();
        
        stringstream ss(content);
        string line;
        
        while (getline(ss, line)) {
            lineCount++;
            charCount += line.length();
            
            processLine(line);
        }
        
        cout << "\n✓ Анализ завершен!" << endl;
    }
    
    void displayStatistics() {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║       СТАТИСТИКА ТЕКСТА               ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "Файл: " << filename << endl;
        cout << "───────────────────────────────────────" << endl;
        cout << "Количество строк:    " << lineCount << endl;
        cout << "Количество слов:     " << wordCount << endl;
        cout << "Количество символов: " << charCount << endl;
        cout << "Уникальных слов:     " << wordFrequency.size() << endl;
        
        if (wordCount > 0) {
            cout << "Средняя длина слова: " << fixed << setprecision(2) 
                 << (double)charCount / wordCount << " символов" << endl;
        }
    }
    
    void searchWordFrequency() {
        string word;
        cout << "\nВведите слово для поиска: ";
        cin >> word;
        
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        
        auto it = wordFrequency.find(word);
        
        if (it != wordFrequency.end()) {
            cout << "\nСлово '" << word << "' встречается " << it->second << " раз(а)" << endl;
            
            if (wordCount > 0) {
                double percentage = (double)it->second / wordCount * 100;
                cout << "Это составляет " << fixed << setprecision(2) 
                     << percentage << "% от всех слов" << endl;
            }
        } else {
            cout << "\nСлово '" << word << "' не найдено в тексте." << endl;
        }
    }
    
    void displayMostFrequentWords(int topN = 10) {
        if (wordFrequency.empty()) {
            cout << "\nНет данных для отображения." << endl;
            return;
        }
        
        vector<pair<string, int>> words(wordFrequency.begin(), wordFrequency.end());
        
        sort(words.begin(), words.end(), 
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 return a.second > b.second;
             });
        
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║      ТОП-" << topN << " ЧАСТЫХ СЛОВ             ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        
        int count = min(topN, (int)words.size());
        
        for (int i = 0; i < count; i++) {
            cout << setw(3) << (i + 1) << ". " 
                 << setw(20) << left << words[i].first 
                 << " - " << words[i].second << " раз(а)" << endl;
        }
    }
    
    void displayLongestWords(int topN = 10) {
        if (wordFrequency.empty()) {
            cout << "\nНет данных для отображения." << endl;
            return;
        }
        
        vector<pair<string, int>> words(wordFrequency.begin(), wordFrequency.end());
        
        sort(words.begin(), words.end(), 
             [](const pair<string, int>& a, const pair<string, int>& b) {
                 if (a.first.length() == b.first.length()) {
                     return a.second > b.second; 
                 }
                 return a.first.length() > b.first.length();
             });
        
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║      ТОП-" << topN << " ДЛИННЫХ СЛОВ            ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        
        int count = min(topN, (int)words.size());
        
        for (int i = 0; i < count; i++) {
            cout << setw(3) << (i + 1) << ". " 
                 << setw(25) << left << words[i].first 
                 << " (" << words[i].first.length() << " симв.)" << endl;
        }
    }
    
    void displayExtendedStatistics() {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║    РАСШИРЕННАЯ СТАТИСТИКА             ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        
        if (!wordFrequency.empty()) {
            auto mostFrequent = max_element(wordFrequency.begin(), wordFrequency.end(),
                [](const pair<string, int>& a, const pair<string, int>& b) {
                    return a.second < b.second;
                });
            
            cout << "Самое частое слово: '" << mostFrequent->first 
                 << "' (" << mostFrequent->second << " раз)" << endl;
        }
        
        if (!wordFrequency.empty()) {
            auto longest = max_element(wordFrequency.begin(), wordFrequency.end(),
                [](const pair<string, int>& a, const pair<string, int>& b) {
                    return a.first.length() < b.first.length();
                });
            
            cout << "Самое длинное слово: '" << longest->first 
                 << "' (" << longest->first.length() << " символов)" << endl;
        }
        
        if (lineCount > 0) {
            cout << "Средняя длина строки: " << fixed << setprecision(2)
                 << (double)charCount / lineCount << " символов" << endl;
        }
    }
    
    static void createTestFile() {
        ofstream file("test.txt");
        
        if (!file.is_open()) {
            cout << "Ошибка создания тестового файла!" << endl;
            return;
        }
        
        file << "Это тестовый текст для анализа текстового файла.\n";
        file << "В этом тексте есть повторяющиеся слова.\n";
        file << "Программа подсчитает количество слов, строк и символов.\n";
        file << "Также программа найдет самое частое слово.\n";
        file << "Слово 'программа' встречается несколько раз.\n";
        file << "Анализатор текста - полезный инструмент для обработки файлов.\n";
        
        file.close();
        cout << "✓ Тестовый файл 'test.txt' создан!" << endl;
    }
    
private:

    void processLine(const string& line) {
        string word;
        string cleanedWord;
        
        for (char c : line) {
            if (isalnum(c) || c == '-' || c == '\'') {
                word += c;
            } else {
                if (!word.empty()) {
                    processWord(word);
                    word.clear();
                }
            }
        }
        
        if (!word.empty()) {
            processWord(word);
        }
    }
    
    void processWord(const string& word) {
        string lowerWord = word;
        
        transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
        
        wordCount++;
        wordFrequency[lowerWord]++;
    }
};

void displayMenu() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║     АНАЛИЗАТОР ТЕКСТОВОГО ФАЙЛА       ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "1. Загрузить файл" << endl;
    cout << "2. Показать общую статистику" << endl;
    cout << "3. Найти частоту слова" << endl;
    cout << "4. Показать самые частые слова" << endl;
    cout << "5. Показать самые длинные слова" << endl;
    cout << "6. Расширенная статистика" << endl;
    cout << "7. Создать тестовый файл" << endl;
    cout << "0. Выход" << endl;
    cout << "───────────────────────────────────────" << endl;
    cout << "Выберите действие: ";
}

int main() {

    system("chcp 65001 > nul");
    
    TextAnalyzer analyzer;
    int choice;
    bool fileLoaded = false;
    
    do {
        displayMenu();
        cin >> choice;
        cin.ignore(); 
        
        switch (choice) {
            case 1: {
                string filename;
                cout << "Введите имя файла: ";
                getline(cin, filename);
                
                if (analyzer.readFile(filename)) {
                    analyzer.analyze();
                    fileLoaded = true;
                }
                break;
            }
            case 2:
                if (fileLoaded) {
                    analyzer.displayStatistics();
                } else {
                    cout << "Сначала загрузите файл!" << endl;
                }
                break;
                
            case 3:
                if (fileLoaded) {
                    analyzer.searchWordFrequency();
                } else {
                    cout << "Сначала загрузите файл!" << endl;
                }
                break;
                
            case 4:
                if (fileLoaded) {
                    analyzer.displayMostFrequentWords(10);
                } else {
                    cout << "Сначала загрузите файл!" << endl;
                }
                break;
                
            case 5:
                if (fileLoaded) {
                    analyzer.displayLongestWords(10);
                } else {
                    cout << "Сначала загрузите файл!" << endl;
                }
                break;
                
            case 6:
                if (fileLoaded) {
                    analyzer.displayExtendedStatistics();
                } else {
                    cout << "Сначала загрузите файл!" << endl;
                }
                break;
                
            case 7:
                TextAnalyzer::createTestFile();
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