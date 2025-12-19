#include "C:\Users\Bratan\source\repos\BinaryIOProject\DynamicLibrary\BinaryFileHandler.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>
using namespace std;

namespace BinaryIO {
    class BinaryFileHandler::FileStreamImpl {
    private:
        fstream m_stream;
    public:
        bool open(const char* filename, ios::openmode mode) {
            if (m_stream.is_open()) {
                m_stream.close();
            }
            m_stream.open(filename, mode);
            return m_stream.is_open();
        }

        void close() {
            if (m_stream.is_open()) {
                m_stream.close();
            }
        }

        bool isOpen() const {
            return m_stream.is_open();
        }

        bool write(const void* data, size_t size) {
            if (!m_stream.is_open()) return false;
            m_stream.write(reinterpret_cast<const char*>(data), size);
            return m_stream.good();
        }

        bool read(void* data, size_t size) {
            if (!m_stream.is_open()) return false;
            m_stream.read(reinterpret_cast<char*>(data), size);
            return m_stream.good();
        }

        int peek() {
            return m_stream.peek();
        }

        operator bool() const {
            return m_stream.good();
        }
    };

    bool BinaryFileHandler::loggingEnabled = true;

    BinaryFileHandler::BinaryFileHandler(const char* filename) : m_filename(nullptr), m_fileStream(new FileStreamImpl()) {
        copyFilename(filename);
        log("Создан BinaryFileHandler");
    }

    BinaryFileHandler::~BinaryFileHandler() {
        log("BinaryFileHandler уничтожен");
        clear();
    }

    BinaryFileHandler::BinaryFileHandler(BinaryFileHandler&& other) noexcept : m_filename(other.m_filename), m_fileStream(other.m_fileStream) {
        other.m_filename = nullptr;
        other.m_fileStream = nullptr;
    }

    BinaryFileHandler& BinaryFileHandler::operator=(BinaryFileHandler&& other) noexcept {
        if (this != &other) {
            clear();
            m_filename = other.m_filename;
            m_fileStream = other.m_fileStream;
            other.m_filename = nullptr;
            other.m_fileStream = nullptr;
        }
        return *this;
    }

    void BinaryFileHandler::copyFilename(const char* filename) {
        if (m_filename) {
            delete[] m_filename;
        }

        if (filename) {
            size_t len = strlen(filename) + 1;
            m_filename = new char[len];
            strcpy_s(m_filename, len, filename);
        }
        else {
            m_filename = nullptr;
        }
    }

    void BinaryFileHandler::clear() {
        if (m_filename) {
            delete[] m_filename;
            m_filename = nullptr;
        }

        if (m_fileStream) {
            delete m_fileStream;
            m_fileStream = nullptr;
        }
    }

    bool BinaryFileHandler::openFile(bool forWriting) {
        if (!m_filename) {
            return false;
        }

        ios::openmode mode = ios::binary;
        if (forWriting) {
            mode |= ios::out | ios::trunc;
        }
        else {
            mode |= ios::in;
        }

        bool success = m_fileStream->open(m_filename, mode);

        if (success) {
            log("Файл успешно открыт");
        }
        else {
            log("Ошибка открытия файла");
        }

        return success;
    }

    void BinaryFileHandler::closeFile() {
        m_fileStream->close();
        log("Файл закрыт");
    }

    bool BinaryFileHandler::writeObject(const SerializableObject* object) {
        if (!object) {
            return false;
        }

        if (!m_fileStream->isOpen()) {
            if (!openFile(true)) {
                return false;
            }
        }

        try {
            auto data = object->serialize();
            uint32_t size = static_cast<uint32_t>(data.size());

            if (!m_fileStream->write(&size, sizeof(size))) {
                log("Ошибка записи размера");
                return false;
            }

            if (!m_fileStream->write(data.data(), size)) {
                log("Ошибка записи данных");
                return false;
            }

            log("Объект успешно записан");
            return true;
        }
        catch (const exception& e) {
            log("Исключение при записи объекта");
            return false;
        }
    }

    bool BinaryFileHandler::readObject(SerializableObject* object) {
        if (!object) {
            return false;
        }

        if (!m_fileStream->isOpen()) {
            if (!openFile(false)) {
                return false;
            }
        }

        try {
            uint32_t size = 0;

            if (!m_fileStream->read(&size, sizeof(size))) {
                return false;
            }

            if (size == 0) {
                return false;
            }

            vector<uint8_t> data(size);
            if (!m_fileStream->read(data.data(), size)) {
                log("Ошибка чтения данных");
                return false;
            }

            bool success = object->deserialize(data);
            if (success) {
                log("Объект успешно прочитан");
            }

            return success;
        }
        catch (const exception& e) {
            log("Исключение при чтении объекта");
            return false;
        }
    }

    const char* BinaryFileHandler::getFilename() const {
        return m_filename ? m_filename : "";
    }

    void BinaryFileHandler::log(const char* message) const {
        if (loggingEnabled) {
            cout << "[BinaryFileHandler] ";
            if (m_filename) {
                cout << m_filename << ": ";
            }
            cout << message << endl;
        }
    }

    BinaryFileManager::BinaryFileManager(size_t initialCapacity) : m_capacity(initialCapacity > 0 ? initialCapacity : 10), m_count(0) {
        m_handlers = new BinaryFileHandler * [m_capacity];
        for (size_t i = 0; i < m_capacity; i++) {
            m_handlers[i] = nullptr;
        }
    }

    BinaryFileManager::~BinaryFileManager() {
        clear();
        delete[] m_handlers;
    }

    BinaryFileHandler* BinaryFileManager::addHandler(const char* filename) {
        if (!filename || m_count >= m_capacity) {
            return nullptr;
        }

        for (size_t i = 0; i < m_count; i++) {
            if (m_handlers[i] &&
                strcmp(m_handlers[i]->getFilename(), filename) == 0) {
                return m_handlers[i];
            }
        }

        m_handlers[m_count] = new BinaryFileHandler(filename);
        return m_handlers[m_count++];
    }

    bool BinaryFileManager::removeHandler(size_t index) {
        if (index >= m_count || !m_handlers[index]) {
            return false;
        }

        delete m_handlers[index];

        for (size_t i = index; i < m_count - 1; i++) {
            m_handlers[i] = m_handlers[i + 1];
        }

        m_handlers[m_count - 1] = nullptr;
        m_count--;

        return true;
    }

    BinaryFileHandler* BinaryFileManager::getHandler(size_t index) const {
        return (index < m_count) ? m_handlers[index] : nullptr;
    }

    void BinaryFileManager::clear() {
        for (size_t i = 0; i < m_count; i++) {
            if (m_handlers[i]) {
                delete m_handlers[i];
                m_handlers[i] = nullptr;
            }
        }
        m_count = 0;
    }

    BinaryFileManager* BinaryFileManager::create() {
        return new BinaryFileManager();
    }

    bool BinaryFileManager::convertFile(const char* source, const char* destination) {
        if (!source || !destination) {
            return false;
        }

        BinaryFileHandler sourceHandler(source);
        BinaryFileHandler destHandler(destination);

        if (!sourceHandler.openFile(false)) {
            return false;
        }

        if (!destHandler.openFile(true)) {
            return false;
        }

        return true;
    }
}