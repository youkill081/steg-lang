//
// Created by Roumite on 17/02/2026.
//

#include <iostream>

#include "steganographer/handlers/handlers.h"
#include "steganographer/handlers/IHander.h"
#include "steganographer/images/Image.h"
#include "steganographer/steganographer/Steganographer.h"
#include "Logger.h"
#include "steganographer/utils/Parameters.h"

int main(int ac, char** av)
{
    try
    {
        Parameters params(ac, av);
        Image image(params.get_image_path());

        if (params.get_mode() == ENCODE)
        {
            std::unique_ptr<IHandler> handler = handler_factory(params.get_type());
            ByteBuffer data = handler->encode(params);
            Steganographer::encode(image, data, params.get_type());
            image.save_png(params.get_output_path());
        }
        else if (params.get_mode() == DECODE)
        {
            Steganographer::DecodeResult result = Steganographer::decode(image);
            auto data_type = static_cast<DataType>(result.header.data_type);

            const std::unique_ptr<IHandler> handler = handler_factory(data_type);
            handler->decode(params, result.data);

            std::cout << "Press Enter to leave..." << std::endl;
            std::cin.get();
            return 0;
        }
    }
    catch (const ParametersError &)
    {
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cout << "This image do not contains data ! (" << e.what() << ")" << std::endl;
        std::cout << "Press Enter to leave..." << std::endl;
        std::cin.get();
        return 2;
    }
}
