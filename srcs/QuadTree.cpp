#include "QuadTree.hpp"
#include <algorithm>
#include <boost/json/kind.hpp>
#include <cstdlib>
#include <glm/fwd.hpp>
#include <memory>
#include <iostream>
#include <string>


int QuadTree::drawTree(std::vector<Vertex>& vertices, std::vector<int>& indices, std::shared_ptr<Leaf> leaf, int index){
  double cW=2.0/160;
  double cH=2.0/90;
  //std::cout << "nbCells: "<<leaf->cellCount<<std::endl;
  Vertex v={
    .pos=glm::vec2(leaf->pos.x*cW-1,leaf->pos.y*cH-1),
    .color=glm::vec3(0.1,0.7,0.2)
  };
  if(leaf->hasChild){
    for(auto l: leaf->childrens)
      index=drawTree(vertices, indices, l, index);
  }
  vertices.push_back(v);
  v.pos.x+=leaf->size*cW;
  vertices.push_back(v);
  v.pos.y+=leaf->size*cH;
  vertices.push_back(v);
  v.pos.x-=leaf->size*cW;
  vertices.push_back(v);
  indices.push_back(index);
  indices.push_back(index+1);
  indices.push_back(index+1);
  indices.push_back(index+2);
  indices.push_back(index+2);
  indices.push_back(index+3);
  indices.push_back(index+3);
  indices.push_back(index);
  return index+4;
}

void QuadTree::draw(std::vector<Vertex>& vertices, std::vector<int>& indices, int index){
  drawTree(vertices, indices, m_root, index);
}


std::shared_ptr<Leaf> QuadTree::getChildAt(glm::ivec2 pos, std::shared_ptr<Leaf> l){
  unsigned int s=l->size>>1;
  glm::ivec2 p=abs(l->pos-pos);
  int x=(int)(p.x/s);
  int y=(int)(p.y/s);
  if(2*y+x>3)
    return nullptr;
  return l->childrens[2*y+x];
}

void QuadTree::addCellAt(glm::ivec2 pos){
  m_root->addCellAt(pos, m_aggregate);
}
/*
void QuadTree::addCellAt(glm::ivec2 pos){
  std::shared_ptr<Leaf> l=m_root;
  while(l!=nullptr && l->size>LEAF_SIZE_DEFAULT_MIN){
    if(!l->hasChild){
      unsigned int s=l->size>>1;
      l->hasChild=true;
      l->childrens.push_back(std::make_shared<Leaf>(l->pos, s, l));
      l->childrens.push_back(std::make_shared<Leaf>(l->pos+glm::ivec2(s,0), s, l));
      l->childrens.push_back(std::make_shared<Leaf>(l->pos+glm::ivec2(0,s), s, l));
      l->childrens.push_back(std::make_shared<Leaf>(l->pos+glm::ivec2(s,s), s, l));
    }
    l=getChildAt(pos, l);
  }
  if(l==nullptr)
    return;
  l->cellCount++;
  auto parent=l->parent;
  while(parent!=nullptr){
    parent->cellCount++;
    parent=parent->parent;
  }
}
*/

std::shared_ptr<Leaf> QuadTree::getLeafAt(glm::ivec2 pos){
  std::shared_ptr<Leaf> l=m_root;
  
  while(l!=nullptr && l->size>LEAF_SIZE_DEFAULT_MIN && l->hasChild)
    l=getChildAt(pos, l);
  if(l!=nullptr && l->size>LEAF_SIZE_DEFAULT_MIN)
    return nullptr;
  return l;
}

void QuadTree::deleteCellAt(glm::ivec2 pos){
  auto l=getLeafAt(pos);
  if(l==nullptr)
    return;
  if(l->cellCount<=0)
    return;
  l->cellCount--;
  auto parent=l->parent;
  while(parent!=nullptr){
    parent->cellCount--;
    int cnt=parent->childrens[0]->cellCount;
    cnt+=parent->childrens[1]->cellCount;
    cnt+=parent->childrens[2]->cellCount;
    cnt+=parent->childrens[3]->cellCount;
    if(cnt>0)
      return;
    else{
      parent->childrens.clear();
      parent->hasChild=false;
      parent=parent->parent;
    }
  }
}

void Leaf::addCellAt(glm::ivec2 pos, std::vector<std::shared_ptr<Leaf>>& aggregate){
  if(size>LEAF_SIZE_DEFAULT_MIN){
    unsigned int s=size>>1;
    if(!hasChild){
      hasChild=true;
      childrens.push_back(std::make_shared<Leaf>(this->pos, s));
      childrens.push_back(std::make_shared<Leaf>(this->pos+glm::ivec2(s,0), s));
      childrens.push_back(std::make_shared<Leaf>(this->pos+glm::ivec2(0,s), s));
      childrens.push_back(std::make_shared<Leaf>(this->pos+glm::ivec2(s,s), s));
    }
    glm::ivec2 p=abs(this->pos-pos);
    int x=(int)(p.x/s);
    int y=(int)(p.y/s);
    if(2*y+x>3)
      return;
    return childrens[2*y+x]->addCellAt(pos, aggregate);
  }
  else{
    cellCount++;
    int x=pos.x-this->pos.x;
    int y=pos.y-this->pos.y;
    content[y*2+x].state=1;
    if(!isPushed){
      aggregate.push_back(std::shared_ptr<Leaf>(this));
      isPushed=true;
    }
    std::cout<<"Adding cell in buffer at : "<<this->pos.x<<","<<this->pos.y<<std::endl;
  }
}

void Leaf::nextGen(std::shared_ptr<Leaf> rootBuffer, std::vector<std::shared_ptr<Leaf>>& aggregate){
  std::cout<<"Branch nextGen : "<<pos.x<<","<<pos.y<<std::endl;
  if(hasChild){
    for(auto s: childrens)
      s->nextGen(rootBuffer, aggregate);
  }
  else if(content.size()!=0){
    std::cout<<"found content in leaf: "<<pos.x<<","<<pos.y<<std::endl;
    for(auto& c: content){
      int nbAlive=0;
      for(int i=c.pos.x-1;i<c.pos.x+1;i++){
        for(int j=c.pos.y-1;j<c.pos.y+1;j++){
          int x=i-pos.x;
          int y=j-pos.y;
          if(x<0 || x>size || y<0 || y>size)
            continue;
          if(content[y*size+x].state>0)
            nbAlive++;
        }
      }
      if(c.state==0 && nbAlive==3){
        rootBuffer->addCellAt(c.pos, aggregate);
        std::cout<<"addCell call "<<c.pos.x<<","<<c.pos.y<<std::endl;
      }
      else if(c.state>0 && (nbAlive==2 || nbAlive==3))
        rootBuffer->addCellAt(c.pos, aggregate);
    }
    
  }
}

void QuadTree::nextGen(){
  std::cout<<"Quadtree nextGen\n";
  m_rootBuffer=std::make_shared<Leaf>(glm::ivec2(0,0), LEAF_SIZE_DEFAULT_MAX);
  m_aggregateBuffer.clear();
  for(auto l: m_aggregate){
    std::cout<<"processing leaf\n";
    l->nextGen(m_rootBuffer, m_aggregateBuffer);
  }
  std::cout<<"swapping tree buffers\n";
  m_root.swap(m_rootBuffer);
  m_aggregate.swap(m_aggregateBuffer);
}

void QuadTree::aggregateLeaf(){
  m_aggregate.clear();
  auto l=m_root;
  while(l->hasChild){
    if(l->size>>1==LEAF_SIZE_DEFAULT_MIN){
      for(auto s: l->childrens){
        if(s->size==LEAF_SIZE_DEFAULT_MIN){
          //for(auto& c: s->content)
            //m_aggregate.insert({std::to_string(c.pos.x)+","+std::to_string(c.pos.y), &c});
        }
      }
    }
  }
}
/*

void QuadTree::updateTree(){
  aggregateLeaf();
  for(auto& p: m_aggregate){
    Cell* c=p.second;
    int nbAlive=0;
    for(int i=c->pos.x-1;i<=c->pos.x+1;i++){
      for(int j=c->pos.y-1;j<=c->pos.y+1;j++){
        if(i==c->pos.x && j==c->pos.y)
          continue;
        if(getLeafAt(glm::ivec2(i,j))==nullptr)
          continue;
        std::string key=std::to_string(i)+","+std::to_string(j);
        if(m_aggregate[key]->state>0)
          nbAlive++;
      }
    }
    if(c->state && (nbAlive>=3 || nbAlive<=2)){
      c->state=0;
      deleteCellAt(c->pos);
    }
    else if(c->state==0 && nbAlive==3){
      c->state=1;
      addCellAt(c->pos);
    }
  }
}*/
