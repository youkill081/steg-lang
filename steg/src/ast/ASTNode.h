//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include <memory>
#include <iostream>

namespace compiler
{
    class ASTNode
    {
    protected:
        static void display_indent(std::size_t left_padding)
        {
            std::cout << std::string(left_padding * 3, ' ');
        }

        static void display_name(const std::string &name, std::size_t left_padding)
        {
            display_indent(left_padding);
            std::cout << name << "->" << std::endl;
        }
    public:
        virtual ~ASTNode() = default;

        virtual void display(std::size_t left_padding = 0) = 0;
    };
}
