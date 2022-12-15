#include "IntcodeCrusher.hpp"

#include <exception>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

#include "helper.hpp"

namespace {

using Code = IntcodeCrusher::Intcode;

Code::iterator copyIterator(const Code& code, Code::const_iterator it, Code& newCode)
{
    auto hops = std::distance(code.begin(), it);
    auto newIt = newCode.begin();
    std::advance(newIt, hops);
    return newIt;
}

} // namespace

IntcodeCrusher::Intcode IntcodeCrusher::parseIntcode(std::ifstream& ifs)
{
    Intcode code;
    BigInt index = 0;
    while (ifs.good())
    {
        std::string value;
        std::getline(ifs, value, ',');
        code[index++] = BigInt(value);
    }
    return code;
}

IntcodeCrusher::IntcodeCrusher(std::ifstream& ifs)
    : m_code(parseIntcode(ifs)), m_current(m_code.begin()), m_inputs()
{
}

IntcodeCrusher::IntcodeCrusher(const Intcode& code)
    : m_code(code), m_current(m_code.begin()), m_inputs()
{
}

IntcodeCrusher::IntcodeCrusher(const IntcodeCrusher& other)
    : m_code(other.m_code),
      m_current(copyIterator(other.m_code, other.m_current, m_code)),
      m_relativeBase(other.m_relativeBase),
      m_inputs(other.m_inputs),
      m_verbose(other.m_verbose),
      m_done(other.m_done)
{
}

std::deque<BigInt> IntcodeCrusher::process()
{
    std::deque<BigInt> out;
    while (true)
    {
        unsigned tmp = m_current->second.convert_to<unsigned>();
        unsigned opcode = getOpcode(tmp);
        unsigned mode = getMode(tmp);
        if (opcode == OPCODE_ADD)
            add(mode);
        else if (opcode == OPCODE_MULTIPLY)
            multiply(mode);
        else if (opcode == OPCODE_INPUT)
        {
            if (m_inputs.empty())
                break;
            input(mode);
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
        else if (opcode == OPCODE_ADJUST_RELATIVE_BASE)
            adjustRelativeBase(mode);
        else if (opcode == OPCODE_HALT)
        {
            m_done = true;
            break;
        }
        else
        {
            std::cout << "Invalid opcode: " << opcode << std::endl;
            break;
        }
    }

    return out;
}

const BigInt& IntcodeCrusher::getValue(const BigInt& index) const
{
    return m_code.at(index);
}

void IntcodeCrusher::setValue(const BigInt& index, const BigInt& value)
{
    m_code[index] = value;
}

void IntcodeCrusher::setInputs(const std::deque<BigInt>& inputs)
{
    m_inputs.insert(m_inputs.end(), inputs.begin(), inputs.end());
}

void IntcodeCrusher::printCode() const
{
    for (const auto& [key, value] : m_code)
        std::cout << "{" << key << "," << value << "}, ";

    std::cout << std::endl;
}

void IntcodeCrusher::setVerbose(bool verbose)
{
    m_verbose = verbose;
}

bool IntcodeCrusher::isProcessDone() const
{
    return m_done;
}

BigInt& IntcodeCrusher::getVal(const BigInt& index)
{
    auto it = m_code.find(index);
    if (it == m_code.end())
    {
        std::tie(it, std::ignore) = m_code.insert({ index, BigInt(0) });
    }
    return it->second;
}

BigInt& IntcodeCrusher::access(int offset, unsigned mode)
{
    int shift = 1;
    for (int i = 1; i < offset; ++i)
        shift *= 10;

    auto tmpIt = m_current;
    std::advance(tmpIt, offset);
    BigInt& tmp = tmpIt->second;
    switch ((mode / shift) % 10)
    {
        case 0:
            return getVal(tmp);
        case 1:
            return tmp;
        case 2:
            return getVal(tmp + m_relativeBase);
        default:
            throw std::invalid_argument("Unsupported access mode");
    }
}

void IntcodeCrusher::add(unsigned mode)
{
    access(3, mode) = access(1, mode) + access(2, mode);
    std::advance(m_current, 4);
}

void IntcodeCrusher::multiply(unsigned mode)
{
    access(3, mode) = access(1, mode) * access(2, mode);
    std::advance(m_current, 4);
}

void IntcodeCrusher::input(unsigned mode)
{
    if (m_inputs.empty())
    {
        std::cout << "Something went wrong, no input available" << std::endl;
        return;
    }

    access(1, mode) = m_inputs.front();
    m_inputs.pop_front();
    std::advance(m_current, 2);
}

void IntcodeCrusher::output(std::deque<BigInt>& out, unsigned mode)
{
    const BigInt& val = access(1, mode);
    out.push_back(val);
    if (m_verbose)
        std::cout << "Output: " << val << std::endl;
    std::advance(m_current, 2);
}

void IntcodeCrusher::jumpIfTrue(unsigned mode)
{
    if (access(1, mode) != 0)
        m_current = m_code.find(access(2, mode));
    else
        std::advance(m_current, 3);
}

void IntcodeCrusher::jumpIfFalse(unsigned mode)
{
    if (access(1, mode) == 0)
        m_current = m_code.find(access(2, mode));
    else
        std::advance(m_current, 3);
}

void IntcodeCrusher::lessThan(unsigned mode)
{
    access(3, mode) = access(1, mode) < access(2, mode) ? 1 : 0;
    std::advance(m_current, 4);
}

void IntcodeCrusher::equals(unsigned mode)
{
    access(3, mode) = access(1, mode) == access(2, mode) ? 1 : 0;
    std::advance(m_current, 4);
}

void IntcodeCrusher::adjustRelativeBase(unsigned mode)
{
    m_relativeBase += access(1, mode);
    std::advance(m_current, 2);
}

unsigned IntcodeCrusher::getOpcode(unsigned value)
{
    return value % 100;
}

unsigned IntcodeCrusher::getMode(unsigned value)
{
    return value / 100;
}
