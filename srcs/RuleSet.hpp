#pragma once
#include "Cell.hpp"


enum RangePointer{
  RANGE_ANYWHERE,
};

struct CACondition{
  bool virtual      satisfy(){return true;}
  int               regionId=-1;
  int               lowBound=-1;
  int               highBound=-1;
  std::vector<int>  stateId;
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
