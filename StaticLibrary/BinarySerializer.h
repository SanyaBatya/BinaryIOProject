#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <memory>
#include <iostream>
using namespace std;

namespace BinaryIO {

    class SerializableObject {
    public:
        virtual ~SerializableObject() = default;
        virtual vector<uint8_t> serialize() const = 0;
        virtual bool deserialize(const vector<uint8_t>& data) = 0;
        virtual void printInfo() const = 0;
        virtual unique_ptr<SerializableObject> clone() const = 0;
    };

    class Person : public SerializableObject {
    private:
        int id;
        string name;
        int age;
        double salary;
    public:
        Person(int id = 0, string name = "", int age = 0, double salary = 0.0);

        int getId() const { return id; }
        string getName() const { return name; }
        int getAge() const { return age; }
        double getSalary() const { return salary; }

        void setName(const string& newName) { name = newName; }
        void setAge(int newAge) { age = newAge; }
        void setSalary(double newSalary) { salary = newSalary; }

        vector<uint8_t> serialize() const override;
        bool deserialize(const vector<uint8_t>& data) override;
        void printInfo() const override;
        unique_ptr<SerializableObject> clone() const override;
    };

    class Product : public SerializableObject {
    private:
        string code;
        string description;
        int quantity;
        double price;
    public:
        Product(string code = "", string desc = "", int qty = 0, double price = 0.0);

        vector<uint8_t> serialize() const override;
        bool deserialize(const std::vector<uint8_t>& data) override;
        void printInfo() const override;
        unique_ptr<SerializableObject> clone() const override;

        double getTotalValue() const { return quantity * price; }
    };

    class Serializer {
    public:
        static vector<uint8_t> serializeInt(int value);
        static vector<uint8_t> serializeDouble(double value);
        static vector<uint8_t> serializeString(const string& str);

        static int deserializeInt(const vector<uint8_t>& data, size_t& offset);
        static double deserializeDouble(const vector<uint8_t>& data, size_t& offset);
        static string deserializeString(const vector<uint8_t>& data, size_t& offset);

        static vector<uint8_t> addChecksum(const vector<uint8_t>& data);
        static bool verifyChecksum(const vector<uint8_t>& data);
    };
}