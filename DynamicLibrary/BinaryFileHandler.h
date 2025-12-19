#pragma once
#include "C:\Users\Bratan\source\repos\BinaryIOProject\StaticLibrary\BinarySerializer.h"

#ifdef DYNAMICLIBRARY_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

namespace BinaryIO {
    class DLL_API BinaryFileHandler {
    private:
        char* m_filename;

        class FileStreamImpl;
        FileStreamImpl* m_fileStream;

        static bool loggingEnabled;

        BinaryFileHandler(const BinaryFileHandler&) = delete;
        BinaryFileHandler& operator=(const BinaryFileHandler&) = delete;

    public:
        explicit BinaryFileHandler(const char* filename);
        virtual ~BinaryFileHandler();
        BinaryFileHandler(BinaryFileHandler&& other) noexcept;
        BinaryFileHandler& operator=(BinaryFileHandler&& other) noexcept;

        bool openFile(bool forWriting = true);
        void closeFile();
        bool writeObject(const SerializableObject* object);
        bool readObject(SerializableObject* object);

        static void enableLogging(bool enable) { loggingEnabled = enable; }

        const char* getFilename() const;
    private:
        void log(const char* message) const;

        void copyFilename(const char* filename);
        void clear();
    };

    class DLL_API BinaryFileManager {
    private:
        BinaryFileHandler** m_handlers;
        size_t m_capacity;
        size_t m_count;

        BinaryFileManager(const BinaryFileManager&) = delete;
        BinaryFileManager& operator=(const BinaryFileManager&) = delete;
    public:
        explicit BinaryFileManager(size_t initialCapacity = 10);

        ~BinaryFileManager();

        BinaryFileHandler* addHandler(const char* filename);

        bool removeHandler(size_t index);

        BinaryFileHandler* getHandler(size_t index) const;

        size_t getHandlerCount() const { return m_count; }

        void clear();

        static BinaryFileManager* create();

        static bool convertFile(const char* source, const char* destination);
    };
}