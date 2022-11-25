#ifndef LIBS_DB_H
#define LIBS_DB_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <exception>
#include <sqlite3.h>

typedef std::vector<std::map<std::string, std::string>> result_query;
typedef std::map<std::string, std::string> result_record;

/**
 * @brief Класс для работы с базой данных SQLite3
 *
 */
class DB
{
    //--- Указатель на базу
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *zErrMsg = 0;

    //--- Название таблицы в запросе
    std::string table_name = "";

    //--- Условие в запросе
    std::string where_string = "";

    //--- Выборка полей в запросе
    std::string value_name = "*";

protected:
    int rc;

public:
    //--- Результат запроса
    result_query result;

    static DB *getInstance();

    int callback(int argc, char **argv, char **azColName);

    DB(std::string fileName);
    ~DB();

    DB *table(std::string _table_name);
    DB *where(std::string param, std::string value);

    std::string prepare();
    std::string value(std::string _value_name);
    result_query get();

    result_query query(std::string sql);

    // Эксепшн класса
    class DBException : public std::exception
    {
        std::string m_error;
        std::string str;
        int code;

    public:
        DBException() : code(0), m_error("")
        {
            str = m_error + std::string(": ") + std::to_string(code);
        };

        DBException(std::string error) : m_error(error), code(0)
        {
            str = m_error + std::string(": ") + std::to_string(code);
        };

        DBException(std::string error, int _code) : code(_code), m_error(error)
        {
            str = m_error + std::string(": ") + std::to_string(code);
        };

        int getCode()
        {
            return code;
        };

        const char *what() const noexcept override
        {
            return str.c_str();
        };
    };
};

/**
 * @brief
 *
 * @param param - указатель на текущий экземпляр DB
 * @param argc - количество полей
 * @param argv - значение полей
 * @param azColName - заголовки полей
 * @return int
 */
static int c_callback(void *param, int argc, char **argv, char **azColName)
{
    DB *db = reinterpret_cast<DB *>(param);
    return db->callback(argc, argv, azColName);
}

/**
 * @brief Construct a new DB::DB object
 *
 * @param fileName - path to file with database
 */
DB::DB(std::string fileName)
{
    rc = sqlite3_open(fileName.c_str(), &db);
    if (rc)
    {
        throw DBException("Cannot find db file: " + fileName);
        // TODO migrate DB
    }
}

/**
 * @brief Destroy the DB::DB object
 *
 */
DB::~DB()
{
    sqlite3_close(db);
}

DB *DB::getInstance()
{
    static DB instance("controller.db");
    return &instance;
}

/**
 * @brief Функция формирования результата
 *
 * @param argc
 * @param argv
 * @param azColName
 * @return int
 */
int DB::callback(int argc, char **argv, char **azColName)
{
    result_record tmp;
    std::string param;
    std::string value;

    // Обходим все поля
    for (int i = 0; i < argc; i++)
    {
        param = std::string(azColName[i]);
        if (argv[i] != NULL)
        {
            value = std::string(argv[i]);
        }
        else
        {
            value = "";
        }
        //--- Добавляем в результат
        tmp.insert(std::make_pair(param, value));
    }

    result.push_back(tmp);

    //--- Успешный успех
    return 0;
}

/**
 * @brief Финальная функция, выполняющая запрос к базе
 *
 * @param sql - запрос sql
 * @return result_query
 */
result_query DB::query(std::string sql)
{
    //--- Инициазизация
    {
        //--- Очищаем результат
        result.clear();

        //--- Очищаем ошибки
        sqlite3_free(zErrMsg);
    }

    //--- Выполнение запроса
    rc = sqlite3_exec(db, sql.c_str(), c_callback, this, &zErrMsg);

    //--- Обнуление параметров запроса, для следующих запросов
    {
        where_string = "";
        table_name = "";
        value_name = "*";
    }

    //--- Если запрос завершился с ошибкой, то ексепшн
    if (rc != SQLITE_OK)
    {
        throw DBException("Error db query: " + std::string(zErrMsg));
    }

    return result;
}

/**
 * @brief Подготавливаем sql запрос для выполнения
 *
 * @return std::string
 */
std::string DB::prepare()
{
    if (table_name == "")
    {
        throw DBException("Table name not specified");
    }
    std::string sql = "SELECT " + value_name + " FROM " + this->table_name;
    if (where_string != "")
    {
        sql += " WHERE " + where_string;
    }

    return sql;
}

/**
 * @brief Задаем таблицу запроса
 *
 * @param _table_name
 * @return DB*
 */
DB *DB::table(std::string _table_name)
{
    this->table_name = _table_name;
    return this;
}

/**
 * @brief Задаем условие запроса
 *
 * @param param
 * @param value
 * @return DB*
 */
DB *DB::where(std::string param, std::string value)
{
    this->where_string = param + " = '" + value + "'";
    return this;
}

/**
 * @brief Получаем значение из таблицы по полю _value_name
 *
 * @param _value_name
 * @return std::string
 */
std::string DB::value(std::string _value_name)
{
    value_name = _value_name;
    result_query res;

    try
    {
        res = query(prepare());
    }
    catch (DBException &e)
    {
        throw e;
    }

    if (!res.size())
    {
        throw DBException("Record with value " + _value_name + " not found");
    }

    return res[0][_value_name];
}

/**
 * @brief Получить все записи в таблице
 *
 * @return result_query
 */
result_query DB::get()
{
    value_name = "*";
    result_query res;
    try
    {
        res = query(prepare());
    }
    catch (DBException &e)
    {
        throw e;
    }
    return res;
}

#endif // LIBS_DB_H