#ifndef CITU_LOCATION_HPP
#define CITU_LOCATION_HPP

#include <string>

struct SourceLocation {
    std::string filename;
    int first_line = 1;
    int first_column = 1;
    int last_line = 1;
    int last_column = 1;

    std::string to_string() const {
        return filename + ":" + std::to_string(first_line) + ":" + std::to_string(first_column);
    }
};

#endif // CITU_LOCATION_HPP
