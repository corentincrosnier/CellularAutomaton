#include "CAParser.hpp"
#include "RuleSet.hpp"
#include <cstddef>
#include <fstream>
#include <glm/fwd.hpp>
#include <iostream>
#include <string>


CARegion CAParser::parseRegion(std::string l){
  CARegion r;
  int vecStart=l.find('(');
  int vecEnd=l.find(')');
  int nbCell=0;
  
  while(vecStart!=std::string::npos){
    nbCell++;
    r.relativeCells.push_back(parseVec(l.substr(vecStart+1,vecEnd-vecStart-1)));
    l.erase(vecStart,vecEnd-vecStart+1);
    vecStart=l.find('(');
    vecEnd=l.find(')');
  }
  r.cellCount=nbCell;
  return r;
}

glm::ivec2 CAParser::parseVec(std::string s){
  int i=s.find(',');
  if(i==std::string::npos){
    std::cout << "error while parsing vector (" << s << ") defaulted to (0,0)\n";
    return glm::ivec2(0,0);
  }
  int x=std::stoi(s.substr(0,i));
  int y=std::stoi(s.substr(i+1,s.size()-i-1));
  std::cout << x << "," << y << std::endl;
  return glm::ivec2(x,y);
}

CATransition CAParser::parseTransition(std::string l){
  int i=l.find('s');
  if(i==std::string::npos){
    std::cout << "error while parsing transition (" << l << ")\n";
    return CATransition();
  }

  int defStart=l.find('{');
  int defEnd=l.find('}');
  if(defStart==std::string::npos){
    std::cout << "error while parsing transition (" << l << ")\n";
    return CATransition();
  }
  std::string sub=l.substr(defStart,defEnd-defStart-1);
  if(sub.empty()){
    return CAC_Always();
  }

  return CATransition();
}

CACondition CAParser::parseCondition(std::string s){
  CACondition cond;
  auto args=stringSplit(s, ':');
  if(args.size()!=5){
    std::cout << "error while parsing Condition {" << s << "}\n";
    return CACondition();
  }
  cond.startState=std::stoi(args[0]);
  cond.region=std::stoi(args[1]);
  cond.endState=std::stoi(args[4]);
}

void CAParser::parseCA(RuleSet& ruleset){
  m_ruleset=&ruleset;
  std::ifstream f(m_filepathCA);
  if(!f.is_open()){
    std::cout << "couldn't open " << m_filepathCA << " for CAParser, aborting\n";
    m_ruleset=NULL;
    return;
  }

  std::string l;
  while(std::getline(f, l)){
    if(l.empty())
      continue;
    removeWS(l);
    switch(l.at(0)){
      case '#':
        continue;
      case 'n':
        l.erase(0,1);
        ruleset.nbState=std::stoi(l);
        break;
      case 'r':
        ruleset.regions.push_back(parseRegion(l));
        break;
      case 't':
        ruleset.transitions.push_back(parseTransition(l));
        break;
    }
    std::cout << l << std::endl;
  }
  f.close();
  m_ruleset=NULL;
}

void removeWS(std::string& s){
  int i=s.find(' ');
  while(i!=std::string::npos){
    s.erase(i,1);
    i=s.find(' ');
  }
}

std::vector<std::string> stringSplit(std::string s, char delimiter){
  std::vector<std::string> ret;
  if(s.empty())
    return ret;
  int i=s.find(delimiter);
  int _i=0;
  std::string sub;
  while(i!=std::string::npos){
    sub=s.substr(_i,i-_i);
    if(!sub.empty())
      ret.push_back(sub);
    _i=i+1;
    i=s.find(delimiter, _i);
  }
  if(_i==0)
    ret.push_back(s);
  else{
    sub=s.substr(_i,s.size()-_i);
    if(!sub.empty())
      ret.push_back(sub);
  }
  return ret;
}
