#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

template <typename Entity> //
class SparseSet
{
public:
  // template <typename... Args> //
  void construct() {}

  void add(Entity id)
  {
    // add the entity to the end of the packed array
    // m_entity_list.push_back(id);

    // // get the index of the new entity in the packed array
    // std::size_t index = m_entity_list.size() - 1;

    // // update the sparse array to map the entity id to its index in the
    // packed
    // // array
    // m_entity_indices[id] = index;
  }

private:
  std::vector<Entity> sparse;
  std::vector<Entity> dense;
};