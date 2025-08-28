#pragma once
#include "Cell.hpp"
#include <glm/detail/qualifier.hpp>
#include <memory>


enum RangePointer{
  RANGE_ANYWHERE,
};

struct CAStateInfo{
  static void addState(std::string name, glm::vec3 color){
    colors.push_back(color);
    names.push_back(name);
  } 
  static inline int                   stateCount=0;
  static std::vector<glm::vec3>       colors;
  static std::vector<std::string>     names;
};

struct CARegion{
  CARegion(){}
  CARegion(std::vector<glm::ivec2> relCells){relativeCells=relCells;}
  std::vector<glm::ivec2>   relativeCells; 
  int                       cellCount;
  int                       id;
};

struct CACondition{
  bool virtual                  satisfy(std::vector<int> tally);
  std::shared_ptr<CARegion>     region;
  int                           lowBound=-1;
  int                           highBound=-1;
  std::vector<int>              stateId;
};

struct CAC_Always: public CACondition{
  bool satisfy(std::vector<int> tally) override{return true;}
};

struct CAExpression{
  CAExpression*     condition;
};

struct CATransition{
  bool              satisfy(std::vector<int> tally);
  CACondition       condition;
  bool              always=false;
  int               stateStartId;
  int               stateEndId;
  int               id;
};

struct RuleSet{
  void                        applyRuleSet(Cell& cell);
  int                         stateCount;
  std::vector<CARegion>       regions;
  std::vector<CATransition>   transitions;
  std::string                 name;
};
