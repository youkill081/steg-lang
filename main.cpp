//
// Created by Roumite on 17/02/2026.
//

#include <iostream>

#include "src/images/Image.h"
#include "src/steganographer/Steganographer.h"
#include "src/utils/Logger.h"

int main() {
    try
    {
        Image image("C:/Users/Roumite/CLionProjects/stegnocode/cmake-build-debug/image.png");
        Steganographer::decode(image);
        // image.save_png("image.png");
    } catch (const std::exception &e)
    {
        Logger::log(std::string("Error -> ") + e.what());
    }

    return 42;
}
