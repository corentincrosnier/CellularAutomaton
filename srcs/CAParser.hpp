#pragma once
#include "CellGrid.hpp"
#include "RuleSet.hpp"
#include <iostream>
#include <string>


void                       removeWS(std::string& s);
std::vector<std::string>   stringSplit(std::string s, char delimiter);

class CAParser{
  public:
    /*
    CAParser(){}
    CAParser(std::string filepathCA){m_filepathCA=filepathCA;}
    CAParser(std::string filepathCA, RuleSet& ruleset){
      std::cout << "Creating parser\n";
      m_filepathCA=filepathCA;
      parseCA(ruleset);
    }
    */

    static void             parseRLE(std::string filepathRLE, CellGrid& grid);
    static void             parseCA(RuleSet& ruleset);
    static void             parseCA(std::string filepathCA, RuleSet& ruleset){m_filepathCA=filepathCA;parseCA(ruleset);}
    static CARegion         parseRegion(std::string l);
    static glm::ivec2       parseVec(std::string s);
    static CATransition     parseTransition(std::string l);
    static CACondition      parseCondition(std::string s);

  protected:
    inline static std::string   m_filepathCA;
    inline static RuleSet*      m_ruleset;
};
