[tests.c](./tests.c) - тесты

[original.c](./original.c) - тестируемый код

[struct_and_func.h](./struct_and_func.h) - объявление функции и структуры

[CMakeLists.txt](./CMakeLists.txt)
для сборки ввести

```bash
mkdir build && cd build
cmake ..
make
```

для запуска тестов ввести
```bash
ctest