#include <iostream>
#include <string>
#include <ctime>
#include <stack>
#include <iomanip>

using namespace std;

struct Task {
    int id;
    string description;
    string priority;
    bool completed;
    time_t createdAt;
    Task* next;
    Task* prev;
    
    Task(int taskId, const string& desc, const string& prior = "Medium") 
        : id(taskId), description(desc), priority(prior), 
          completed(false), next(nullptr), prev(nullptr) {
        createdAt = time(nullptr);
    }
};

struct Action {
    enum Type { ADD, REMOVE, COMPLETE, UNCOMPLETE };
    Type type;
    Task* task;
    
    Action(Type t, Task* originalTask) : type(t) {
        task = new Task(originalTask->id, originalTask->description, originalTask->priority);
        task->completed = originalTask->completed;
        task->createdAt = originalTask->createdAt;
    }
    
    ~Action() {
        delete task;
    }
};

class ToDoList {
private:
    Task* head;
    Task* tail;
    int taskCounter;
    stack<Action*> history;
    
public:
    ToDoList() : head(nullptr), tail(nullptr), taskCounter(0) {}
    
    ~ToDoList() {
        clear();
        while (!history.empty()) {
            delete history.top();
            history.pop();
        }
    }
    
    void addToBeginning() {
        string desc, priority;
        cin.ignore();
        
        cout << "\nВведите описание задачи: ";
        getline(cin, desc);
        
        cout << "Приоритет (Low/Medium/High) [Medium]: ";
        getline(cin, priority);
        if (priority.empty()) priority = "Medium";
        
        Task* newTask = new Task(++taskCounter, desc, priority);
        
        if (head == nullptr) {
            head = tail = newTask;
        } else {
            newTask->next = head;
            head->prev = newTask;
            head = newTask;
        }
        
        history.push(new Action(Action::ADD, newTask));
        cout << "✓ Задача #" << newTask->id << " добавлена в начало списка" << endl;
    }
    
    void addToEnd() {
        string desc, priority;
        cin.ignore();
        
        cout << "\nВведите описание задачи: ";
        getline(cin, desc);
        
        cout << "Приоритет (Low/Medium/High) [Medium]: ";
        getline(cin, priority);
        if (priority.empty()) priority = "Medium";
        
        Task* newTask = new Task(++taskCounter, desc, priority);
        
        if (tail == nullptr) {
            head = tail = newTask;
        } else {
            tail->next = newTask;
            newTask->prev = tail;
            tail = newTask;
        }
        
        history.push(new Action(Action::ADD, newTask));
        cout << "✓ Задача #" << newTask->id << " добавлена в конец списка" << endl;
    }
    
    void removeTask() {
        if (head == nullptr) {
            cout << "\nСписок пуст!" << endl;
            return;
        }
        
        displayAll();
        
        int id;
        cout << "\nВведите ID задачи для удаления: ";
        cin >> id;
        
        Task* current = head;
        while (current != nullptr && current->id != id) {
            current = current->next;
        }
        
        if (current == nullptr) {
            cout << "Задача с ID " << id << " не найдена!" << endl;
            return;
        }
        
        history.push(new Action(Action::REMOVE, current));
        
        if (current == head && current == tail) {
            head = tail = nullptr;
        } else if (current == head) {
            head = head->next;
            head->prev = nullptr;
        } else if (current == tail) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {
            current->prev->next = current->next;
            current->next->prev = current->prev;
        }
        
        cout << "✓ Задача #" << id << " удалена" << endl;
        delete current;
    }
    
    void toggleComplete() {
        if (head == nullptr) {
            cout << "\nСписок пуст!" << endl;
            return;
        }
        
        displayAll();
        
        int id;
        cout << "\nВведите ID задачи: ";
        cin >> id;
        
        Task* current = head;
        while (current != nullptr && current->id != id) {
            current = current->next;
        }
        
        if (current == nullptr) {
            cout << "Задача с ID " << id << " не найдена!" << endl;
            return;
        }
        
        if (current->completed) {
            history.push(new Action(Action::UNCOMPLETE, current));
            current->completed = false;
            cout << "✓ Задача #" << id << " отмечена как невыполненная" << endl;
        } else {
            history.push(new Action(Action::COMPLETE, current));
            current->completed = true;
            cout << "✓ Задача #" << id << " отмечена как выполненная" << endl;
        }
    }
    
    void displayAll() {
        if (head == nullptr) {
            cout << "\nСписок задач пуст!" << endl;
            return;
        }
        
        cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
        cout << "║                      СПИСОК ЗАДАЧ                              ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
        
        Task* current = head;
        while (current != nullptr) {
            displayTask(current);
            current = current->next;
        }
    }
    
    void displayActive() {
        if (head == nullptr) {
            cout << "\nСписок задач пуст!" << endl;
            return;
        }
        
        cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
        cout << "║                   АКТИВНЫЕ ЗАДАЧИ                              ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
        
        bool found = false;
        Task* current = head;
        while (current != nullptr) {
            if (!current->completed) {
                displayTask(current);
                found = true;
            }
            current = current->next;
        }
        
        if (!found) {
            cout << "Нет активных задач!" << endl;
        }
    }
    
    void displayCompleted() {
        if (head == nullptr) {
            cout << "\nСписок задач пуст!" << endl;
            return;
        }
        
        cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
        cout << "║                 ВЫПОЛНЕННЫЕ ЗАДАЧИ                             ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
        
        bool found = false;
        Task* current = head;
        while (current != nullptr) {
            if (current->completed) {
                displayTask(current);
                found = true;
            }
            current = current->next;
        }
        
        if (!found) {
            cout << "Нет выполненных задач!" << endl;
        }
    }
    
    void undo() {
        if (history.empty()) {
            cout << "\nНечего отменять!" << endl;
            return;
        }
        
        Action* lastAction = history.top();
        history.pop();
        
        switch (lastAction->type) {
            case Action::ADD: {
                Task* current = head;
                while (current != nullptr && current->id != lastAction->task->id) {
                    current = current->next;
                }
                
                if (current != nullptr) {
                    if (current == head && current == tail) {
                        head = tail = nullptr;
                    } else if (current == head) {
                        head = head->next;
                        head->prev = nullptr;
                    } else if (current == tail) {
                        tail = tail->prev;
                        tail->next = nullptr;
                    } else {
                        current->prev->next = current->next;
                        current->next->prev = current->prev;
                    }
                    delete current;
                    cout << "✓ Отменено: добавление задачи #" << lastAction->task->id << endl;
                }
                break;
            }
            
            case Action::REMOVE: {
                Task* restored = new Task(lastAction->task->id, 
                                         lastAction->task->description, 
                                         lastAction->task->priority);
                restored->completed = lastAction->task->completed;
                restored->createdAt = lastAction->task->createdAt;
                
                if (head == nullptr) {
                    head = tail = restored;
                } else {
                    tail->next = restored;
                    restored->prev = tail;
                    tail = restored;
                }
                
                cout << "✓ Отменено: удаление задачи #" << restored->id << endl;
                break;
            }
            
            case Action::COMPLETE: {
                Task* current = head;
                while (current != nullptr && current->id != lastAction->task->id) {
                    current = current->next;
                }
                
                if (current != nullptr) {
                    current->completed = false;
                    cout << "✓ Отменено: завершение задачи #" << current->id << endl;
                }
                break;
            }
            
            case Action::UNCOMPLETE: {
                Task* current = head;
                while (current != nullptr && current->id != lastAction->task->id) {
                    current = current->next;
                }
                
                if (current != nullptr) {
                    current->completed = true;
                    cout << "✓ Отменено: снятие отметки выполнения задачи #" << current->id << endl;
                }
                break;
            }
        }
        
        delete lastAction;
    }
    
    void displayStatistics() {
        int total = 0, completed = 0, active = 0;
        int highPriority = 0, mediumPriority = 0, lowPriority = 0;
        
        Task* current = head;
        while (current != nullptr) {
            total++;
            if (current->completed) completed++;
            else active++;
            
            if (current->priority == "High") highPriority++;
            else if (current->priority == "Medium") mediumPriority++;
            else if (current->priority == "Low") lowPriority++;
            
            current = current->next;
        }
        
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║          СТАТИСТИКА                   ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "Всего задач:       " << total << endl;
        cout << "Активных:          " << active << endl;
        cout << "Выполнено:         " << completed << endl;
        
        if (total > 0) {
            cout << "Процент выполнения: " << fixed << setprecision(1) 
                 << (double)completed / total * 100 << "%" << endl;
        }
        
        cout << "\nПо приоритетам:" << endl;
        cout << "  High:   " << highPriority << endl;
        cout << "  Medium: " << mediumPriority << endl;
        cout << "  Low:    " << lowPriority << endl;
        
        cout << "\nДействий в истории: " << history.size() << endl;
    }
    
private:
    void displayTask(Task* task) {
        cout << "\n[" << task->id << "] ";
        cout << (task->completed ? "✓ " : "☐ ");
        cout << task->description << endl;
        
        cout << "    Приоритет: ";
        if (task->priority == "High") cout << "🔴 High";
        else if (task->priority == "Medium") cout << "🟡 Medium";
        else cout << "🟢 Low";
        
        char timeStr[80];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M", localtime(&task->createdAt));
        cout << " | Создана: " << timeStr << endl;
    }
    
    void clear() {
        while (head != nullptr) {
            Task* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
    }
};

void displayMenu() {
    cout << "\n╔════════════════════════════════════════╗" << endl;
    cout << "║         TO-DO LIST                    ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "1.  Добавить задачу в начало" << endl;
    cout << "2.  Добавить задачу в конец" << endl;
    cout << "3.  Удалить задачу" << endl;
    cout << "4.  Отметить выполнение/отменить" << endl;
    cout << "5.  Показать все задачи" << endl;
    cout << "6.  Показать активные задачи" << endl;
    cout << "7.  Показать выполненные задачи" << endl;
    cout << "8.  Отменить последнее действие (Undo)" << endl;
    cout << "9.  Статистика" << endl;
    cout << "0.  Выход" << endl;
    cout << "───────────────────────────────────────" << endl;
    cout << "Выберите действие: ";
}

int main() {
    system("chcp 65001 > nul");
    
    ToDoList todoList;
    int choice;
    
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                todoList.addToBeginning();
                break;
            case 2:
                todoList.addToEnd();
                break;
            case 3:
                todoList.removeTask();
                break;
            case 4:
                todoList.toggleComplete();
                break;
            case 5:
                todoList.displayAll();
                break;
            case 6:
                todoList.displayActive();
                break;
            case 7:
                todoList.displayCompleted();
                break;
            case 8:
                todoList.undo();
                break;
            case 9:
                todoList.displayStatistics();
                break;
            case 0:
                cout << "\nДо свидания!" << endl;
                break;
            default:
                cout << "Неверный выбор!" << endl;
        }
        
    } while (choice != 0);
    
    return 0;
}