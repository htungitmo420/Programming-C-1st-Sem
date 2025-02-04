#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "sandpile.h"
#include "bmp_writer.h"

void parseArguments(int argc, char* argv[], std::string &inputFile, std::string &outputDir, int &maxIter, int &frequency) {
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-i" || std::string(argv[i]) == "--input") {
            inputFile = argv[++i];
        } else if (std::string(argv[i]) == "-o" || std::string(argv[i]) == "--output") {
            outputDir = argv[++i];
        } else if (std::string(argv[i]) == "-m" || std::string(argv[i]) == "--max-iter") {
            maxIter = std::stoi(argv[++i]);
        } else if (std::string(argv[i]) == "-f" || std::string(argv[i]) == "--freq") {
            frequency = std::stoi(argv[++i]);
        } else {
            throw std::invalid_argument("Invalid argument");
        }
    }
}

int main(int argc, char* argv[]) {
    std::string inputFile, outputDir;
    int maxIter = 5, frequency = 1;
    try {
        parseArguments(argc, argv, inputFile, outputDir, maxIter, frequency);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    SandpileModel model;
    std::cout << "Initializing model with input file: " << inputFile << std::endl;
    model.initialize(inputFile);
    /*
    cout << "Height: " << model.getHeight()  << endl;
        cout << "Width: " << model.getWidth()  << endl;
        for (int x = 0; x < model.getHeight(); x++){
            for (int y = 0; y < model.getWidth();y++){
                cout << model.getCell(x, y) << " ";
            }
            cout << endl;
        }*/
    for (int i = 0; i < maxIter; i++) {
        model.update();
        std::cout << "Iteration " << i << " completed." << std::endl;

        if (frequency > 0 && i % frequency == 0) {
            BMPWriter::save(model, outputDir, i);
            std::cout << "Saved BMP for iteration " << i << std::endl;
        }

        if (model.isStable()) {
            std::cout << "Model reached stability at iteration " << i << std::endl;
            break;
        }

        /*
        cout << "Height: " << model.getHeight()  << endl;
        cout << "Width: " << model.getWidth()  << endl;

        for (int x = 0; x < model.getHeight(); x++){
            for (int y = 0; y < model.getWidth();y++){
                cout << model.getCell(x, y) << " ";
            }
            cout << endl;
        }
        */
    }
    BMPWriter::save(model, outputDir, maxIter);
    std::cout << "Final BMP saved." << std::endl;

    return 0;
}
