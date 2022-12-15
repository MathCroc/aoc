#include "day9.hpp"

#include <iostream>
#include <unistd.h>
#include <unordered_map>

#include "IntcodeCrusher.hpp"
#include "helper.hpp"

namespace {

constexpr int EMPTY = 0;
constexpr int WALL = 1;
constexpr int BLOCK = 2;
constexpr int PADDLE = 3;
constexpr int BALL = 4;

class ArcadeGame
{
public:
    ArcadeGame(std::ifstream& ifs) : m_processor(ifs) { m_processor.setVerbose(false); }

    void setUserInput(bool userInput) { m_userInput = userInput; }
    void setVisible(bool visible) { m_visible = visible; }
    long long getScore() const { return m_score; }

    void play()
    {
        setUserInput(0);
        setVisible(0);

        // bool val;
        // std::cout << "Starting game" << std::endl;
        // setUserInput(0);

        // std::cout << "Enable user input? [0/1]: ";
        // std::cin >> val;
        // setUserInput(val);

        // if (not m_userInput)
        // {
        //     std::cout << "Render game? [0/1]: ";
        //     std::cin >> val;
        //     setVisible(val);
        // }

        m_processor.setValue(0, 2);
        while (not m_processor.isProcessDone())
        {
            std::deque<BigInt> output = m_processor.process();
            update(output);

            if (m_visible)
            {
                usleep(100000);
                render();
            }

            if (m_userInput)
                userInput();
            else
                aiInput();
        }
    }

private:
    void update(const std::deque<BigInt>& output)
    {
        for (unsigned i = 0; i < output.size(); i += 3)
        {
            Point p{ (int)output[i], (int)output[i + 1] };
            if (p == Point{ -1, 0 })
            {
                m_score = (long long)output[i + 2];
                continue;
            }

            int tileId = (int)output[i + 2];
            if (tileId == PADDLE)
            {
                if (m_paddle != Point{ -1, -1 })
                    m_tiles[m_paddle] = EMPTY;

                m_paddle = p;
            }
            else if (tileId == BALL)
            {
                m_ball = p;
            }

            m_tiles[p] = tileId;
        }
    }

    void render() const
    {
        int xMin = 0, xMax = 0, yMin = 0, yMax = 0;
        for (const auto& [point, tileId] : m_tiles)
        {
            (void)tileId;
            xMin = std::min(xMin, point.x);
            xMax = std::max(xMax, point.x);
            yMin = std::min(yMin, point.y);
            yMax = std::max(yMax, point.y);
        }

        for (int y = yMin; y <= yMax; ++y)
        {
            for (int x = xMin; x <= xMax; ++x)
            {
                Point pos{ x, y };
                auto it = m_tiles.find(pos);
                if (it == m_tiles.end())
                    std::cout << ' ';
                else
                {
                    char c;
                    switch (it->second)
                    {
                        case WALL:
                            c = '|';
                            break;
                        case BLOCK:
                            c = '#';
                            break;
                        case PADDLE:
                            c = '-';
                            break;
                        case BALL:
                            c = 'O';
                            break;
                        default:
                            c = ' ';
                            break;
                    }
                    std::cout << c;
                }
            }
            std::cout << std::endl;
        }

        std::cout << "Score: " << m_score << std::endl;
    }

    void userInput()
    {
        char c;
        int in;
        std::cout << "Input [a/s/d]: ";
        std::cin >> c;
        switch (c)
        {
            case 'a':
                in = -1;
                break;
            case 'd':
                in = 1;
                break;
            default:
                in = 0;
                break;
        }

        m_processor.setInputs({ in });
    }

    void aiInput() { m_processor.setInputs({ sgn(m_ball.x - m_paddle.x) }); }

    IntcodeCrusher m_processor;
    std::unordered_map<Point, int, PointHash> m_tiles;
    Point m_paddle = Point{ -1, -1 };
    Point m_ball = Point{ -1, -1 };
    long long m_score = 0;
    bool m_userInput = false;
    bool m_visible = true;
}; // namespace

void part1Solution(std::ifstream& ifs)
{
    IntcodeCrusher processor(ifs);
    processor.setVerbose(false);
    std::deque<BigInt> output = processor.process();

    std::unordered_map<Point, int, PointHash> tiles;
    for (unsigned i = 0; i < output.size(); i += 3)
    {
        Point p{ (int)output[i], (int)output[i + 1] };
        auto it = tiles.find(p);
        if (it == tiles.end())
        {
            tiles.insert({ p, (int)output[i + 2] });
        }
        else
        {
            if (it->second != WALL and it->second != PADDLE)
                it->second = (int)output[i + 2];
        }
    }

    int count = 0;
    for (const auto& [p, tileId] : tiles)
    {
        (void)p;
        if (tileId == BLOCK)
            count += 1;
    }

    std::cout << "Number of blocks: " << count << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    ArcadeGame game(ifs);
    game.play();

    std::cout << "Final score: " << game.getScore() << std::endl;
}

} // namespace

void day13Solution(std::ifstream& ifs, int part)
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
