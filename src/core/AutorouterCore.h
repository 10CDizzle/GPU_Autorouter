#ifndef AUTOROUTER_CORE_H
#define AUTOROUTER_CORE_H

#include <memory>
#include <string>

class PcbData;
class PcbParser;

class AutorouterCore {
public:
    AutorouterCore();
    ~AutorouterCore();

    /**
     * @brief Loads a PCB from a file.
     * @param filePath The path to the PCB file.
     * @return true on success, false on failure.
     */
    bool loadPcbFile(const std::string& filePath);

    std::shared_ptr<PcbData> getPcbData() const;

private:
    std::unique_ptr<PcbParser> m_parser;
    std::shared_ptr<PcbData> m_pcbData;
};

#endif // AUTOROUTER_CORE_H