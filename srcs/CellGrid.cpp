#include "CellGrid.hpp"
#include "CAParser.hpp"
#include "RuleSet.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <glm/fwd.hpp>
#include <hephaestus/memory/hephMemoryAllocator.hpp>
#include <string>
#include <vector>



CellGrid::CellGrid(int width, int height){
  m_width=width;
  m_height=height;
  for(int j=0;j<height;j++){
    for(int i=0;i<width;i++){
      m_cells.push_back(Cell(i,j));
    }
  }
  RuleSet rs;
  CAParser::parseCA("./wireworld.ca", rs);

  std::string s="";
  auto a=stringSplit(s, 'a');
  for(auto d:a)std::cout<<d<<std::endl;
}

void CellGrid::setState(int x, int y, CellState state){
  auto it=std::find(m_liveCells.begin(), m_liveCells.end(), glm::ivec2(x,y));
  if(it==m_liveCells.end() && state==CELL_STATE_1){
    m_liveCells.push_back(glm::ivec2(x,y));
  }
  else if(it!=m_liveCells.end() && state==CELL_STATE_0){
    m_liveCells.erase(it);
  }
  //std::cout << "setting " << x << "," << y << " state to" << state << std::endl;
  m_cells[y*m_width+x].state=state;
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
    //std::cout << _x << "," << _y << std::endl;
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
      CellState state=(c=='b')?CELL_STATE_0:CELL_STATE_1;
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
  std::cout << "calc next gen\n";
  m_prevCells=m_cells;

  for(auto& c: m_cells){
    applyRuleSet(c, m_prevCells);
  }
  m_genCount++;
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

void CellGrid::activateCell(int x, int y){
  m_cells[y*m_width+x].state=CELL_STATE_1;
  m_liveCells.push_back(glm::ivec2(x,y));
}

void CellGrid::switchCell(int x, int y){
  int ind=y*m_width+x;
  Cell& c=m_cells[ind];
  if(c.state==CELL_STATE_1){
    c.state=CELL_STATE_0;
    m_liveCells.erase(std::find(m_liveCells.begin(), m_liveCells.end(), c.pos));
  }
  else{
    c.state=CELL_STATE_1;
    m_liveCells.push_back(c.pos);
  }
}

void CellGrid::drawCell(std::vector<Vertex>& verticesTri, std::vector<int>& indicesTri, int x, int y){
  float cellH=2.0/m_height;
  float cellW=2.0/m_width;
  int nbVertex=verticesTri.size();
  Vertex v={
    .pos=glm::vec2(x*cellW-1,y*cellH-1),
    .color=glm::vec3(0.6,0.8,0.5)
  };
  //std::cout << x << " , " << y << std::endl << cellSize << " : " << nbVertex << std::endl;
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
    //std::cout << c.pos.x << " , " << c.pos.y << "  :  " << c.state << std::endl;
    drawCell(verticesTri, indicesTri, c.pos.x, c.pos.y);
  }

}

