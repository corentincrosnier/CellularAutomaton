#include "CellGrid.hpp"
#include "CAParser.hpp"
#include "Cell.hpp"
#include "RuleSet.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <boost/json/kind.hpp>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <glm/fwd.hpp>
#include <hephaestus/memory/hephMemoryAllocator.hpp>
#include <memory>
#include <string>
#include <vector>
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define WIDTH   160
#define HEIGHT  90



CellGrid::CellGrid(int width, int height){
  //m_width=width;
  //m_height=height;
  m_width=WIDTH;
  m_height=HEIGHT;
  for(int j=0;j<m_height;j++){
    for(int i=0;i<m_width;i++){
      m_cells.push_back(Cell(i,j));
    }
  }
  m_quadtree=std::make_shared<QuadTree>();
  /*
  setState(8, 4 , 1);
  m_quadtree->addCellAt(glm::ivec2(8,4));
  setState(12, 4 , 1);
  m_quadtree->addCellAt(glm::ivec2(12,4));
  setState(100, 30 , 1);
  m_quadtree->addCellAt(glm::ivec2(100,30));
  */
  m_ruleset=CAParser::parseCA("./gol.ca");
  //loadRLEat(0, 0, "./life_patterns/newgun.rle");
  //loadRLEat(40, 10, "./life_patterns/34p20.rle");
}

void CellGrid::loadRLEat(int x, int y, std::string rlepath){
  std::ifstream rle(rlepath);
  if(!rle.is_open()){
    std::cout << "couldn't find .rle file at " << rlepath << std::endl;
    return;
  }

  int _x=0;
  int _y=0;
  char c;
  int tmp=0;
  std::string s;

  while( (c=rle.peek()) !='!'){
    if((x+_x)<0 || (x+_x)>=m_width || (y+_y)<0 || (y+_y)>=m_height){
      std::cout << "while loading " << rlepath << ": index: " << (x+_x) << "," << (y+_y) << " is out of bound\n";  
      break;
    }
    int ind=(y+_y)*m_width+x+_x;
    if(c=='#' || c=='@'){
      std::getline(rle,s);
      continue;
    }
    if(std::isdigit(c)){
      rle >> tmp >> c;
      if(c=='$'){
        _x=0;
        _y+=tmp;
        continue;
      }
      int state=(c=='b')?CELL_STATE_0:CELL_STATE_1;
      for(int i=ind;i<ind+tmp;i++){
        setState(x+_x, y+_y, state);
        _x++;
      }
    }
    else if(c=='b'){
      setState(x+_x, y+_y, CELL_STATE_0);
      _x++;
      rle.get();
    }
    else if(c=='o'){
      setState(x+_x, y+_y, CELL_STATE_1);
      _x++;
      rle.get();
    }
    else if(c=='$'){
      _y++;
      _x=0;
      rle.get();
    }
    else if(c=='x' || c=='y' || c=='r'){
      std::getline(rle, s);
    }
    else
      rle.get();
  }
}

void CellGrid::nextGen(){
  m_quadtree->nextGen();
}

/*
void CellGrid::nextGen(){
  m_prevCells=m_cells;
  if(m_ruleset!=nullptr){
    for(auto& c: m_cells)
      applyRuleSet(c, m_prevCells, *m_ruleset);
  }
  else{
    for(auto& c: m_cells)
      applyRuleSet(c, m_prevCells);
  }
  m_genCount++;
}
*/

void CellGrid::applyRuleSet(Cell& cell, std::vector<Cell> grid, RuleSet& ruleset){
  std::vector<int> tally;
  for(int i=0;i<ruleset.stateCount;i++)
    tally.push_back(0);

  for(auto& r: ruleset.regions){
    for(auto& rel: r.relativeCells){
      int x=cell.pos.x + rel.x;
      int y=cell.pos.y + rel.y;
      if(x<0 || x>=m_width || y<0 || y>=m_height){
        tally[0]++;
        continue;
      }
      tally[grid[y*m_width+x].state]++;
    }
  }

  for(auto& t: ruleset.transitions){
    if(cell.state!=t.stateStartId)
      continue;
    if(t.satisfy(tally)){
      setState(cell.pos.x, cell.pos.y, t.stateEndId);
      break;
    }
  }
}

void CellGrid::applyRuleSet(Cell& cell, std::vector<Cell> grid, int range){
  //std::cout << "applyRuleSet\n";
  int nbAlive=0;
  for(int i=cell.pos.x-range;i<=cell.pos.x+range;i++){
    for(int j=cell.pos.y-range;j<=cell.pos.y+range;j++){
      if(i==cell.pos.x && j==cell.pos.y)
        continue;
      if(i<0 || i>=m_width || j<0 || j>=m_height)
        continue;
      if(grid[j*m_width+i].state == CELL_STATE_1)
        nbAlive++;
    }
  }
  if(cell.state==CELL_STATE_1 && nbAlive<2){
    cell.state=CELL_STATE_0;
    auto it=std::find(m_liveCells.begin(), m_liveCells.end(), cell.pos);
    if(it != m_liveCells.end()){
      m_liveCells.erase(it);
      std::cout << "found live cell at " << cell.pos.x << "," << cell.pos.y << ", erased by underpopulation\n";
    }
    else
      std::cout << "couldn't find live underpopulated cell!! at " << cell.pos.x << "," << cell.pos.y << std::endl;
  }
  else if(cell.state==CELL_STATE_0 && nbAlive==3){
    cell.state=CELL_STATE_1;
    std::cout << "reproduced cell at " << cell.pos.x << "," << cell.pos.y << std::endl;
    m_liveCells.push_back(cell.pos);
  }
  else if(cell.state==CELL_STATE_1 && nbAlive>3){
    cell.state=CELL_STATE_0;
    auto it=std::find(m_liveCells.begin(), m_liveCells.end(), cell.pos);
    if(it != m_liveCells.end()){
      m_liveCells.erase(it);
      std::cout << "found live cell at " << cell.pos.x << "," << cell.pos.y << ", erased by overpopulation\n";
    }
    else
      std::cout << "couldn't find live overpopulated cell!! at " << cell.pos.x << "," << cell.pos.y << std::endl;
  }
}

void CellGrid::setState(int x, int y, int state){
  auto it=std::find(m_liveCells.begin(), m_liveCells.end(), glm::ivec2(x,y));
  if(it==m_liveCells.end() && state>0){
    m_liveCells.push_back(glm::ivec2(x,y));
    m_quadtree->addCellAt(glm::ivec2(x,y));
  }
  else if(it!=m_liveCells.end() && state==CELL_STATE_0){
    m_liveCells.erase(it);
    m_quadtree->deleteCellAt(glm::ivec2(x,y));
  }
  m_cells[y*m_width+x].state=state;
}

void CellGrid::activateCell(int x, int y){
  m_cells[y*m_width+x].state=CELL_STATE_1;
  m_liveCells.push_back(glm::ivec2(x,y));
}

void CellGrid::switchCell(int x, int y){
  int ind=y*m_width+x;
  setState(x,y,(m_cells[ind].state+1)%m_ruleset->stateCount);
}

void CellGrid::drawCell(std::vector<Vertex>& verticesTri, std::vector<int>& indicesTri, int x, int y){
  float cellH=2.0/m_height;
  float cellW=2.0/m_width;
  int nbVertex=verticesTri.size();
  int st=m_cells[y*m_width+x].state;
  Vertex v={
    .pos=glm::vec2(x*cellW-1,y*cellH-1),
    .color=glm::vec3(0.3+st*0.2,0.3+st*0.2,0.3+st*0.2)
  };
  verticesTri.push_back(v);
  v.pos.x+=cellW;
  verticesTri.push_back(v);
  v.pos.y+=cellH;
  verticesTri.push_back(v);
  v.pos.x-=cellW;
  verticesTri.push_back(v);
  indicesTri.push_back(nbVertex+0);
  indicesTri.push_back(nbVertex+1);
  indicesTri.push_back(nbVertex+3);
  indicesTri.push_back(nbVertex+1);
  indicesTri.push_back(nbVertex+2);
  indicesTri.push_back(nbVertex+3);
}

void CellGrid::draw(std::vector<Vertex>& verticesTri, std::vector<int>& indicesTri){
  for(auto i: m_liveCells){
    Cell c=m_cells[i.y*m_width+i.x];
    drawCell(verticesTri, indicesTri, c.pos.x, c.pos.y);
  }
}

void CellGrid::drawQuadTree(std::vector<Vertex>& vertices, std::vector<int>& indices, int index){
  m_quadtree->draw(vertices, indices, index);
}

void CellGrid::benchAddFrame(double& _time, double& prevTime){
  double delta=_time-prevTime;
  m_bench.cellCount+=m_liveCells.size();
  m_bench.genCount++;
  m_bench.fps+=1/delta;
  m_bench.genPeriod+=delta;
  m_bench.periodPerCell+=1000*delta/m_bench.gridSize;
  m_bench.periodPerLiveCell+=1000*delta/m_liveCells.size();
  m_bench.frame--;
  if(m_bench.frame<=0){
    std::ofstream benchOut(m_bench.filePath, std::ofstream::app);
    if(benchOut.is_open()){
      time_t ts=time(NULL);
      benchOut<<"-------------------------------------------------------------\n";
      benchOut<<"Benchmark on "<<m_bench.nbFrame<<" frames\n";
      benchOut<<"Date and time: "<<ctime(&ts)<<"\n";
      benchOut<<"Elapsed time: "<<_time-m_bench.startBenchTime<<" seconds\n";
      benchOut<<std::endl;
      benchOut<<"gen count:               "<<m_bench.genCount<<"\n";
      benchOut<<"grid size:               "<<m_bench.gridSize<<"\n";
      benchOut<<"cell count:             ~"<<m_bench.cellCount/m_bench.nbFrame<<"\n";
      benchOut<<std::endl;
      benchOut<<"fps:                    ~"<<m_bench.fps/m_bench.nbFrame<<"\n";
      benchOut<<"gen period:             ~"<<m_bench.genPeriod/m_bench.nbFrame<<" seconds\n";
      benchOut<<"period per cell:        ~"<<m_bench.periodPerCell/m_bench.nbFrame<<" milliseconds\n";
      benchOut<<"period per live cell:   ~"<<m_bench.periodPerLiveCell/m_bench.nbFrame<<" milliseconds\n";
      benchOut<<std::endl;
    }
    benchOut.close();
    std::cout << "Benchmark done.\n";
    m_bench.active=false;
  }
}

void CellGrid::benchmark(double& time, int nbFrame, std::string benchFilePath){
  if(m_bench.active){
    std::cout << "Another benchmark still in progress." << std::endl;
    return;
  }
  m_bench.filePath=benchFilePath;
  m_bench.nbFrame=nbFrame;
  m_bench.frame=nbFrame;
  m_bench.startBenchTime=time;
  m_bench.fps=0;
  m_bench.gridSize=m_cells.size();
  m_bench.cellCount=0;
  m_bench.genCount=0;
  m_bench.genPeriod=0;
  m_bench.periodPerCell=0;
  m_bench.periodPerLiveCell=0;
  m_bench.active=true;
  std::cout<<"benchmark "<<nbFrame<<" frames...\n";
}

void CellGrid::drawInfo(double& time, double& prevTime){
  if(m_bench.active)
    benchAddFrame(time,prevTime);
  if(!m_showInfo)
    return;
  ImGui::Text("fps: %f", 1/(time-prevTime));
  ImGui::Text("genPeriod: %f%s", (time-prevTime), " s");
  ImGui::Text("genCount: %i", m_genCount);
  ImGui::Text("gridSize: %i", (int)m_cells.size());
  ImGui::Text("cellCount: %i", (int)m_liveCells.size());
  ImGui::Text("periodPerCell: %f%s",1000*(time-prevTime)/m_cells.size(), " ms");
  ImGui::Text("periodPerLiveCell: %f%s",1000*(time-prevTime)/m_liveCells.size(), " ms");
}

