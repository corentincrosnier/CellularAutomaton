#pragma once
#include "Cell.hpp"


enum RangePointer{
  RANGE_ANYWHERE,
};

struct CACondition{
  bool virtual      satisfy();
  int               regionId;
  std::vector<int>  stateId;
  int               lowBound;
  int               highBound;
};
struct CAC_Always: CACondition{
  bool satisfy() override{return true;}
};

struct CAExpression{
  CAExpression*     condition;
};

struct CATransition{
  CACondition       condition;
  std::vector<int>  stateStartId;
  int               stateEndId;
};

struct CARegion{
  CARegion(){}
  CARegion(std::vector<glm::ivec2> relCells){relativeCells=relCells;}
  std::vector<glm::ivec2>   relativeCells; 
  int                       cellCount;
};

struct RuleSet{
  void  applyRuleSet(Cell& cell);

  int                         stateCount;
  std::vector<CARegion>       regions;
  std::vector<CATransition>   transitions;
  std::string                 name;
};
