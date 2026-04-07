//
// Created by Roumite on 21/02/2026.
//

#pragma once

#include <exception>
#include <stdexcept>

class InterpreterError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class MemoryError final : public InterpreterError
{
    using InterpreterError::InterpreterError;
};

class RegistryError final: public InterpreterError
{
    using InterpreterError::InterpreterError;
};

class LoaderError final: public InterpreterError
{
    using InterpreterError::InterpreterError;
};

class GraphicalBackendError final: public InterpreterError
{
    using InterpreterError::InterpreterError;
};

class FileError final: public InterpreterError
{
    using InterpreterError::InterpreterError;
};

class ClockError final: public InterpreterError
{
    using InterpreterError::InterpreterError;
};

class UtilsError final: public InterpreterError
{
    using InterpreterError::InterpreterError;
};