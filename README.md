# db_sqllite3

Класс для работы с SQLite базой данных на с++.
```c++
Model_Devices modelDevices;
auto recordDevices = modelDevices.where("title", "ccnet")->get();
```
