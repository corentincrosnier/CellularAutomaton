#pragma once
#include "Cell.hpp"
#include <glm/fwd.hpp>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unordered_map>


#define LEAF_SIZE_DEFAULT_MAX   128
#define LEAF_SIZE_DEFAULT_MIN   4
#define LEAF_SIZE_INF           -1

struct vec{
  int x;
  int y;
};

class Leaf{
  public:
    Leaf(){}
    Leaf(glm::ivec2 pos, unsigned int size, std::shared_ptr<Leaf> parent=nullptr){
      this->pos=pos;
      this->size=size;
      this->parent=parent;
      if(size==LEAF_SIZE_DEFAULT_MIN){
        for(int i=0;i<size*size;i++)
          content.push_back( Cell(i%size, (int)(i/size)) );
        for(int i=0;i<4*(size+1);i++){
          
        }
      }
    }
    void                                  nextGen(std::shared_ptr<Leaf> rootBuffer, std::vector<std::shared_ptr<Leaf>>& aggregate);
    void                                  addCellAt(glm::ivec2 pos, std::vector<std::shared_ptr<Leaf>>& aggregate);

    bool                                  isPushed=false;
    std::shared_ptr<Leaf>                 parent;
    int                                   cellCount=0;
    glm::ivec2                            pos;
    unsigned int                          size=LEAF_SIZE_INF;
    int                                   depth=0;
    bool                                  hasChild=false;
    std::vector<std::shared_ptr<Leaf>>    childrens;
    std::vector<Cell>                     content;
    //std::unordered_map<std::string, Cell>                     content;
    std::vector<Cell>                     surrounding;
};


class QuadTree{
  public:
    QuadTree(){
      m_root=std::make_unique<Leaf>(glm::ivec2(0,0), LEAF_SIZE_DEFAULT_MAX);
    }
    void                  nextGen();
    std::shared_ptr<Leaf> getChildAt(glm::ivec2 pos, std::shared_ptr<Leaf> l);
    std::shared_ptr<Leaf> getLeafAt(glm::ivec2 pos);
    void                  addCellAt(glm::ivec2 pos);
    void                  deleteCellAt(glm::ivec2 pos);
    void                  updateTree();
    std::vector<Cell>     makeTree(std::vector<Cell>& liveCells);
    int                   drawTree(std::vector<Vertex>& vertices, std::vector<int>& indices, std::shared_ptr<Leaf> leaf, int index);
    void                  draw(std::vector<Vertex>& vertices, std::vector<int>& indices, int index);
    void                  aggregateLeaf();

  protected:
    unsigned int                            depth=0;
    std::vector<std::shared_ptr<Leaf>>      m_aggregate;
    std::vector<std::shared_ptr<Leaf>>      m_aggregateBuffer;
    std::shared_ptr<Leaf>                   m_root;
    std::shared_ptr<Leaf>                   m_rootBuffer;
};

