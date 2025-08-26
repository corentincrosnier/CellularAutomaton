#include <vector>
#include <string>

class SubSpace;

struct IPartitioner{
  virtual std::vector<SubSpace>   partition(SubSpace& subspace) = 0;
  virtual std::vector<SubSpace&>  surrounding_partitions() = 0;
  virtual SubSpace&               partition_at(std::string pointer_str) = 0;
};

class SubSpace{
  public:

    void  parseTree(SubSpace& partition);

  protected:


    int                   depth = 0;
    std::vector<SubSpace> partitions;
    IPartitioner*         partitioner;
};
