
#ifndef RLE_H
#define RLE_H

#include <string>
#include <vector>

class RLE {
public:

    static std::vector<unsigned char> compress(const std::vector<unsigned char>& data);
    
    static std::vector<unsigned char> decompress(const std::vector<unsigned char>& data);
    
    static std::string compressText(const std::string& text);
    
    static std::string decompressText(const std::string& compressed);
    
    static double getCompressionRatio(size_t originalSize, size_t compressedSize);
    
    static void printCompressionStats(size_t originalSize, size_t compressedSize);
};

#endif // RLE_H

#include <iostream>
#include <iomanip>

std::vector<unsigned char> RLE::compress(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> compressed;
    
    if (data.empty()) {
        return compressed;
    }
    
    size_t i = 0;
    while (i < data.size()) {
        unsigned char current = data[i];
        int count = 1;
        
        while (i + count < data.size() && data[i + count] == current && count < 255) {
            count++;
        }
        
        compressed.push_back(static_cast<unsigned char>(count));
        compressed.push_back(current);
        
        i += count;
    }
    
    return compressed;
}

std::vector<unsigned char> RLE::decompress(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> decompressed;
    
    if (data.size() % 2 != 0) {
        std::cerr << "Ошибка: неверный формат сжатых данных!" << std::endl;
        return decompressed;
    }
    
    for (size_t i = 0; i < data.size(); i += 2) {
        int count = data[i];
        unsigned char value = data[i + 1];
        
        for (int j = 0; j < count; j++) {
            decompressed.push_back(value);
        }
    }
    
    return decompressed;
}

std::string RLE::compressText(const std::string& text) {
    if (text.empty()) {
        return "";
    }
    
    std::string compressed;
    size_t i = 0;
    
    while (i < text.size()) {
        char current = text[i];
        int count = 1;
        
        while (i + count < text.size() && text[i + count] == current && count < 99) {
            count++;
        }
        
        if (count >= 3) {
            compressed += std::to_string(count) + current;
        } else {

            for (int j = 0; j < count; j++) {
                compressed += current;
            }
        }
        
        i += count;
    }
    
    return compressed;
}

std::string RLE::decompressText(const std::string& compressed) {
    std::string decompressed;
    size_t i = 0;
    
    while (i < compressed.size()) {

        if (isdigit(compressed[i])) {
            int count = 0;
            
            while (i < compressed.size() && isdigit(compressed[i])) {
                count = count * 10 + (compressed[i] - '0');
                i++;
            }
            
            if (i < compressed.size()) {
                char value = compressed[i];
                for (int j = 0; j < count; j++) {
                    decompressed += value;
                }
                i++;
            }
        } else {
            decompressed += compressed[i];
            i++;
        }
    }
    
    return decompressed;
}

double RLE::getCompressionRatio(size_t originalSize, size_t compressedSize) {
    if (originalSize == 0) {
        return 0.0;
    }
    return (1.0 - (double)compressedSize / originalSize) * 100.0;
}

void RLE::printCompressionStats(size_t originalSize, size_t compressedSize) {
    double ratio = getCompressionRatio(originalSize, compressedSize);
    
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║       СТАТИСТИКА СЖАТИЯ               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    std::cout << "Исходный размер:  " << originalSize << " байт" << std::endl;
    std::cout << "Сжатый размер:    " << compressedSize << " байт" << std::endl;
    
    if (ratio > 0) {
        std::cout << "Степень сжатия:   " << std::fixed << std::setprecision(2) 
                  << ratio << "%" << std::endl;
        std::cout << "Результат:        ✓ Сжатие успешно" << std::endl;
    } else {
        std::cout << "Степень сжатия:   " << std::fixed << std::setprecision(2) 
                  << -ratio << "% (увеличение)" << std::endl;
        std::cout << "Результат:        ✗ Данные увеличились" << std::endl;
        std::cout << "Совет: RLE эффективен для данных с повторениями" << std::endl;
    }
}

#include <fstream>
#include <cstring>

class Archiver {
private:
    std::string inputFile;
    std::string outputFile;
    
public:
    // Сжатие файла
    void compressFile() {
        std::cout << "Введите имя исходного файла: ";
        std::cin >> inputFile;
        
        std::cout << "Введите имя для сжатого файла: ";
        std::cin >> outputFile;
        
        std::ifstream input(inputFile, std::ios::binary);
        if (!input.is_open()) {
            std::cout << "Ошибка: не удалось открыть файл '" << inputFile << "'" << std::endl;
            return;
        }
        
        std::vector<unsigned char> data((std::istreambuf_iterator<char>(input)),
                                        std::istreambuf_iterator<char>());
        input.close();
        
        if (data.empty()) {
            std::cout << "Файл пустой!" << std::endl;
            return;
        }
        
        std::cout << "\nСжатие файла..." << std::endl;
        std::vector<unsigned char> compressed = RLE::compress(data);
        
        std::ofstream output(outputFile, std::ios::binary);
        if (!output.is_open()) {
            std::cout << "Ошибка создания выходного файла!" << std::endl;
            return;
        }
        
        output.write(reinterpret_cast<char*>(compressed.data()), compressed.size());
        output.close();
        
        RLE::printCompressionStats(data.size(), compressed.size());
        std::cout << "\n✓ Файл сжат и сохранен: " << outputFile << std::endl;
    }
    
    void decompressFile() {
        std::cout << "Введите имя сжатого файла: ";
        std::cin >> inputFile;
        
        std::cout << "Введите имя для распакованного файла: ";
        std::cin >> outputFile;
        
        std::ifstream input(inputFile, std::ios::binary);
        if (!input.is_open()) {
            std::cout << "Ошибка: не удалось открыть файл '" << inputFile << "'" << std::endl;
            return;
        }
        
        std::vector<unsigned char> compressed((std::istreambuf_iterator<char>(input)),
                                              std::istreambuf_iterator<char>());
        input.close();
        
        std::cout << "\nРаспаковка файла..." << std::endl;
        std::vector<unsigned char> decompressed = RLE::decompress(compressed);
        
        if (decompressed.empty()) {
            std::cout << "Ошибка распаковки!" << std::endl;
            return;
        }
        
        std::ofstream output(outputFile, std::ios::binary);
        if (!output.is_open()) {
            std::cout << "Ошибка создания выходного файла!" << std::endl;
            return;
        }
        
        output.write(reinterpret_cast<char*>(decompressed.data()), decompressed.size());
        output.close();
        
        std::cout << "\n✓ Файл распакован и сохранен: " << outputFile << std::endl;
        std::cout << "Размер: " << decompressed.size() << " байт" << std::endl;
    }
    
    void compressText() {
        std::cin.ignore();
        std::string text;
        
        std::cout << "Введите текст для сжатия:" << std::endl;
        std::getline(std::cin, text);
        
        if (text.empty()) {
            std::cout << "Текст пустой!" << std::endl;
            return;
        }
        
        std::string compressed = RLE::compressText(text);
        
        std::cout << "\n--- РЕЗУЛЬТАТ СЖАТИЯ ---" << std::endl;
        std::cout << "Исходный текст:  " << text << std::endl;
        std::cout << "Сжатый текст:    " << compressed << std::endl;
        
        RLE::printCompressionStats(text.length(), compressed.length());
    }
    
    void decompressText() {
        std::cin.ignore();
        std::string compressed;
        
        std::cout << "Введите сжатый текст для распаковки:" << std::endl;
        std::getline(std::cin, compressed);
        
        std::string decompressed = RLE::decompressText(compressed);
        
        std::cout << "\n--- РЕЗУЛЬТАТ РАСПАКОВКИ ---" << std::endl;
        std::cout << "Сжатый текст:      " << compressed << std::endl;
        std::cout << "Распакованный:     " << decompressed << std::endl;
    }
    
    void createTestFile() {
        std::ofstream file("test.txt");
        
        if (!file.is_open()) {
            std::cout << "Ошибка создания тестового файла!" << std::endl;
            return;
        }
        
        file << "AAAAAABBBBBCCCCCDDDDD\n";
        file << "aaaaaabbbbbbcccccc\n";
        file << "111111222222333333\n";
        file << "        \n"; 
        file << "XXXXXXXXXXXXXXXXXX\n";
        
        file.close();
        std::cout << "✓ Тестовый файл 'test.txt' создан!" << std::endl;
        std::cout << "Этот файл содержит много повторений и хорошо сжимается RLE." << std::endl;
    }
    
    void showDemo() {
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║     ДЕМОНСТРАЦИЯ АЛГОРИТМА RLE        ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝" << std::endl;
        
        std::string examples[] = {
            "AAAAAABBBBCC",
            "111222333",
            "XXXXXXXXXXXXXXX",
            "ABCDEF"
        };
        
        for (const auto& example : examples) {
            std::string compressed = RLE::compressText(example);
            std::string decompressed = RLE::decompressText(compressed);
            
            std::cout << "\nИсходная:     " << example << std::endl;
            std::cout << "Сжатая:       " << compressed << std::endl;
            std::cout << "Распакованная: " << decompressed << std::endl;
            std::cout << "Проверка:     " << (example == decompressed ? "✓ OK" : "✗ ОШИБКА") << std::endl;
            
            double ratio = RLE::getCompressionRatio(example.length(), compressed.length());
            std::cout << "Сжатие:       " << std::fixed << std::setprecision(1) << ratio << "%" << std::endl;
        }
    }
};

void displayMenu() {
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║        АРХИВАТОР RLE                  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    std::cout << "1. Сжать файл" << std::endl;
    std::cout << "2. Распаковать файл" << std::endl;
    std::cout << "3. Сжать текст" << std::endl;
    std::cout << "4. Распаковать текст" << std::endl;
    std::cout << "5. Демонстрация алгоритма" << std::endl;
    std::cout << "6. Создать тестовый файл" << std::endl;
    std::cout << "0. Выход" << std::endl;
    std::cout << "───────────────────────────────────────" << std::endl;
    std::cout << "Выберите действие: ";
}

int main() {
    system("chcp 65001 > nul");
    
    Archiver archiver;
    int choice;
    
    do {
        displayMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                archiver.compressFile();
                break;
            case 2:
                archiver.decompressFile();
                break;
            case 3:
                archiver.compressText();
                break;
            case 4:
                archiver.decompressText();
                break;
            case 5:
                archiver.showDemo();
                break;
            case 6:
                archiver.createTestFile();
                break;
            case 0:
                std::cout << "\nДо свидания!" << std::endl;
                break;
            default:
                std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
        }
        
    } while (choice != 0);
    
    return 0;
}