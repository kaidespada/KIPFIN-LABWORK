#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Book {
    string author;
    string title;
    int year;
    bool available; 
};

class Library {
private:
    vector<Book> catalog;
    const string filename = "library.dat";

public:

    void addBook() {
        Book book;
        cin.ignore();
        
        cout << "\n=== Добавление книги ===" << endl;
        cout << "Введите автора: ";
        getline(cin, book.author);
        
        cout << "Введите название: ";
        getline(cin, book.title);
        
        cout << "Введите год издания: ";
        cin >> book.year;
        
        book.available = true;
        catalog.push_back(book);
        
        cout << "\n✓ Книга успешно добавлена!" << endl;
    }
    
    void searchByAuthor() {
        string author;
        cin.ignore();
        
        cout << "\nВведите имя автора: ";
        getline(cin, author);
        
        bool found = false;
        cout << "\n=== Результаты поиска ===" << endl;
        
        for (size_t i = 0; i < catalog.size(); i++) {
            if (catalog[i].author.find(author) != string::npos) {
                displayBook(catalog[i], i + 1);
                found = true;
            }
        }
        
        if (!found) {
            cout << "Книги данного автора не найдены." << endl;
        }
    }
    
    void searchByTitle() {
        string title;
        cin.ignore();
        
        cout << "\nВведите название книги: ";
        getline(cin, title);
        
        bool found = false;
        cout << "\n=== Результаты поиска ===" << endl;
        
        for (size_t i = 0; i < catalog.size(); i++) {
            if (catalog[i].title.find(title) != string::npos) {
                displayBook(catalog[i], i + 1);
                found = true;
            }
        }
        
        if (!found) {
            cout << "Книги с таким названием не найдены." << endl;
        }
    }
    
    void displayCatalog() {
        if (catalog.empty()) {
            cout << "\nКаталог пуст." << endl;
            return;
        }
        
        cout << "\n=== Каталог библиотеки ===" << endl;
        cout << string(80, '=') << endl;
        
        for (size_t i = 0; i < catalog.size(); i++) {
            displayBook(catalog[i], i + 1);
        }
    }
    
    void toggleBookStatus() {
        if (catalog.empty()) {
            cout << "\nКаталог пуст." << endl;
            return;
        }
        
        displayCatalog();
        
        int index;
        cout << "\nВведите номер книги: ";
        cin >> index;
        
        if (index < 1 || index > (int)catalog.size()) {
            cout << "Неверный номер книги!" << endl;
            return;
        }
        
        index--; 
        
        if (catalog[index].available) {
            catalog[index].available = false;
            cout << "\n✓ Книга выдана читателю." << endl;
        } else {
            catalog[index].available = true;
            cout << "\n✓ Книга возвращена в библиотеку." << endl;
        }
    }
    
    void saveCatalog() {
        ofstream file(filename, ios::binary);
        
        if (!file) {
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
        }
        
        size_t size = catalog.size();
        file.write((char*)&size, sizeof(size));
        
        for (const auto& book : catalog) {
            
            size_t len = book.author.length();
            file.write((char*)&len, sizeof(len));
            file.write(book.author.c_str(), len);
            
            len = book.title.length();
            file.write((char*)&len, sizeof(len));
            file.write(book.title.c_str(), len);
            
            file.write((char*)&book.year, sizeof(book.year));
            file.write((char*)&book.available, sizeof(book.available));
        }
        
        file.close();
        cout << "\n✓ Каталог сохранен в файл." << endl;
    }
    
    
    void loadCatalog() {
        ifstream file(filename, ios::binary);
        
        if (!file) {
            cout << "Файл каталога не найден. Создан новый каталог." << endl;
            return;
        }
        
        catalog.clear();
        
        size_t size;
        file.read((char*)&size, sizeof(size));
        
        for (size_t i = 0; i < size; i++) {
            Book book;
            
            size_t len;
            file.read((char*)&len, sizeof(len));
            book.author.resize(len);
            file.read(&book.author[0], len);
            
            file.read((char*)&len, sizeof(len));
            book.title.resize(len);
            file.read(&book.title[0], len);
            
            file.read((char*)&book.year, sizeof(book.year));
            file.read((char*)&book.available, sizeof(book.available));
            
            catalog.push_back(book);
        }
        
        file.close();
        cout << "✓ Каталог загружен из файла. Книг в каталоге: " << catalog.size() << endl;
    }
    
private:

    void displayBook(const Book& book, int number) {
        cout << "\n[" << number << "] " << book.title << endl;
        cout << "    Автор: " << book.author << endl;
        cout << "    Год издания: " << book.year << endl;
        cout << "    Статус: " << (book.available ? "✓ В библиотеке" : "✗ Выдана") << endl;
    }
};

void displayMenu() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║   СИСТЕМА УПРАВЛЕНИЯ БИБЛИОТЕКОЙ      ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "1. Добавить книгу" << endl;
    cout << "2. Поиск по автору" << endl;
    cout << "3. Поиск по названию" << endl;
    cout << "4. Показать весь каталог" << endl;
    cout << "5. Выдать/вернуть книгу" << endl;
    cout << "6. Сохранить каталог" << endl;
    cout << "7. Загрузить каталог" << endl;
    cout << "0. Выход" << endl;
    cout << "───────────────────────────────────────" << endl;
    cout << "Выберите действие: ";
}

int main() {
    
    system("chcp 65001 > nul");
    
    Library library;
    int choice;
    
    library.loadCatalog();
    
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                library.addBook();
                break;
            case 2:
                library.searchByAuthor();
                break;
            case 3:
                library.searchByTitle();
                break;
            case 4:
                library.displayCatalog();
                break;
            case 5:
                library.toggleBookStatus();
                break;
            case 6:
                library.saveCatalog();
                break;
            case 7:
                library.loadCatalog();
                break;
            case 0:
                cout << "\nСохранить изменения перед выходом? (1-да, 0-нет): ";
                int save;
                cin >> save;
                if (save == 1) {
                    library.saveCatalog();
                }
                cout << "До свидания!" << endl;
                break;
            default:
                cout << "Неверный выбор! Попробуйте снова." << endl;
        }
        
    } while (choice != 0);
    
    return 0;
}