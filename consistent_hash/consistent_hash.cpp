#include "consistent_hash.h"

// the hash ring size is 0 ~ 2**32
#define HASH_LEN 32

unsigned int MurMurHash(const void *key, int len) {
  const unsigned int m = 0x5bd1e995;
  const int r = 24;
  const int seed = 97;
  unsigned int h = seed ^ len;
  // Mix 4 bytes at a time into the hash
  const unsigned char *data = (const unsigned char *)key;
  while (len >= 4) {
    unsigned int k = *(unsigned int *)data;
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
    data += 4;
    len -= 4;
  }
  // Handle the last few bytes of the input array
  switch (len) {
  case 3:
    h ^= data[2] << 16;
  case 2:
    h ^= data[1] << 8;
  case 1:
    h ^= data[0];
    h *= m;
  };
  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
}


unsigned int consistent_hash::find_nearest_node_simple(
    unsigned int
        hash_value) { // find the nearest virtual node for given hash position
  int low = 0;
  int high = sorted_node_hash_list.size() - 1;
  int mid;
  if (hash_value > sorted_node_hash_list[high]) {
    return 0;
  }
  while (low < high) {
    mid = (low + high) / 2;
    if (sorted_node_hash_list[mid] == hash_value) {
      return mid;
    } else if (sorted_node_hash_list[mid] > hash_value) {
      high = mid;
    } else { // sorted_node_hash_list[mid]<data_hash
      low = mid + 1;
    }
  }
  return low;
}

consistent_hash::consistent_hash() {
  real_node_sum = 0;
  virtual_node_sum = 0;
}

consistent_hash::~consistent_hash() {
  virtual_node_map.clear();
  real_node_map.clear();
  sorted_node_hash_list.clear();
}

unsigned int consistent_hash::find_nearest_node(
    unsigned int
        hash_value) { // find the nearest virtual node for given hash position
  int low = 0;
  int high = sorted_node_hash_list.size() - 1;
  int mid;
  if (hash_value > sorted_node_hash_list[high]) {
    return 0;
  }
  while (low < high) {
    mid = (low + high) / 2;
    if (sorted_node_hash_list[mid] == hash_value) {
      return mid;
    } else if (sorted_node_hash_list[mid] > hash_value) {
      high = mid;
    } else { // sorted_node_hash_list[mid]<data_hash
      low = mid + 1;
    }
  }
  return low;
}

std::pair<unsigned int, unsigned int>
consistent_hash::look_up(const std::string &content) {
  // return the ip of virtual node that serve this content
  auto hash_position = MurMurHash(content.c_str(), content.length());
  auto virtual_node_index = find_nearest_node(hash_position);
  virtual_node_index++;
  if (virtual_node_index >= sorted_node_hash_list.size()) { // cross the zero
    virtual_node_index = 0;
  }

  // Peixuan 10262020: File : vnode and rnode map:

  fileID_vnode_map[content] = virtual_node_index;
  fileID_rnode_map[content] =
      virtual_node_map.find(sorted_node_hash_list[virtual_node_index])
          ->second.cache_index;

  return std::pair<unsigned int, unsigned int>(
      virtual_node_index,
      virtual_node_map[sorted_node_hash_list[virtual_node_index]].cache_index);
}

void consistent_hash::add_real_node(std::string ip,
                                    unsigned int virtual_node_num) {
  std::cout << "[add_real_node]\t" << ip << std::endl;
  real_node *node;
  if (real_node_map.find(ip) !=
      real_node_map.end()) { // this real node has added before
    node = &real_node_map[ip];
  } else {
    real_node new_node = real_node(ip);
    node = &new_node;
    real_node_sum++;
  }
  int cur_port = node->cur_max_port;
  unsigned int vir_node_num = 0;
  std::string tmp_ip;    // the ip of virtual node
  unsigned int tmp_hash; // the position of virtual node on hash ring
  while (vir_node_num < virtual_node_num) {
    do { // find a  virtual node and avoid collision
      cur_port++;
      tmp_ip = ip + ":" + std::to_string(cur_port);
      tmp_hash = MurMurHash(tmp_ip.c_str(),
                            tmp_ip.length());       
    } while (virtual_node_map.find(tmp_hash) != virtual_node_map.end());
    vir_node_num++;
    virtual_node_map[tmp_hash] =
        virtual_node(tmp_ip, tmp_hash, real_node_sum - 1);
    sorted_node_hash_list.push_back(tmp_hash);
    sort(sorted_node_hash_list.begin(), sorted_node_hash_list.end());
    unsigned int id = find_nearest_node(tmp_hash);
    unsigned int next_id = id + 1;
    if (next_id >= sorted_node_hash_list.size()) { // cross the zero
      next_id = 0;
    }
  }
  node->cur_max_port = cur_port;
  node->virtual_node_num += virtual_node_num;
  real_node_map[ip] = *node;

  virtual_node_sum += virtual_node_num;
  std::cout << "[add_real_node finished]\t" << ip << std::endl << std::endl;
}