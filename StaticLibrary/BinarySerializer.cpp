#include "BinarySerializer.h"
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <iostream>
using namespace std;

namespace BinaryIO {

    Person::Person(int id, std::string name, int age, double salary) : id(id), name(name), age(age), salary(salary) {}

    vector<uint8_t> Person::serialize() const {

        vector<uint8_t> result;

        auto idData = Serializer::serializeInt(id);
        result.insert(result.end(), idData.begin(), idData.end());

        auto nameData = Serializer::serializeString(name);
        result.insert(result.end(), nameData.begin(), nameData.end());

        auto ageData = Serializer::serializeInt(age);
        result.insert(result.end(), ageData.begin(), ageData.end());

        auto salaryData = Serializer::serializeDouble(salary);
        result.insert(result.end(), salaryData.begin(), salaryData.end());

        return Serializer::addChecksum(result);
    }

    bool Person::deserialize(const vector<uint8_t>& data) {

        if (!Serializer::verifyChecksum(data)) {
            cerr << "Ошибка проверки контрольной суммы!" << endl;
            return false;
        }

        vector<uint8_t> cleanData(data.begin(), data.end() - 4);

        try {
            size_t offset = 0;

            id = Serializer::deserializeInt(cleanData, offset);
            name = Serializer::deserializeString(cleanData, offset);
            age = Serializer::deserializeInt(cleanData, offset);
            salary = Serializer::deserializeDouble(cleanData, offset);

            return true;
        }
        catch (const exception& e) {
            cerr << "Ошибка десериализации Person: " << e.what() << endl;
            return false;
        }
    }

    void Person::printInfo() const {
        cout << "Person [ID: " << id << ", Name: " << name << ", Age: " << age << ", Salary: " << salary << "]" << endl;
    }

    unique_ptr<SerializableObject> Person::clone() const {
        return make_unique<Person>(id, name, age, salary);
    }

    Product::Product(string code, string desc, int qty, double price) : code(code), description(desc), quantity(qty), price(price) {}

    vector<uint8_t> Product::serialize() const {

        vector<uint8_t> result;

        auto codeData = Serializer::serializeString(code);
        result.insert(result.end(), codeData.begin(), codeData.end());

        auto descData = Serializer::serializeString(description);
        result.insert(result.end(), descData.begin(), descData.end());

        auto qtyData = Serializer::serializeInt(quantity);
        result.insert(result.end(), qtyData.begin(), qtyData.end());

        auto priceData = Serializer::serializeDouble(price);
        result.insert(result.end(), priceData.begin(), priceData.end());

        return Serializer::addChecksum(result);
    }

    bool Product::deserialize(const vector<uint8_t>& data) {

        if (!Serializer::verifyChecksum(data)) {
            return false;
        }

        vector<uint8_t> cleanData(data.begin(), data.end() - 4);

        try {
            size_t offset = 0;

            code = Serializer::deserializeString(cleanData, offset);
            description = Serializer::deserializeString(cleanData, offset);
            quantity = Serializer::deserializeInt(cleanData, offset);
            price = Serializer::deserializeDouble(cleanData, offset);

            return true;
        }
        catch (const exception& e) {
            cerr << "Ошибка десериализации Product: " << e.what() << endl;
            return false;
        }
    }

    void Product::printInfo() const {
        cout << "Product [Code: " << code << ", Desc: " << description << ", Qty: " << quantity << ", Price: " << price << ", Total: " << getTotalValue() << "]" << endl;
    }

    unique_ptr<SerializableObject> Product::clone() const {
        return make_unique<Product>(code, description, quantity, price);
    }

    vector<uint8_t> Serializer::serializeInt(int value) {
        vector<uint8_t> result(4);

        // Оптимизированная реализация с memcpy
        memcpy(result.data(), &value, sizeof(value));

        return result;
    }

    vector<uint8_t> Serializer::serializeDouble(double value) {
        vector<uint8_t> result(8);
        memcpy(result.data(), &value, 8);

        return result;
    }

    vector<uint8_t> Serializer::serializeString(const string& str) {
        vector<uint8_t> result;
        auto lengthData = serializeInt(static_cast<int>(str.length()));
        result.insert(result.end(), lengthData.begin(), lengthData.end());
        result.insert(result.end(), str.begin(), str.end());

        return result;
    }

    int Serializer::deserializeInt(const vector<uint8_t>& data, size_t& offset) {

        if (offset + 4 > data.size()) {
            throw runtime_error("Недостаточно данных для десериализации int");
        }

        int value = 0;
        for (int i = 0; i < 4; i++) {
            value |= (data[offset + i] << (i * 8));
        }
        offset += 4;

        return value;
    }

    double Serializer::deserializeDouble(const vector<uint8_t>& data, size_t& offset) {

        if (offset + 8 > data.size()) {
            throw runtime_error("Недостаточно данных для десериализации double");
        }

        double value;
        memcpy(&value, data.data() + offset, 8);
        offset += 8;

        return value;
    }

    string Serializer::deserializeString(const vector<uint8_t>& data, size_t& offset) {
        int length = deserializeInt(data, offset);

        if (offset + length > data.size()) {
            throw std::runtime_error("Недостаточно данных для десериализации string");
        }

        string result(data.begin() + offset, data.begin() + offset + length);
        offset += length;

        return result;
    }

    vector<uint8_t> Serializer::addChecksum(const vector<uint8_t>& data) {
        vector<uint8_t> result = data;
        uint32_t checksum = 0;

        for (uint8_t byte : data) {
            checksum += byte;
        }

        for (int i = 0; i < 4; i++) {
            result.push_back((checksum >> (i * 8)) & 0xFF);
        }

        return result;
    }

    bool Serializer::verifyChecksum(const vector<uint8_t>& data) {
        if (data.size() < 4) return false;

        uint32_t storedChecksum = 0;
        for (int i = 0; i < 4; i++) {
            storedChecksum |= (data[data.size() - 4 + i] << (i * 8));
        }

        uint32_t calculatedChecksum = 0;
        for (size_t i = 0; i < data.size() - 4; i++) {
            calculatedChecksum += data[i];
        }

        return storedChecksum == calculatedChecksum;
    }
}