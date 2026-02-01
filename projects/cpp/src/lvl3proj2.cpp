#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QTimer>
#include <QMessageBox>
#include <QGroupBox>
#include <QScrollArea>
#include <vector>
#include <QString>

struct Question {
    QString text;
    std::vector<QString> options;
    std::vector<int> correctAnswers;
    bool isMultipleChoice;
};

// Класс для теста
class Test {
public:
    QString title;
    std::vector<Question> questions;
    int timeLimit; // в секундах
    
    Test() {
        title = "Тест по программированию на C++";
        timeLimit = 180; // 3 минуты
        
        // Вопрос 1 - одиночный выбор
        Question q1;
        q1.text = "Какой заголовочный файл нужен для использования std::cout?";
        q1.options = {"<stdio.h>", "<iostream>", "<string>", "<vector>"};
        q1.correctAnswers = {1};
        q1.isMultipleChoice = false;
        questions.push_back(q1);
        
        // Вопрос 2 - множественный выбор
        Question q2;
        q2.text = "Какие из этих типов данных являются примитивными в C++?";
        q2.options = {"int", "string", "char", "vector", "double"};
        q2.correctAnswers = {0, 2, 4};
        q2.isMultipleChoice = true;
        questions.push_back(q2);
        
        // Вопрос 3 - одиночный выбор
        Question q3;
        q3.text = "Что делает ключевое слово 'const'?";
        q3.options = {
            "Создает константу",
            "Удаляет переменную",
            "Увеличивает скорость",
            "Ничего не делает"
        };
        q3.correctAnswers = {0};
        q3.isMultipleChoice = false;
        questions.push_back(q3);
        
        // Вопрос 4 - множественный выбор
        Question q4;
        q4.text = "Выберите контейнеры STL:";
        q4.options = {"vector", "array", "list", "cout", "map"};
        q4.correctAnswers = {0, 2, 4};
        q4.isMultipleChoice = true;
        questions.push_back(q4);
        
        // Вопрос 5 - одиночный выбор
        Question q5;
        q5.text = "Какой оператор используется для динамического выделения памяти?";
        q5.options = {"malloc", "new", "alloc", "create"};
        q5.correctAnswers = {1};
        q5.isMultipleChoice = false;
        questions.push_back(q5);
    }
};

// Главное окно приложения
class TestApplication : public QWidget {
    Q_OBJECT
    
private:
    QStackedWidget* stackedWidget;
    Test test;
    
    // Форма 1: Ввод данных
    QWidget* dataInputForm;
    QLineEdit* nameInput;
    QLineEdit* emailInput;
    QString userName;
    QString userEmail;
    
    // Форма 2: Тест
    QWidget* testForm;
    QLabel* timerLabel;
    QLabel* questionLabel;
    QLabel* questionNumberLabel;
    QTimer* timer;
    int timeRemaining;
    int currentQuestion;
    std::vector<std::vector<QRadioButton*>> radioButtons;
    std::vector<std::vector<QCheckBox*>> checkBoxes;
    std::vector<std::vector<int>> userAnswers;
    QPushButton* nextButton;
    QPushButton* prevButton;
    QScrollArea* scrollArea;
    
    // Форма 3: Результаты
    QWidget* resultForm;
    QLabel* resultLabel;
    
public:
    TestApplication(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("Тестирующая система");
        setMinimumSize(600, 500);
        
        stackedWidget = new QStackedWidget(this);
        
        createDataInputForm();
        createTestForm();
        createResultForm();
        
        stackedWidget->addWidget(dataInputForm);
        stackedWidget->addWidget(testForm);
        stackedWidget->addWidget(resultForm);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(stackedWidget);
        
        stackedWidget->setCurrentIndex(0);
    }
    
private:
    void createDataInputForm() {
        dataInputForm = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(dataInputForm);
        
        QLabel* titleLabel = new QLabel("<h2>Добро пожаловать в тестирующую систему!</h2>");
        titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(titleLabel);
        
        layout->addSpacing(20);
        
        QLabel* infoLabel = new QLabel("Пожалуйста, введите ваши данные:");
        layout->addWidget(infoLabel);
        
        layout->addSpacing(10);
        
        QLabel* nameLabel = new QLabel("Имя и фамилия:");
        layout->addWidget(nameLabel);
        nameInput = new QLineEdit();
        nameInput->setPlaceholderText("Введите ваше имя");
        layout->addWidget(nameInput);
        
        layout->addSpacing(10);
        
        QLabel* emailLabel = new QLabel("Email:");
        layout->addWidget(emailLabel);
        emailInput = new QLineEdit();
        emailInput->setPlaceholderText("example@email.com");
        layout->addWidget(emailInput);
        
        layout->addSpacing(20);
        
        QLabel* testInfoLabel = new QLabel(
            QString("<b>Информация о тесте:</b><br>"
                   "Тема: %1<br>"
                   "Количество вопросов: %2<br>"
                   "Время на прохождение: %3 минут")
                .arg(test.title)
                .arg(test.questions.size())
                .arg(test.timeLimit / 60)
        );
        layout->addWidget(testInfoLabel);
        
        layout->addStretch();
        
        QPushButton* startButton = new QPushButton("Начать тест");
        startButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px; font-size: 14px; }");
        connect(startButton, &QPushButton::clicked, this, &TestApplication::startTest);
        layout->addWidget(startButton);
    }
    
    void createTestForm() {
        testForm = new QWidget();
        QVBoxLayout* mainLayout = new QVBoxLayout(testForm);
        
        // Заголовок с таймером
        QHBoxLayout* headerLayout = new QHBoxLayout();
        QLabel* testTitleLabel = new QLabel("<h3>" + test.title + "</h3>");
        timerLabel = new QLabel("Время: 00:00");
        timerLabel->setStyleSheet("QLabel { color: red; font-size: 16px; font-weight: bold; }");
        headerLayout->addWidget(testTitleLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(timerLabel);
        mainLayout->addLayout(headerLayout);
        
        // Номер вопроса
        questionNumberLabel = new QLabel();
        mainLayout->addWidget(questionNumberLabel);
        
        // Область прокрутки для вопроса
        scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        mainLayout->addWidget(scrollArea);
        
        // Кнопки навигации
        QHBoxLayout* navLayout = new QHBoxLayout();
        prevButton = new QPushButton("← Предыдущий");
        nextButton = new QPushButton("Следующий →");
        
        prevButton->setStyleSheet("QPushButton { padding: 8px; }");
        nextButton->setStyleSheet("QPushButton { padding: 8px; background-color: #2196F3; color: white; }");
        
        connect(prevButton, &QPushButton::clicked, this, &TestApplication::previousQuestion);
        connect(nextButton, &QPushButton::clicked, this, &TestApplication::nextQuestion);
        
        navLayout->addWidget(prevButton);
        navLayout->addStretch();
        navLayout->addWidget(nextButton);
        mainLayout->addLayout(navLayout);
        
        // Инициализация таймера
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &TestApplication::updateTimer);
        
        // Инициализация структур для ответов
        userAnswers.resize(test.questions.size());
        radioButtons.resize(test.questions.size());
        checkBoxes.resize(test.questions.size());
    }
    
    void createResultForm() {
        resultForm = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(resultForm);
        
        QLabel* titleLabel = new QLabel("<h2>Результаты тестирования</h2>");
        titleLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(titleLabel);
        
        layout->addSpacing(20);
        
        resultLabel = new QLabel();
        resultLabel->setAlignment(Qt::AlignCenter);
        resultLabel->setWordWrap(true);
        layout->addWidget(resultLabel);
        
        layout->addStretch();
        
        QPushButton* restartButton = new QPushButton("Пройти тест заново");
        restartButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px; }");
        connect(restartButton, &QPushButton::clicked, this, &TestApplication::restartTest);
        layout->addWidget(restartButton);
    }
    
    void displayQuestion(int index) {
        QWidget* questionWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(questionWidget);
        
        questionNumberLabel->setText(QString("Вопрос %1 из %2").arg(index + 1).arg(test.questions.size()));
        
        Question& q = test.questions[index];
        
        questionLabel = new QLabel("<h4>" + q.text + "</h4>");
        questionLabel->setWordWrap(true);
        layout->addWidget(questionLabel);
        
        layout->addSpacing(10);
        
        if (q.isMultipleChoice) {
            QLabel* hintLabel = new QLabel("<i>(Можно выбрать несколько вариантов)</i>");
            hintLabel->setStyleSheet("QLabel { color: #666; }");
            layout->addWidget(hintLabel);
            layout->addSpacing(5);
        }
        
        if (!q.isMultipleChoice) {
            // Радиокнопки для одиночного выбора
            radioButtons[index].clear();
            for (size_t i = 0; i < q.options.size(); i++) {
                QRadioButton* radio = new QRadioButton(q.options[i]);
                radio->setStyleSheet("QRadioButton { padding: 5px; }");
                radioButtons[index].push_back(radio);
                layout->addWidget(radio);
                
                // Восстановление предыдущего ответа
                for (int ans : userAnswers[index]) {
                    if (ans == static_cast<int>(i)) {
                        radio->setChecked(true);
                    }
                }
            }
        } else {
            // Чекбоксы для множественного выбора
            checkBoxes[index].clear();
            for (size_t i = 0; i < q.options.size(); i++) {
                QCheckBox* checkBox = new QCheckBox(q.options[i]);
                checkBox->setStyleSheet("QCheckBox { padding: 5px; }");
                checkBoxes[index].push_back(checkBox);
                layout->addWidget(checkBox);
                
                // Восстановление предыдущих ответов
                for (int ans : userAnswers[index]) {
                    if (ans == static_cast<int>(i)) {
                        checkBox->setChecked(true);
                    }
                }
            }
        }
        
        layout->addStretch();
        
        scrollArea->setWidget(questionWidget);
        
        // Обновление кнопок
        prevButton->setEnabled(index > 0);
        if (index == static_cast<int>(test.questions.size()) - 1) {
            nextButton->setText("Завершить тест");
            nextButton->setStyleSheet("QPushButton { padding: 8px; background-color: #f44336; color: white; }");
        } else {
            nextButton->setText("Следующий →");
            nextButton->setStyleSheet("QPushButton { padding: 8px; background-color: #2196F3; color: white; }");
        }
    }
    
    void saveCurrentAnswer() {
        userAnswers[currentQuestion].clear();
        Question& q = test.questions[currentQuestion];
        
        if (!q.isMultipleChoice) {
            for (size_t i = 0; i < radioButtons[currentQuestion].size(); i++) {
                if (radioButtons[currentQuestion][i]->isChecked()) {
                    userAnswers[currentQuestion].push_back(i);
                    break;
                }
            }
        } else {
            for (size_t i = 0; i < checkBoxes[currentQuestion].size(); i++) {
                if (checkBoxes[currentQuestion][i]->isChecked()) {
                    userAnswers[currentQuestion].push_back(i);
                }
            }
        }
    }
    
    void calculateResults() {
        int correctAnswers = 0;
        QString detailedResults = "<h3>Детальные результаты:</h3><br>";
        
        for (size_t i = 0; i < test.questions.size(); i++) {
            Question& q = test.questions[i];
            std::vector<int>& correct = q.correctAnswers;
            std::vector<int>& user = userAnswers[i];
            
            std::sort(user.begin(), user.end());
            std::vector<int> sortedCorrect = correct;
            std::sort(sortedCorrect.begin(), sortedCorrect.end());
            
            bool isCorrect = (user == sortedCorrect);
            if (isCorrect) {
                correctAnswers++;
            }
            
            detailedResults += QString("<b>Вопрос %1:</b> %2<br>").arg(i + 1).arg(isCorrect ? "✓ Правильно" : "✗ Неправильно");
        }
        
        double percentage = (static_cast<double>(correctAnswers) / test.questions.size()) * 100;
        QString grade;
        
        if (percentage >= 90) grade = "Отлично!";
        else if (percentage >= 75) grade = "Хорошо";
        else if (percentage >= 60) grade = "Удовлетворительно";
        else grade = "Неудовлетворительно";
        
        resultLabel->setText(
            QString("<h3>%1</h3>"
                   "<p><b>Имя:</b> %2</p>"
                   "<p><b>Email:</b> %3</p>"
                   "<p><b>Правильных ответов:</b> %4 из %5</p>"
                   "<p><b>Процент:</b> %6%</p>"
                   "<p><b>Оценка:</b> %7</p>"
                   "<br>%8")
                .arg(test.title)
                .arg(userName)
                .arg(userEmail)
                .arg(correctAnswers)
                .arg(test.questions.size())
                .arg(QString::number(percentage, 'f', 1))
                .arg(grade)
                .arg(detailedResults)
        );
    }
    
private slots:
    void startTest() {
        userName = nameInput->text().trimmed();
        userEmail = emailInput->text().trimmed();
        
        if (userName.isEmpty() || userEmail.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля!");
            return;
        }
        
        currentQuestion = 0;
        timeRemaining = test.timeLimit;
        
        // Очистка предыдущих ответов
        for (auto& answers : userAnswers) {
            answers.clear();
        }
        
        displayQuestion(currentQuestion);
        stackedWidget->setCurrentIndex(1);
        
        timer->start(1000); 
    }
    
    void updateTimer() {
        timeRemaining--;
        
        int minutes = timeRemaining / 60;
        int seconds = timeRemaining % 60;
        
        timerLabel->setText(QString("Время: %1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0')));
        
        if (timeRemaining <= 30) {
            timerLabel->setStyleSheet("QLabel { color: red; font-size: 16px; font-weight: bold; }");
        }
        
        if (timeRemaining <= 0) {
            timer->stop();
            QMessageBox::information(this, "Время вышло", "Время на прохождение теста истекло!");
            finishTest();
        }
    }
    
    void nextQuestion() {
        saveCurrentAnswer();
        
        if (currentQuestion < static_cast<int>(test.questions.size()) - 1) {
            currentQuestion++;
            displayQuestion(currentQuestion);
        } else {
            finishTest();
        }
    }
    
    void previousQuestion() {
        if (currentQuestion > 0) {
            saveCurrentAnswer();
            currentQuestion--;
            displayQuestion(currentQuestion);
        }
    }
    
    void finishTest() {
        timer->stop();
        saveCurrentAnswer();
        calculateResults();
        stackedWidget->setCurrentIndex(2);
    }
    
    void restartTest() {
        nameInput->clear();
        emailInput->clear();
        stackedWidget->setCurrentIndex(0);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    TestApplication window;
    window.show();
    
    return app.exec();
}

#include "lvl3proj2.moc"