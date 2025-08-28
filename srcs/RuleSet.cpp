#include "RuleSet.hpp"
#include <vector>
#include <iostream>


bool CACondition::satisfy(std::vector<int> tally){
  int cnt=0;
  std::cout<<"adding tally for states: ";
  for(auto i: stateId)
    std::cout<<i<<",";
  std::cout<<"\n";
  for(auto sid: stateId){
    std::cout << tally[sid]<<",";
    cnt+=tally[sid];
  }
  std::cout<<"\n";
  std::cout<<"counted "<<cnt<<" cells for condition "<<std::endl;
  if(cnt>=lowBound && cnt<=highBound)
    return true;
  else
    return false;
}

bool CATransition::satisfy(std::vector<int> tally){
  return (always)?true:condition.satisfy(tally);
}
