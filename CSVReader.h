#ifndef CSV_READER_H
#define CSV_READER_H

#include <vector>
#include <string>

class CSVReader {
public:
    static std::vector<double> read_strain(const std::string& filename);
};

#endif