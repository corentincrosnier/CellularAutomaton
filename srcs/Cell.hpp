#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

#define CELL_STATE_0 0
#define CELL_STATE_1 1
#define CELL_STATE_2 2
#define CELL_STATE_3 3
#define CELL_STATE_4 4
#define CELL_STATE_5 5

struct Cell{
  Cell(int x, int y, int state=CELL_STATE_0){
    pos=glm::ivec2(x,y);
    this->state=state;
  }

  int         state;
  glm::ivec2  pos;
};
