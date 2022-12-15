#pragma once

#include <deque>
#include <fstream>
#include <map>
#include <vector>

#include "defs.hpp"

class IntcodeCrusher
{
public:
    static constexpr unsigned OPCODE_ADD = 1;
    static constexpr unsigned OPCODE_MULTIPLY = 2;
    static constexpr unsigned OPCODE_INPUT = 3;
    static constexpr unsigned OPCODE_OUTPUT = 4;
    static constexpr unsigned OPCODE_JUMP_IF_TRUE = 5;
    static constexpr unsigned OPCODE_JUMP_IF_FALSE = 6;
    static constexpr unsigned OPCODE_LESS_THAN = 7;
    static constexpr unsigned OPCODE_EQUALS = 8;
    static constexpr unsigned OPCODE_ADJUST_RELATIVE_BASE = 9;
    static constexpr unsigned OPCODE_HALT = 99;

    using Intcode = std::map<BigInt, BigInt>;

    static Intcode parseIntcode(std::ifstream& ifs);

    IntcodeCrusher() = default;
    IntcodeCrusher(std::ifstream& ifs);
    IntcodeCrusher(const Intcode& code);
    IntcodeCrusher(const IntcodeCrusher& other);

    std::deque<BigInt> process();
    const BigInt& getValue(const BigInt& index) const;
    void setValue(const BigInt& index, const BigInt& value);
    void setInputs(const std::deque<BigInt>& inputs);
    void setVerbose(bool verbose);
    void printCode() const;
    bool isProcessDone() const;

private:
    BigInt& getVal(const BigInt& index);
    BigInt& access(int offset, unsigned mode);

    void add(unsigned mode);
    void multiply(unsigned mode);
    void input(unsigned mode);
    void output(std::deque<BigInt>& out, unsigned mode);
    void jumpIfTrue(unsigned mode);
    void jumpIfFalse(unsigned mode);
    void lessThan(unsigned mode);
    void equals(unsigned mode);
    void adjustRelativeBase(unsigned mode);

    static unsigned getOpcode(unsigned value);
    static unsigned getMode(unsigned value);

    Intcode m_code;
    Intcode::iterator m_current;
    BigInt m_relativeBase = 0;
    std::deque<BigInt> m_inputs;
    bool m_verbose = true;
    bool m_done = false;
};
