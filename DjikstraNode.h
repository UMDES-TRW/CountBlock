#ifndef _DJIKSTRANODE_H_
#define _DJIKSTRANODE_H_

#include <SmartStack.h> //For StateIndexType

struct DjikstraNode
{
private:
  ///Default constructor is kept private
  DjikstraNode(){};

public:
  ///@brief The parent state
  StateIndexType parent;
    
  ///@brief The event from the parent state
  std::string event;

  /*
   * @brief Constructor
   * @param p The parent state
   * @param e The event from parent state
   */
  DjikstraNode(StateIndexType p, std::string e)
    :parent(p), event(e){};   
};

///@brief Used to store
struct StateIndexWithPathCost
{
private:
  ///@Default constructor is kept private
  StateIndexWithPathCost(){};

public:
  ///@brief The concerned composite state
  StateIndexType state;

  ///@brief The number of transitions used to reach this state
  int pathlength; 

  ///@brief Constructor
  StateIndexWithPathCost(StateIndexType s, int p)
    :state(s),pathlength(p){};

  ///@brief Operator for comparison
  bool operator<(const StateIndexWithPathCost & other) const
  {
    return (other.pathlength < this->pathlength);
  };
};

#endif
