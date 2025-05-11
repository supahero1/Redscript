#include <iostream>
#include "lexer.hpp"
#include "file.hpp"
#include "logger.hpp"
#include "rbc.hpp"
int main(int argc, const char** argv)
{
    if (argc < 3) 
    {
        ERROR("Invalid command line arguments supplied (argc: %d).", argc);
        return EXIT_FAILURE;
    }

    const char* fileName = argv[1];

    std::string fContent = readFile(fileName);

    if(fContent.length() == 0)
    {
        ERROR("Provided source file does not exist.");
        return EXIT_FAILURE;
    }

    rs_error error;
    token_list list = tlex(fileName, fContent, &error);

    if (error.trace.ec)
    {
        printerr(error);
        return EXIT_FAILURE;
    }

    INFO("Token Count: %d", list.size());
    for(token t : list)
    {
        std::cout << t.str() << std::endl;
    }
    
    INFO("Compiling...");

    rbc_program bytecode = torbc(list, fileName, fContent, &error);

    if (error.trace.ec)
    {
        printerr(error);
        return EXIT_FAILURE;
    }

    mc_program endProgram = tomc(bytecode, &error);

    if (error.trace.ec)
    {
        printerr(error);
        return EXIT_FAILURE;
    }

    writemc(endProgram, argv[2], &error);

    if (error.trace.ec)
    {
        printerr(error);
        return EXIT_FAILURE;
    }
    SUCCESS("Compiled successfully to %s.", argv[2]);
    return EXIT_SUCCESS;
}