//
// Created by Roumite on 14/03/2026.
//

#include <iostream>

#include "ast/ASTProgramNode.h"
#include "lexer/Lexer.h"
#include "lexer/TextParser.h"
#include "parser/parser_program.h"
#include "parser/monadic/monadic.hpp"
#include "parser/monadic/string_parser.h"

using namespace compilator;

int main()
{
    try
    {
        TextParser parser = TextParser::from_file("C:/Users/Roumite/CLionProjects/stegnocode/steg/examples/test.steg");
        Lexer lexer(parser);

        lexer.compute();
        // lexer.display();

        auto tokens = lexer.tokens();

        auto result = parseMainProgram(tokens);

        if (!result)
            std::cout << "Il n'y a pas de prametre";
        else
            result->value->display(0);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
