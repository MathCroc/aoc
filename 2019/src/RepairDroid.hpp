#pragma once

#include <unordered_map>
#include <utility>

#include "IntcodeCrusher.hpp"
#include "helper.hpp"

class RepairDroid
{
public:
    RepairDroid(const IntcodeCrusher::Intcode& code) : m_processor(code), m_map()
    {
        m_processor.setVerbose(false);
    }

    std::pair<Point, int> findOxygenStation();
    void drawMap() const;
    const std::unordered_map<Point, int, PointHash>& getMap() const { return m_map; }

private:
    void dfs(unsigned toDir, std::unordered_map<Point, int, PointHash>& dist, int count);
    int move(unsigned dirIndex);
    Point getNewPos(unsigned dirIndex) const;

    IntcodeCrusher m_processor;
    std::unordered_map<Point, int, PointHash> m_map;
    Point m_pos = Point{ 0, 0 };
};
