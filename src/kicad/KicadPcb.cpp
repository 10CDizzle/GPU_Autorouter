#include "kicad/KicadPcb.h"
#include "kicad/SexpParser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

KicadPcb::KicadPcb()
{
}

KicadPcb::~KicadPcb()
{
}

bool KicadPcb::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string contents = buffer.str();

    std::cout << "Successfully opened " << filename << ". Parsing..." << std::endl;

    try
    {
        m_rootNode = SexpParser::parse(contents);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Error parsing KiCad PCB file: " << e.what() << std::endl;
        return false;
    }

    std::cout << "File successfully parsed." << std::endl;
    return true;
}

const SexpNode& KicadPcb::getRootNode() const
{
    return m_rootNode;
}
