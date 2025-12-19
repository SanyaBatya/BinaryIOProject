# BinaryIOProject

Статическая и динамическая библиотеки для работы с бинарными данными на C++.

## Структура проекта

- `StaticLibrary/` - Статическая библиотека для сериализации
- `DynamicLibrary/` - Динамическая библиотека для работы с файлами
- `Examples/` - Примеры использования

## Требования

- Visual Studio 2019 или выше
- Git для управления версиями

## Компиляция

### Статическая библиотека:
```bash
cd StaticLibrary
cl /c BinarySerializer.cpp /Fostatic.lib

### Динамическая библиотека:
```bash
cd DynamicLibrary
cl /c BinaryFileHandler.cpp /DDYNAMICLIBRARY_EXPORTS /Fodynamic.obj
link /DLL dynamic.obj /OUT:BinaryFileHandler.dll
