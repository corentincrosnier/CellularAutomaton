#include "CAParser.hpp"
#include "RuleSet.hpp"
#include <cstddef>
#include <fstream>
#include <glm/fwd.hpp>
#include <iostream>
#include <memory>
#include <string>


CARegion CAParser::parseRegion(std::string s){
  CARegion r;
  int vecStart=s.find('(');
  int vecEnd=s.find(')');
  int nbCell=0;
  
  while(vecStart!=std::string::npos){
    nbCell++;
    r.relativeCells.push_back(parseVec(s.substr(vecStart+1,vecEnd-vecStart-1)));
    s.erase(vecStart,vecEnd-vecStart+1);
    vecStart=s.find('(');
    vecEnd=s.find(')');
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
  //std::cout << x << "," << y << std::endl;
  return glm::ivec2(x,y);
}

CATransition CAParser::parseTransition(std::string s){
  int i=s.find('s');
  CATransition trans;
  /*
  if(i==std::string::npos){
    std::cout << "error while parsing transition (" << s << ")\n";
    return CATransition();
  }*/

  int defStart=s.find('{');
  int defEnd=s.find('}');
  if(defStart==std::string::npos){
    std::cout << "error while parsing transition (" << s << ")\n";
    return CATransition();
  }
  trans.stateStartId=std::stoi(s.substr(1,defStart-1));
  trans.stateEndId=std::stoi(s.substr(defEnd+1,s.size()-defEnd-1));
  std::string sub=s.substr(defStart+1,defEnd-defStart-1);
  if(sub.empty()){
    trans.condition=CAC_Always();
    trans.always=true;
  }
  else
    trans.condition=parseCondition(sub);

  return trans;
}

CACondition CAParser::parseCondition(std::string s){
  if(s.empty()){
    std::cout<<"condition {"<<s<<"} will always be satisfied\n";
    return CAC_Always();
  }
  CACondition cond;
  auto args=stringSplit(s, ':');
  if(args.size()!=3){
    std::cout << "error while parsing Condition {" << s << "}\n";
    std::cout << "A condition requires either 0 or 3 arguments\n";
    return cond;
  }
  cond.region=std::make_shared<CARegion>(m_ruleset->regions[std::stoi(args[0])]);
  cond.stateId.push_back(std::stoi(args[1]));
  int i=args[2].find('(');
  int j=args[2].find(')');
  if(i==std::string::npos || j==std::string::npos){
    std::cout << "Error while parsing Condition {" <<s<<"}, requires range '(x,y)' as 3rd argument\n";
    return cond;
  }
  auto range=stringSplit(args[2].substr(i+1,j-i-1), ',');
  cond.lowBound=std::stoi(range[0]);
  if(range.size()==1)
    cond.highBound=cond.lowBound;
  else if(range[1]=="$")
    cond.highBound=cond.region->cellCount;
  else
   cond.highBound=std::stoi(range[1]);
  return cond;
}

std::shared_ptr<RuleSet> CAParser::parseCA(){
  m_ruleset=std::make_shared<RuleSet>();
  std::ifstream f(m_filepathCA);
  if(!f.is_open()){
    std::cout << "couldn't open " << m_filepathCA << " for CAParser, aborting\n";
    return nullptr;
  }

  std::string l;
  while(std::getline(f, l)){
    stringEraseComment(l, '#');
    if(l.empty())
      continue;
    stringEraseWS(l);
    switch(l.at(0)){
      //case '#':
      //  continue;
      case 'n':
        l.erase(0,1);
        m_ruleset->stateCount=std::stoi(l);
        break;
      case 'r':
        m_ruleset->regions.push_back(parseRegion(l));
        break;
      case 't':
        m_ruleset->transitions.push_back(parseTransition(l));
        break;
    }
    //std::cout << l << std::endl;
  }
  f.close();
  return m_ruleset;
}

void stringEraseWS(std::string& s){
  int i=s.find(' ');
  while(i!=std::string::npos){
    s.erase(i,1);
    i=s.find(' ');
  }
}

void stringEraseComment(std::string& s, char token){
  int i=s.find(token);
  if(i!=std::string::npos)
    s.erase(i);
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
