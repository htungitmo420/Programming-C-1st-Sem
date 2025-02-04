#ifndef BMP_WRITER_H
#define BMP_WRITER_H

#include <string>
#include "sandpile.h"

class BMPWriter {
public:
    static void save(const SandpileModel &model, const std::string &outputDir, int iteration);
};

#endif
