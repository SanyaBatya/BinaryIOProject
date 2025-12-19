#include "C:\Users\Bratan\source\repos\BinaryIOProject\StaticLibrary\BinarySerializer.h"
#include "C:\Users\Bratan\source\repos\BinaryIOProject\DynamicLibrary\BinaryFileHandler.h"
#include <iostream>
#include <memory>
using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");
    using namespace BinaryIO;

    cout << "Пример использования статической библиотеки" << endl;

    Person person(1, "Иван Иванов", 30, 50000.50);
    cout << "Исходный объект Person: ";
    person.printInfo();

    auto serializedData = person.serialize();
    cout << "Размер сериализованных данных: " << serializedData.size() << " байт" << endl;

    Person restoredPerson;
    if (restoredPerson.deserialize(serializedData)) {
        cout << "Восстановленный объект Person: ";
        restoredPerson.printInfo();
    }

    auto clonedPerson = person.clone();
    cout << "Клонированный объект Person: ";
    clonedPerson->printInfo();



    Product product("P001", "Ноутбук", 10, 999.99);
    cout << "\nИсходный объект Product: ";
    product.printInfo();

    auto productData = product.serialize();
    Product restoredProduct;
    if (restoredProduct.deserialize(productData)) {
        cout << "Восстановленный объект Product: ";
        restoredProduct.printInfo();
    }




    cout << "Пример использования динамической библиотеки" << endl;

    BinaryFileHandler::enableLogging(true);

    BinaryFileHandler fileHandler("data.bin");

    Person person1(1, "Анна Петрова", 25, 45000.75);
    Person person2(2, "Петр Сидоров", 35, 60000.25);

    if (fileHandler.openFile(true)) {
        fileHandler.writeObject(&person1);
        fileHandler.writeObject(&person2);
        fileHandler.closeFile();
    }

    cout << "\nЧтение объектов из файла:" << endl;
    if (fileHandler.openFile(false)) {
        Person readPerson1, readPerson2;

        if (fileHandler.readObject(&readPerson1)) {
            readPerson1.printInfo();
        }

        if (fileHandler.readObject(&readPerson2)) {
            readPerson2.printInfo();
        }

        fileHandler.closeFile();
    }

    cout << "\nИспользование BinaryFileManager:" << endl;
    BinaryFileManager* manager = BinaryFileManager::create();

    auto* handler1 = manager->addHandler("file1.bin");
    auto* handler2 = manager->addHandler("file2.bin");

    BinaryFileHandler anotherHandler("singleton.bin");
    if (anotherHandler.openFile(true)) {
        Product product("P100", "Смартфон", 5, 29999.99);
        anotherHandler.writeObject(&product);
        anotherHandler.closeFile();
    }

    cout << "Количество обработчиков в менеджере: "
        << manager->getHandlerCount() << endl;

    delete manager;

    cout << "Пример завершен успешно!" << endl;

    return 0;
}