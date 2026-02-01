#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

struct Contact {
    int id;
    std::string name;
    std::string phone;
    std::string email;
    std::string address;
    
    Contact() : id(0) {}
    
    Contact(int _id, const std::string& _name, const std::string& _phone,
            const std::string& _email, const std::string& _address)
        : id(_id), name(_name), phone(_phone), email(_email), address(_address) {}
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["name"] = QString::fromStdString(name);
        obj["phone"] = QString::fromStdString(phone);
        obj["email"] = QString::fromStdString(email);
        obj["address"] = QString::fromStdString(address);
        return obj;
    }
    
    static Contact fromJson(const QJsonObject& obj) {
        Contact contact;
        contact.id = obj["id"].toInt();
        contact.name = obj["name"].toString().toStdString();
        contact.phone = obj["phone"].toString().toStdString();
        contact.email = obj["email"].toString().toStdString();
        contact.address = obj["address"].toString().toStdString();
        return contact;
    }
};

class NotebookService {
private:
    std::vector<Contact> contacts;
    std::string filename;
    int nextId;
    
public:
    NotebookService(const std::string& file = "contacts.json") 
        : filename(file), nextId(1) {
        loadFromFile();
    }
    
    bool addContact(const Contact& contact) {
        Contact newContact = contact;
        newContact.id = nextId++;
        contacts.push_back(newContact);
        saveToFile();
        return true;
    }
    
    bool updateContact(int id, const Contact& contact) {
        auto it = std::find_if(contacts.begin(), contacts.end(),
            [id](const Contact& c) { return c.id == id; });
        
        if (it != contacts.end()) {
            it->name = contact.name;
            it->phone = contact.phone;
            it->email = contact.email;
            it->address = contact.address;
            saveToFile();
            return true;
        }
        return false;
    }
    
    bool deleteContact(int id) {
        auto it = std::find_if(contacts.begin(), contacts.end(),
            [id](const Contact& c) { return c.id == id; });
        
        if (it != contacts.end()) {
            contacts.erase(it);
            saveToFile();
            return true;
        }
        return false;
    }
    
    Contact* getContact(int id) {
        auto it = std::find_if(contacts.begin(), contacts.end(),
            [id](const Contact& c) { return c.id == id; });
        
        if (it != contacts.end()) {
            return &(*it);
        }
        return nullptr;
    }
    
    std::vector<Contact> getAllContacts() const {
        return contacts;
    }
    
    std::vector<Contact> searchContacts(const std::string& query) const {
        std::vector<Contact> results;
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
        
        for (const auto& contact : contacts) {
            std::string lowerName = contact.name;
            std::string lowerPhone = contact.phone;
            std::string lowerEmail = contact.email;
            
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            std::transform(lowerPhone.begin(), lowerPhone.end(), lowerPhone.begin(), ::tolower);
            std::transform(lowerEmail.begin(), lowerEmail.end(), lowerEmail.begin(), ::tolower);
            
            if (lowerName.find(lowerQuery) != std::string::npos ||
                lowerPhone.find(lowerQuery) != std::string::npos ||
                lowerEmail.find(lowerQuery) != std::string::npos) {
                results.push_back(contact);
            }
        }
        return results;
    }
    
    int getContactCount() const {
        return contacts.size();
    }
    
    void clearAll() {
        contacts.clear();
        nextId = 1;
        saveToFile();
    }
    
    bool saveToFile() {
        QJsonArray jsonArray;
        for (const auto& contact : contacts) {
            jsonArray.append(contact.toJson());
        }
        
        QJsonObject root;
        root["contacts"] = jsonArray;
        root["nextId"] = nextId;
        
        QJsonDocument doc(root);
        
        QFile file(QString::fromStdString(filename));
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        
        file.write(doc.toJson());
        file.close();
        return true;
    }
    
    bool loadFromFile() {
        QFile file(QString::fromStdString(filename));
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }
        
        QByteArray data = file.readAll();
        file.close();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) {
            return false;
        }
        
        QJsonObject root = doc.object();
        QJsonArray jsonArray = root["contacts"].toArray();
        nextId = root["nextId"].toInt(1);
        
        contacts.clear();
        for (const auto& value : jsonArray) {
            if (value.isObject()) {
                contacts.push_back(Contact::fromJson(value.toObject()));
            }
        }
        
        return true;
    }
    
    bool exportToFile(const std::string& exportFile) {
        std::string oldFile = filename;
        filename = exportFile;
        bool result = saveToFile();
        filename = oldFile;
        return result;
    }
    
    bool importFromFile(const std::string& importFile) {
        std::string oldFile = filename;
        filename = importFile;
        bool result = loadFromFile();
        filename = oldFile;
        if (result) {
            saveToFile();
        }
        return result;
    }
};

class ContactDialog : public QDialog {
    Q_OBJECT
    
private:
    QLineEdit* nameEdit;
    QLineEdit* phoneEdit;
    QLineEdit* emailEdit;
    QLineEdit* addressEdit;
    Contact contact;
    bool isEditMode;
    
public:
    ContactDialog(QWidget* parent = nullptr, const Contact* editContact = nullptr)
        : QDialog(parent), isEditMode(editContact != nullptr) {
        
        setWindowTitle(isEditMode ? "Редактировать контакт" : "Добавить контакт");
        setMinimumWidth(400);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        
        QFormLayout* formLayout = new QFormLayout();
        
        nameEdit = new QLineEdit();
        phoneEdit = new QLineEdit();
        emailEdit = new QLineEdit();
        addressEdit = new QLineEdit();
        
        if (isEditMode) {
            contact = *editContact;
            nameEdit->setText(QString::fromStdString(contact.name));
            phoneEdit->setText(QString::fromStdString(contact.phone));
            emailEdit->setText(QString::fromStdString(contact.email));
            addressEdit->setText(QString::fromStdString(contact.address));
        }
        
        formLayout->addRow("Имя:", nameEdit);
        formLayout->addRow("Телефон:", phoneEdit);
        formLayout->addRow("Email:", emailEdit);
        formLayout->addRow("Адрес:", addressEdit);
        
        mainLayout->addLayout(formLayout);
        
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        
        QPushButton* saveButton = new QPushButton("Сохранить");
        saveButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; }");
        connect(saveButton, &QPushButton::clicked, this, &ContactDialog::onSave);
        
        QPushButton* cancelButton = new QPushButton("Отмена");
        cancelButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");
        connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        
        buttonLayout->addWidget(saveButton);
        buttonLayout->addWidget(cancelButton);
        
        mainLayout->addLayout(buttonLayout);
    }
    
    Contact getContact() const {
        return contact;
    }
    
private slots:
    void onSave() {
        if (nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Поле 'Имя' не может быть пустым!");
            return;
        }
        
        contact.name = nameEdit->text().toStdString();
        contact.phone = phoneEdit->text().toStdString();
        contact.email = emailEdit->text().toStdString();
        contact.address = addressEdit->text().toStdString();
        
        accept();
    }
};

class NotebookView : public QWidget {
    Q_OBJECT
    
private:
    NotebookService* service;
    QTableWidget* table;
    QLineEdit* searchEdit;
    QLabel* statusLabel;
    QMenuBar* menuBar;
    
public:
    NotebookView(QWidget* parent = nullptr) : QWidget(parent) {
        service = new NotebookService();
        
        setWindowTitle("Записная книжка");
        setMinimumSize(800, 600);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        
        createMenuBar();
        mainLayout->setMenuBar(menuBar);
        
        QWidget* centralWidget = new QWidget();
        QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
        centralLayout->setContentsMargins(10, 10, 10, 10);
        
        QLabel* titleLabel = new QLabel("<h2>Записная книжка контактов</h2>");
        titleLabel->setAlignment(Qt::AlignCenter);
        centralLayout->addWidget(titleLabel);
        
        QHBoxLayout* searchLayout = new QHBoxLayout();
        QLabel* searchLabel = new QLabel("Поиск:");
        searchEdit = new QLineEdit();
        searchEdit->setPlaceholderText("Введите имя, телефон или email...");
        connect(searchEdit, &QLineEdit::textChanged, this, &NotebookView::onSearch);
        
        searchLayout->addWidget(searchLabel);
        searchLayout->addWidget(searchEdit);
        centralLayout->addLayout(searchLayout);
        
        table = new QTableWidget();
        table->setColumnCount(5);
        table->setHorizontalHeaderLabels({"ID", "Имя", "Телефон", "Email", "Адрес"});
        table->horizontalHeader()->setStretchLastSection(true);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setAlternatingRowColors(true);
        table->verticalHeader()->setVisible(false);
        
        table->setColumnWidth(0, 50);
        table->setColumnWidth(1, 200);
        table->setColumnWidth(2, 150);
        table->setColumnWidth(3, 200);
        
        centralLayout->addWidget(table);
        
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        
        QPushButton* addButton = new QPushButton("Добавить контакт");
        addButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px; }");
        connect(addButton, &QPushButton::clicked, this, &NotebookView::onAddContact);
        
        QPushButton* editButton = new QPushButton("Редактировать");
        editButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 10px; }");
        connect(editButton, &QPushButton::clicked, this, &NotebookView::onEditContact);
        
        QPushButton* deleteButton = new QPushButton("Удалить");
        deleteButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 10px; }");
        connect(deleteButton, &QPushButton::clicked, this, &NotebookView::onDeleteContact);
        
        buttonLayout->addWidget(addButton);
        buttonLayout->addWidget(editButton);
        buttonLayout->addWidget(deleteButton);
        buttonLayout->addStretch();
        
        centralLayout->addLayout(buttonLayout);
        
        statusLabel = new QLabel();
        statusLabel->setStyleSheet("QLabel { padding: 5px; background-color: #e0e0e0; }");
        centralLayout->addWidget(statusLabel);
        
        mainLayout->addWidget(centralWidget);
        
        refreshTable();
        updateStatus();
    }
    
    ~NotebookView() {
        delete service;
    }
    
private:
    void createMenuBar() {
        menuBar = new QMenuBar(this);
        
        QMenu* fileMenu = menuBar->addMenu("Файл");
        
        QAction* importAction = new QAction("Импорт из файла", this);
        connect(importAction, &QAction::triggered, this, &NotebookView::onImport);
        fileMenu->addAction(importAction);
        
        QAction* exportAction = new QAction("Экспорт в файл", this);
        connect(exportAction, &QAction::triggered, this, &NotebookView::onExport);
        fileMenu->addAction(exportAction);
        
        fileMenu->addSeparator();
        
        QAction* clearAction = new QAction("Очистить все", this);
        connect(clearAction, &QAction::triggered, this, &NotebookView::onClearAll);
        fileMenu->addAction(clearAction);
        
        fileMenu->addSeparator();
        
        QAction* exitAction = new QAction("Выход", this);
        exitAction->setShortcut(QKeySequence("Ctrl+Q"));
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        fileMenu->addAction(exitAction);
        
        QMenu* helpMenu = menuBar->addMenu("Справка");
        
        QAction* aboutAction = new QAction("О программе", this);
        connect(aboutAction, &QAction::triggered, this, &NotebookView::onAbout);
        helpMenu->addAction(aboutAction);
    }
    
    void refreshTable(const std::vector<Contact>& contacts) {
        table->setRowCount(0);
        
        for (const auto& contact : contacts) {
            int row = table->rowCount();
            table->insertRow(row);
            
            table->setItem(row, 0, new QTableWidgetItem(QString::number(contact.id)));
            table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(contact.name)));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(contact.phone)));
            table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(contact.email)));
            table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(contact.address)));
        }
    }
    
    void refreshTable() {
        refreshTable(service->getAllContacts());
    }
    
    void updateStatus() {
        statusLabel->setText(QString("Всего контактов: %1").arg(service->getContactCount()));
    }
    
    int getSelectedContactId() {
        int row = table->currentRow();
        if (row >= 0) {
            return table->item(row, 0)->text().toInt();
        }
        return -1;
    }
    
private slots:
    void onAddContact() {
        ContactDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            Contact contact = dialog.getContact();
            if (service->addContact(contact)) {
                refreshTable();
                updateStatus();
                QMessageBox::information(this, "Успех", "Контакт успешно добавлен!");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось добавить контакт!");
            }
        }
    }
    
    void onEditContact() {
        int id = getSelectedContactId();
        if (id < 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите контакт для редактирования!");
            return;
        }
        
        Contact* contact = service->getContact(id);
        if (!contact) {
            QMessageBox::warning(this, "Ошибка", "Контакт не найден!");
            return;
        }
        
        ContactDialog dialog(this, contact);
        if (dialog.exec() == QDialog::Accepted) {
            Contact updatedContact = dialog.getContact();
            if (service->updateContact(id, updatedContact)) {
                refreshTable();
                QMessageBox::information(this, "Успех", "Контакт успешно обновлен!");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось обновить контакт!");
            }
        }
    }
    
    void onDeleteContact() {
        int id = getSelectedContactId();
        if (id < 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите контакт для удаления!");
            return;
        }
        
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
            "Вы уверены, что хотите удалить этот контакт?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            if (service->deleteContact(id)) {
                refreshTable();
                updateStatus();
                QMessageBox::information(this, "Успех", "Контакт успешно удален!");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось удалить контакт!");
            }
        }
    }
    
    void onSearch(const QString& text) {
        if (text.isEmpty()) {
            refreshTable();
        } else {
            std::vector<Contact> results = service->searchContacts(text.toStdString());
            refreshTable(results);
        }
    }
    
    void onImport() {
        QString filename = QFileDialog::getOpenFileName(this, "Импорт контактов",
            "", "JSON Files (*.json);;All Files (*)");
        
        if (!filename.isEmpty()) {
            if (service->importFromFile(filename.toStdString())) {
                refreshTable();
                updateStatus();
                QMessageBox::information(this, "Успех", "Контакты успешно импортированы!");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось импортировать контакты!");
            }
        }
    }
    
    void onExport() {
        QString filename = QFileDialog::getSaveFileName(this, "Экспорт контактов",
            "contacts_export.json", "JSON Files (*.json);;All Files (*)");
        
        if (!filename.isEmpty()) {
            if (service->exportToFile(filename.toStdString())) {
                QMessageBox::information(this, "Успех", "Контакты успешно экспортированы!");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось экспортировать контакты!");
            }
        }
    }
    
    void onClearAll() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
            "Вы уверены, что хотите удалить ВСЕ контакты?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            service->clearAll();
            refreshTable();
            updateStatus();
            QMessageBox::information(this, "Успех", "Все контакты удалены!");
        }
    }
    
    void onAbout() {
        QMessageBox::about(this, "О программе",
            "<h3>Записная книжка контактов</h3>"
            "<p>Версия 1.0</p>"
            "<p><b>Возможности:</b></p>"
            "<ul>"
            "<li>Добавление, редактирование и удаление контактов</li>"
            "<li>Поиск по имени, телефону и email</li>"
            "<li>Импорт и экспорт в JSON формате</li>"
            "<li>Автосохранение данных</li>"
            "</ul>"
            "<p>Разработано с использованием Qt5</p>");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    NotebookView window;
    window.show();
    
    return app.exec();
}

#include "lvl3proj4.moc"