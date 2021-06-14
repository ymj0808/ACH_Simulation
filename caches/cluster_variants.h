#ifndef CLUSTER_VARIENTS_H
#define CLUSTER_VARIENTS_H

#include "../consistent_hash/consistent_hash.h"
#include "../double_queue_node/double_queue_node.h"
#include "cache.h"
#include "cache_object.h"
#include "lru_cache.h"
#include <list>
#include <random>
#include <set>
#include <unordered_map>
#include <chrono>
#include <queue>
#include "CpuMonitor.cpp"

/*
    this file defines cluster-based cache policy
*/

/*
    Consistent hash
*/

class CHCache : public Cache {
protected:
    int cache_number;
    bool printMap;
    LRUCache* caches_list; // LRUCaches cluster
    // std::unordered_map<CacheObject, uint8_t> mapper; // map CacheObjec to
    // LRUCache
    consistent_hash chash;
    int virtual_node;

    CpuMonitor cpu_mon; // 0118202 Peixuan

public:
    CHCache() : Cache() {}

    virtual ~CHCache() {}

    virtual void setPar(std::string parName, std::string parValue);
    void init_mapper();
    virtual bool lookup(SimpleRequest* req);
    virtual void admit(SimpleRequest* req);
    virtual void evict(SimpleRequest* req) {};
    virtual void evict() {};
    bool request(SimpleRequest* req);
    void printReqAndFileNum(); // print request number and file number

    uint32_t window_size = 10000;
    uint32_t position = 0;
};

static Factory<CHCache> factoryCH("CH");


/*
    Adaptive Consistent hash
*/

class ACHCache : public Cache {
protected:
    int cache_number;     // cache number in cluster
    uint32_t window_size; // for each window_size gap, run optimizer
    int virtual_node;     // how many virtual node for each real node
    double alpha;
    uint32_t threshold;
    std::chrono::steady_clock::time_point time;
    uint64_t m = 0;        // unique content number
    LRUCache* caches_list; // LRUCaches cluster
    // FilterCache *caches_list;   // LRUCaches cluster
    uint64_t* request_count; // request number of each server in a window
    uint64_t* hit_count;
    uint64_t* miss_count;
    double* miss_rate;
    double* usage_ratio;
    double* rank;
    std::vector<std::list<uint32_t>> vnode_index_for_each_real_node;
    /*
        each position is a unordered_map to record the last access info of its
       virtual node
        the unordered_map is <content_id, the last access postition of this
       content>
    */
    std::vector<std::unordered_map<uint64_t, uint32_t>>
        last_access_on_each_virtual_node;
    std::vector<std::unordered_map<uint64_t, uint32_t>>
        last_access_on_each_real_node;
    /*
        each position is a unordered_map to record the counter stacks of its
       virtual node
        the two vector is the origin counter stack
    */
    std::pair<unsigned int, unsigned int> look_up_res;
    dequeue_node* head = nullptr;
    dequeue_node* tail = nullptr;
    dequeue_node* pointer = nullptr;

    std::vector<std::map<uint32_t, uint32_t>> frag_arrs; // store arr of each
                                                         // vnode in set, <start,
                                                         // size> // Peixuan:
                                                         // accumulate size for
                                                         // each vnode
    std::vector<std::map<uint32_t, uint32_t>>
        frag_arrs_rnode; // store arr of each rnode
    std::vector<int> cache_index_each_node;
    // std::vector<std::vector<int>> request_array; // cache_index in chash.sorted
    // and its contents row number vector
    int virtual_node_number = 0;
    consistent_hash chash;
    uint32_t position = 0;

    uint64_t max_requests;
    int max_i;
    int min_i;
    double max_rank;
    double min_rank;

    int SD_Max;
    std::list<uint32_t>::iterator target;

    bool flag;

    std::queue<dequeue_node*> queue_of_min;
    std::queue<dequeue_node*> queue_of_max;
    std::queue<dequeue_node*> queue_of_c_i;

    std::unordered_map<uint64_t, uint32_t>::iterator iter_in_last_access;

    CpuMonitor cpu_mon; // 0118202 Peixuan

public:
    ACHCache() : Cache() {}

    virtual void setPar(std::string parName, std::string parValue);
    void init_mapper();
    virtual bool lookup(SimpleRequest* req);
    virtual void admit(SimpleRequest* req);
    virtual void evict(SimpleRequest* req) {};
    virtual void evict() {};
    virtual void print_hash_space();
    virtual void update();
    void printReqAndFileNum(); 
    bool request(SimpleRequest* req);
    void reset();
};

static Factory<ACHCache> factorySFM("ACH");

#endif /*CLUSTER_VARIENTS_H*/
