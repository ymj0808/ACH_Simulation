#ifndef CONSISTENT_HASH_H
#define CONSISTENT_HASH_H

#include "node.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

class consistent_hash {
public:
  int real_node_sum;
  unsigned int virtual_node_sum;
  std::map<std::string, real_node> real_node_map;
  std::map<unsigned int, virtual_node>
      virtual_node_map; // hash position to virtual node
  std::map<unsigned int, unsigned int> uid_map_hash_value; // uid to hash_value
  std::map<unsigned int, virtual_node>::iterator
      iter; // debug iterator 10122020Peixuan
  
  std::vector<unsigned int>
      sorted_node_hash_list; // the position of each virtual node on hash ring

  std::map<std::string, unsigned int>
      fileID_vnode_map; // Peixuan 10262020: File ID to virtual node map
  std::map<std::string, unsigned int>
      fileID_rnode_map; // Peixuan 10262020: File ID to real node map

  consistent_hash();

  ~consistent_hash();

  std::pair<unsigned int, unsigned int> look_up(const std::string &content);

  unsigned int find_nearest_node(unsigned int hash_value);

  // unsigned int put(std::string data_id);

  void add_real_node(std::string ip, unsigned int virtual_node_num);

  unsigned int find_nearest_node_simple(unsigned int hash_value);
};

#endif // LEETCODE_MY_HASH_H
