#include "intcode.hpp"

#include <iostream>

#include "helper.hpp"

Intcode parseIntcode(std::ifstream& ifs)
{
    Intcode code;
    long long val;
    while (ifs.good())
    {
        ifs >> val;
        ifs.ignore(1, ',');
        code.push_back(val);
    }
    return code;
}

IntcodeProcessor::IntcodeProcessor(std::ifstream& ifs) : m_code(parseIntcode(ifs))
{
}

IntcodeProcessor::IntcodeProcessor(const Intcode& code) : m_code(code)
{
}

IntcodeProcessor::IntcodeProcessor(const IntcodeProcessor& other)
    : m_code(other.m_code),
      m_index(other.m_index),
      m_inputs(other.m_inputs),
      m_verbose(other.m_verbose),
      m_done(other.m_done)
{
}

std::deque<long long> IntcodeProcessor::process()
{
    std::deque<long long> out;
    while (m_index < static_cast<int>(m_code.size()))
    {
        unsigned opcode = getOpcode(m_code[m_index]);
        unsigned mode = getMode(m_code[m_index]);
        if (opcode == OPCODE_ADD)
            add(mode);
        else if (opcode == OPCODE_MULTIPLY)
            multiply(mode);
        else if (opcode == OPCODE_INPUT)
        {
            if (m_inputs.empty())
                break;
            input();
        }
        else if (opcode == OPCODE_OUTPUT)
            output(out, mode);
        else if (opcode == OPCODE_JUMP_IF_TRUE)
            jumpIfTrue(mode);
        else if (opcode == OPCODE_JUMP_IF_FALSE)
            jumpIfFalse(mode);
        else if (opcode == OPCODE_LESS_THAN)
            lessThan(mode);
        else if (opcode == OPCODE_EQUALS)
            equals(mode);
        else if (opcode == OPCODE_HALT)
        {
            m_done = true;
            break;
        }
        else
        {
            std::cout << "Invalid opcode: " << opcode << ", index: " << m_index << std::endl;
            break;
        }
    }

    return out;
}

long long IntcodeProcessor::getValue(int index) const
{
    return m_code[index];
}

void IntcodeProcessor::setValue(int index, long long value)
{
    m_code[index] = value;
}

void IntcodeProcessor::setInputs(const std::deque<long long>& inputs)
{
    m_inputs.insert(m_inputs.end(), inputs.begin(), inputs.end());
}

void IntcodeProcessor::printCode() const
{
    printVec(m_code);
}

void IntcodeProcessor::setVerbose(bool verbose)
{
    m_verbose = verbose;
}

bool IntcodeProcessor::isProcessDone() const
{
    return m_done;
}

long long IntcodeProcessor::access(int offset, unsigned mode)
{
    int shift = 1;
    for (int i = 1; i < offset; ++i)
        shift *= 10;

    int pos = m_index + offset;
    return m_code[(mode / shift) % 10 == 0 ? m_code[pos] : pos];
}

void IntcodeProcessor::add(unsigned mode)
{
    m_code[m_code[m_index + 3]] = access(1, mode) + access(2, mode);
    m_index += 4;
}

void IntcodeProcessor::multiply(unsigned mode)
{
    m_code[m_code[m_index + 3]] = access(1, mode) * access(2, mode);
    m_index += 4;
}

void IntcodeProcessor::input()
{
    if (m_inputs.empty())
    {
        std::cout << "Something went wrong, no input available" << std::endl;
        return;
    }

    setValue(m_code[m_index + 1], m_inputs.front());
    m_inputs.pop_front();
    m_index += 2;
}

void IntcodeProcessor::output(std::deque<long long>& out, unsigned mode)
{
    long long val = access(1, mode);
    out.push_back(val);
    if (m_verbose)
        std::cout << "Output: " << val << std::endl;
    m_index += 2;
}

void IntcodeProcessor::jumpIfTrue(unsigned mode)
{
    if (access(1, mode) != 0)
        m_index = access(2, mode);
    else
        m_index += 3;
}

void IntcodeProcessor::jumpIfFalse(unsigned mode)
{
    if (access(1, mode) == 0)
        m_index = access(2, mode);
    else
        m_index += 3;
}

void IntcodeProcessor::lessThan(unsigned mode)
{
    m_code[m_code[m_index + 3]] = access(1, mode) < access(2, mode) ? 1 : 0;
    m_index += 4;
}

void IntcodeProcessor::equals(unsigned mode)
{
    m_code[m_code[m_index + 3]] = access(1, mode) == access(2, mode) ? 1 : 0;
    m_index += 4;
}

unsigned IntcodeProcessor::getOpcode(long long value)
{
    return static_cast<unsigned>(value % 100);
}

unsigned IntcodeProcessor::getMode(long long value)
{
    return static_cast<unsigned>(value / 100);
}
