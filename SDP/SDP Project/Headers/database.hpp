#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <list>
#include <string>
#include <vector>

#include "field.hpp"
#include "record.hpp"
#include "table.hpp"

/** The database
 *  The main database class containing all the tables,
 *  used as a simple container, completely controlled by the
 *  CommandLine interface
 */
class Database {
   private:
    std::vector<Table *> tables;

   public:
    Database() {}
    ~Database() {}

    friend class CommandLine;  //!< The database is considered a container and is completely controlled by the CLI
};

#endif