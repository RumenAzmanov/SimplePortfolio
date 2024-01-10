#ifndef RECORD_HPP
#define RECORD_HPP

#include <list>
#include <vector>

#include "field.hpp"

/** The record
 *  The main record class containing all the data fields
 *  of a record, used as a simple container, completely
 *  controlled by the table class
 */
class Record {
   private:
    std::vector<DataField*> fields;

   public:
    Record() {}
    ~Record() {
        for (unsigned int i = 0; i < fields.size(); ++i) delete fields[i];
    }

    //! Get data from a record with selected index
    DataField* getData(int index) { return fields[index]; }

    friend class Table;  //!< The record is considered a container and is completely controlled by the table
};

#endif