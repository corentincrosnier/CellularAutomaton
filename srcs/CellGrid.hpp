#pragma once
#include "RuleSet.hpp"
#include "Cell.hpp"
#include <hephaestus/memory/hephMemoryAllocator.hpp>
#include <memory>
#include <vector>
#include <glm/glm.hpp>


struct Vertex{
  glm::vec2 pos;
  glm::vec3 color;
};

class CellGrid{

  public:

    CellGrid(){}
    CellGrid(CellGrid& a);
    CellGrid(int width, int height);

    void                nextGen();
    void                applyRuleSet(Cell& cell, std::vector<Cell> grid, int range=1);
    void                applyRuleSet(Cell& cell, std::vector<Cell> grid, RuleSet& ruleset);

    void                draw(std::vector<Vertex>& verticesTri, std::vector<int>& indicesTri);
    void                drawCell(std::vector<Vertex>& verticesTri, std::vector<int>& indicesTri, int x, int y);

    void                loadRLEat(int x, int y, std::string rlepath);
    std::vector<Cell>   copy(glm::ivec2 pos, int range);

    float               getCellSize(){return 2.0/m_height;}
    int                 getWidth(){return m_width;}
    int                 getHeight(){return m_height;}

    void                setState(int x, int y, int state);
    void                activateCell(int x, int y);
    void                switchCell(int x, int y);

  protected:
    int                               m_width=100;
    int                               m_height=100;
    int                               m_genCount=0;
    std::shared_ptr<RuleSet>          m_ruleset;
    std::vector<Cell>                 m_cells;
    std::vector<Cell>                 m_prevCells;
    std::vector<glm::ivec2>           m_liveCells;
};
