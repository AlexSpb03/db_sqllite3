#ifndef LIBS_DB_MODEL_DEVICES_H
#define LIBS_DB_MODEL_DEVICES_H

#include "model.hpp"

class Model_Devices : public Model
{
    //--- название таблицы в базе
    std::string getTableName() override
    {
        return "devices";
    }

public:
};

#endif // LIBS_DB_MODEL_DEVICES_H