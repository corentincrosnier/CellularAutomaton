#pragma once
#include "QuadTree.hpp"
#include "RuleSet.hpp"
#include "Cell.hpp"
#include <hephaestus/memory/hephMemoryAllocator.hpp>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct Bench{
  int           startBenchTime=0;
  int           frame=0;
  int           nbFrame=0;
  std::string   filePath="";
  bool          active=false;

  int           gridSize=0;
  double        fps;
  double        genPeriod;
  int           genCount;
  int           cellCount;
  double        periodPerCell;
  double        periodPerLiveCell;
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
    void                drawQuadTree(std::vector<Vertex>& vertices, std::vector<int>& indices, int index);
    void                drawCell(std::vector<Vertex>& verticesTri, std::vector<int>& indicesTri, int x, int y);
    void                drawInfo(double& time, double& prevTime);
    void                toggleShowInfo(){m_showInfo=(m_showInfo)?false:true;}
    bool                getShowInfo(){return m_showInfo;}
    void                benchmark(double& time, int nbFrame, std::string benchFilePath="");
    void                benchAddFrame(double& time, double& prevTime);

    void                loadRLEat(int x, int y, std::string rlepath);
    std::vector<Cell>   copy(glm::ivec2 pos, int range);

    float               getCellSize(){return 2.0/m_height;}
    int                 getWidth(){return m_width;}
    int                 getHeight(){return m_height;}

    void                setState(int x, int y, int state);
    void                activateCell(int x, int y);
    void                switchCell(int x, int y);

  protected:
    Bench                             m_bench;
    bool                              m_showInfo=false;
    int                               m_width=100;
    int                               m_height=100;
    int                               m_genCount=0;
    std::vector<Cell>                 m_cells;
    std::vector<Cell>                 m_prevCells;
    std::vector<glm::ivec2>           m_liveCells;
    std::shared_ptr<RuleSet>          m_ruleset;
    std::shared_ptr<QuadTree>         m_quadtree;
};
