#ifndef LIBS_DB_MODELS_MODEL_H
#define LIBS_DB_MODELS_MODEL_H

#include "../DB.hpp"

class Model
{
private:
protected:
    std::string table_name = "";
    std::string where_param;
    std::string where_value;
    std::string values = "*";

public:
    Model();
    ~Model();

    virtual std::string getTableName()
    {
        return this->table_name;
    }

    Model *where(std::string param, std::string value);
    std::string value(std::string _values);
    result_query get();
};

Model::Model()
{
}

Model::~Model()
{
}

Model *Model::where(std::string param, std::string value)
{
    where_param = param;
    where_value = value;

    return this;
}

std::string Model::value(std::string _values)
{
    return DB::getInstance()
        ->table(getTableName())
        ->where(where_param, where_value)
        ->value(_values);
}

result_query Model::get()
{
    return DB::getInstance()
        ->table(getTableName())
        ->where(where_param, where_value)
        ->get();
}

#endif // LIBS_DB_MODELS_MODEL_H