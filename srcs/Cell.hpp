#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>


enum CellState{
  CELL_STATE_0,
  CELL_STATE_1
};

struct Cell{
  Cell(int x, int y, CellState state=CELL_STATE_0){
    pos=glm::ivec2(x,y);
    this->state=state;
  }

  CellState   state;
  glm::ivec2  pos;
};
