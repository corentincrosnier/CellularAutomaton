#pragma once
#include "CellGrid.hpp"
#include "RuleSet.hpp"
#include <iostream>
#include <memory>
#include <string>


void                        stringEraseWS(std::string& s);
void                        stringEraseComment(std::string& s, char token);
std::vector<std::string>    stringSplit(std::string s, char delimiter);

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

    static void                                 parseRLE(std::string filepathRLE, CellGrid& grid);
    static void                                 parseCA(RuleSet& ruleset);
    static std::shared_ptr<RuleSet>             parseCA();
    static std::shared_ptr<RuleSet>             parseCA(std::string filepathCA){m_filepathCA=filepathCA;return parseCA();}
    static CARegion                             parseRegion(std::string s);
    static glm::ivec2                           parseVec(std::string s);
    static CATransition                         parseTransition(std::string s);
    static CACondition                          parseCondition(std::string s);

  protected:
    inline static std::string                   m_filepathCA;
    inline static std::shared_ptr<RuleSet>      m_ruleset;
};
