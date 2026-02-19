//
// Created by Roumite on 17/02/2026.
//

#include <iostream>

#include "src/images/Image.h"
#include "src/steganographer/Steganographer.h"
#include "src/utils/Logger.h"
#include "src/utils/Parameters.h"

int main(int ac, char** av)
{
    try
    {
        Parameters params(ac, av);
        std::cout << std::to_string(params) << std::endl;

        Image image("C:/Users/Roumite/CLionProjects/stegnocode/cmake-build-debug/image.png");
        Steganographer::decode(image);
        // image.save_png("image.png");
    }
    catch (const ParametersError &)
    {
    }
    catch (const std::exception &e)
    {
        Logger::log(std::string("Error -> ") + e.what());
    }

    return 42;
}
