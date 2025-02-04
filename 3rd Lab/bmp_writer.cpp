#include "bmp_writer.h"
#include <fstream>
#include <iostream>

static uint8_t colorMap[5][3] = {
    {255, 255, 255}, // 0 grains - White
    {0, 255, 0},     // 1 grain  - Green
    {0, 0, 255},     // 2 grains - Blue
    {255, 255, 0},   // 3 grains - Yellow
    {0, 0, 0}        // 4+ grains - Black
};

void BMPWriter::save(const SandpileModel &model, const std::string &outputDir, int iteration) {
    int width = model.getWidth();
    int height = model.getHeight();
    int fileSize = 54 + 3 * width * height; // BMP header + pixel data

    // Construct file path with iteration number
    std::string filePath = outputDir + "/sandpile_" + std::to_string(iteration) + ".bmp";
    std::ofstream file(filePath, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return;
    }

    // BMP Header fields written individually
    uint16_t fileType = 0x4D42; // 'BM' in little-endian
    int32_t reserved = 0;
    int32_t dataOffset = 54;     // Pixel data offset
    int32_t dibHeaderSize = 40;  // DIB header size
    int16_t planes = 1;          // Color planes
    int16_t bitsPerPixel = 24;   // Bits per pixel
    int32_t compression = 0;     // No compression
    int32_t imageSize = fileSize - 54; // Image size (pixel data only)
    int32_t xPixelsPerMeter = 0; // Horizontal resolution (unused)
    int32_t yPixelsPerMeter = 0; // Vertical resolution (unused)
    int32_t colorsInPalette = 0; // No color palette
    int32_t importantColors = 0; // All colors are important

    // Write BMP file header
    file.write(reinterpret_cast<char*>(&fileType), 2);                   // Signature 'BM'
    file.write(reinterpret_cast<char*>(&fileSize), 4);                   // File size
    file.write(reinterpret_cast<char*>(&reserved), 4);                   // Reserved
    file.write(reinterpret_cast<char*>(&dataOffset), 4);                 // Pixel data offset

    // Write DIB header (bitmap information header)
    file.write(reinterpret_cast<char*>(&dibHeaderSize), 4);              // DIB header size
    file.write(reinterpret_cast<char*>(&width), 4);                      // Image width
    file.write(reinterpret_cast<char*>(&height), 4);                     // Image height
    file.write(reinterpret_cast<char*>(&planes), 2);                     // Planes
    file.write(reinterpret_cast<char*>(&bitsPerPixel), 2);               // Bits per pixel
    file.write(reinterpret_cast<char*>(&compression), 4);                // Compression
    file.write(reinterpret_cast<char*>(&imageSize), 4);                  // Image size
    file.write(reinterpret_cast<char*>(&xPixelsPerMeter), 4);            // Horizontal resolution
    file.write(reinterpret_cast<char*>(&yPixelsPerMeter), 4);            // Vertical resolution
    file.write(reinterpret_cast<char*>(&colorsInPalette), 4);            // Colors in palette
    file.write(reinterpret_cast<char*>(&importantColors), 4);            // Important colors

    // Write pixel data
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            int grains = model.getCell(y, x);
            uint8_t* color;

            // If grains >= 4, use the last color (black); otherwise, use the exact grain color
            if (grains >= 4) {
                color = colorMap[4];
            } else {
                color = colorMap[grains];
            }

            // Write RGB triplet for each pixel
            file.put(color[2]).put(color[1]).put(color[0]); // BMP format requires BGR order
        }

        // Padding to align to a multiple of 4 bytes per row
        for (int p = 0; p < (width % 4); ++p) file.put(0);
    }

    file.close();
}
