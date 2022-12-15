#include "day25.hpp"

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "IntcodeCrusher.hpp"
#include "helper.hpp"

namespace {

std::string getFileCommandPath()
{
    return std::string(BASE_PATH) + std::string("/day25/part1.cmd");
}

std::string getLogPath()
{
    return std::string(BASE_PATH) + std::string("/day25/part1.log");
}

class Droid
{
public:
    Droid(std::ifstream& ifs)
        : m_program(ifs), m_fileCommands(getFileCommandPath()), m_log(getLogPath())
    {
        m_program.setVerbose(false);
        std::cout << getFileCommandPath() << std::endl;
    }

    void run()
    {
        execute(true);
        runFileCommands();

        while (not m_program.isProcessDone() and m_running)
        {
            execute(true);
            userInput();
        }
    }

private:
    void runFileCommands()
    {
        while (m_fileCommands.good())
        {
            std::getline(m_fileCommands, m_command);
            if (m_command.empty())
                continue;

            std::cout << "From file: " << m_command << std::endl;
            if (m_command == "checkpoint")
            {
                std::getline(m_fileCommands, m_command);
                tacklePressureCheck(m_command);
            }

            execute(true);
        }

        m_command = "";
    }

    void execute(bool print)
    {
        if (not m_command.empty())
        {
            m_command += "\n";
            m_program.setInputs(std::deque<BigInt>(m_command.begin(), m_command.end()));
            m_log << m_command;

            if (m_command == "south\n")
                m_pos = add(m_pos, { 0, -1 });
            else if (m_command == "north\n")
                m_pos = add(m_pos, { 0, 1 });
            else if (m_command == "east\n")
                m_pos = add(m_pos, { -1, 0 });
            else if (m_command == "west\n")
                m_pos = add(m_pos, { 1, 0 });
        }

        auto output = m_program.process();
        storeLastOutput(output);
        if (print)
            printLastOutput();
    }

    void userInput()
    {
        std::getline(std::cin, m_command);
        if (m_command == "drop all")
        {
            dropAll(getInv());
            m_command = "";
            return;
        }
        else if (m_command == "quit")
        {
            m_running = false;
            return;
        }
        else if (m_command == "checkpoint")
        {
            tacklePressureCheck();
            m_command = "";
            return;
        }
    }

    std::vector<std::string> getInv()
    {
        m_command = "inv";
        execute(false);

        std::vector<std::string> items;
        std::string::size_type pos = 0, end;
        while (pos != std::string::npos and pos < m_lastOutput.size())
        {
            pos = m_lastOutput.find('-', pos);
            if (pos == std::string::npos)
                break;

            pos += 2;
            end = m_lastOutput.find('\n', pos);
            if (end == std::string::npos)
                break;

            items.push_back(m_lastOutput.substr(pos, end - pos));
            pos = end + 1;
        }

        std::cout << makeRange(items) << std::endl;

        return items;
    }

    int pressureCheckStatus() const
    {
        if (m_lastOutput.find("Alert!") != std::string::npos)
        {
            std::cout << "Try failed" << std::endl;
            if (m_lastOutput.find("heavier") != std::string::npos)
                return -1;
            else
                return 1;
        }
        else
        {
            std::cout << "Success!!" << std::endl;
            return 0;
        }
    }

    bool isCertainlyTooHeavy(const std::vector<uint64_t>& tooHeavy, uint64_t itemSet)
    {
        for (uint64_t items : tooHeavy)
        {
            if ((itemSet & items) == items)
                return true;
        }

        return false;
    }

    bool isCertainlyTooLight(const std::vector<uint64_t>& tooLight, uint64_t itemSet)
    {
        for (uint64_t items : tooLight)
        {
            if ((items & itemSet) == itemSet)
                return true;
        }

        return false;
    }

    void take(const std::string& item)
    {
        m_command = std::string("take ") + item;
        execute(true);
    }

    void drop(const std::string& item)
    {
        m_command = std::string("drop ") + item;
        execute(true);
    }

    void takeItems(const std::vector<std::string>& items, uint64_t current, uint64_t target)
    {
        for (uint64_t i = 0, bit = 1; i < items.size(); ++i, bit <<= 1)
        {
            if (target & bit)
            {
                if (current & bit)
                    continue;

                take(items[i]);
            }
            else if (current & bit)
            {
                drop(items[i]);
            }
        }
    }

    void tacklePressureCheck(std::string command = "")
    {
        if (command.empty())
        {
            std::cout << "Which direction should be moved?: ";
            std::getline(std::cin, command);
        }

        std::vector<uint64_t> tooLight;
        std::vector<uint64_t> tooHeavy;

        const std::vector<std::string> items(getInv());
        int tries = 0;

        uint64_t current = ((uint64_t)1 << items.size()) - 1;
        for (uint64_t itemSet = current; itemSet > 0; --itemSet)
        {
            if (isCertainlyTooHeavy(tooHeavy, itemSet) or isCertainlyTooLight(tooLight, itemSet))
                continue;

            ++tries;
            takeItems(items, current, itemSet);

            m_command = command;
            execute(true);

            int status = pressureCheckStatus();
            if (status == 0)
            {
                std::cout << "Tries: " << tries << std::endl;
                break;
            }
            else if (status < 0)
                tooLight.push_back(itemSet);
            else
                tooHeavy.push_back(itemSet);

            current = itemSet;
        }
    }

    void dropAll(const std::vector<std::string>& items)
    {
        for (const std::string& item : items)
        {
            std::string command = std::string("drop ") + item + "\n";
            m_program.setInputs(std::deque<BigInt>(command.begin(), command.end()));
            execute(false);
        }
    }

    void storeLastOutput(const std::deque<BigInt>& output)
    {
        std::stringstream ss;
        for (BigInt c : output)
            ss << c.convert_to<char>();

        ss << std::endl;
        m_lastOutput = ss.str();
    }

    void printLastOutput() { std::cout << m_lastOutput; }

    IntcodeCrusher m_program;
    Point m_pos = { 0, 0 };
    std::string m_command = "";
    std::string m_lastOutput = "";
    std::ifstream m_fileCommands;
    std::ofstream m_log;
    bool m_running = true;
};

void part1Solution(std::ifstream& ifs)
{
    Droid droid(ifs);
    droid.run();
}

void part2Solution(std::ifstream& ifs)
{
    std::cout << "All done, good job!" << std::endl;
}

} // namespace

void day25Solution(std::ifstream& ifs, int part)
{
    switch (part)
    {
        case 1:
            part1Solution(ifs);
            break;
        case 2:
            part2Solution(ifs);
            break;
        default:
            std::cout << "Unsupported part number " << part << std::endl;
            break;
    }
}
