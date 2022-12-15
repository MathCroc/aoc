#pragma once

#include <deque>
#include <fstream>
#include <vector>

using Intcode = std::vector<long long>;

Intcode parseIntcode(std::ifstream& ifs);

class IntcodeProcessor
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
    static constexpr unsigned OPCODE_HALT = 99;

    IntcodeProcessor(std::ifstream& ifs);
    IntcodeProcessor(const Intcode& code);
    IntcodeProcessor(const IntcodeProcessor& other);

    std::deque<long long> process();
    long long getValue(int index) const;
    void setValue(int index, long long value);
    void setInputs(const std::deque<long long>& inputs);
    void setVerbose(bool verbose);
    void printCode() const;
    bool isProcessDone() const;

private:
    long long access(int offset, unsigned mode);
    void add(unsigned mode);
    void multiply(unsigned mode);
    void input();
    void output(std::deque<long long>& out, unsigned mode);
    void jumpIfTrue(unsigned mode);
    void jumpIfFalse(unsigned mode);
    void lessThan(unsigned mode);
    void equals(unsigned mode);

    static unsigned getOpcode(long long value);
    static unsigned getMode(long long value);

    Intcode m_code;
    int m_index = 0;
    std::deque<long long> m_inputs;
    bool m_verbose = true;
    bool m_done = false;
};
