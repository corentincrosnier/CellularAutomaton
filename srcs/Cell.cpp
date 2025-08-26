#include "Cell.hpp"

/*
void cell::nextGen(CellGrid& grid, std::vector<cell*>& liveCells){
  int nbAlive;
  for(int x=m_pos.x-range;x<=m_pos.x+range;x++){
    for(int y=m_pos.y-range;y<=m_pos.y+range;y++){
      if(x<0 || x>=m_grid.m_width || y<0 || y>=m_grid.m_height)
        continue;
      if(x==m_pos.x && y==m_pos.y)
        continue;
      if(grid.cells[x][y].state==CELL_STATE_1)
        nbAlive++;
    }
  }
  if(state==CELL_STATE_0 && nbAlive==3)
    state=CELL_STATE_1;
  else{
    if(nbAlive<2)
      state=CELL_STATE_0;
    else if (nbAlive>3)
      state=CELL_STATE_0;
  }
  if(state==CELL_STATE_1){
    liveCells.push_back(this);
  }
}*/
