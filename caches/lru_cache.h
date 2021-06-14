#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "cache.h"
#include "cache_object.h"
#include <list>
#include <random>
#include <set>
#include <unordered_map>

// uncomment to enable cache debugging:
// #define CDEBUG 1

// util for debug
#ifdef CDEBUG
inline void logMessage(std::string m, double x, double y, double z) {
	std::cerr << m << "," << x << "," << y << "," << z << "\n";
}
#define LOG(m, x, y, z) logMessage(m, x, y, z)
#else
#define LOG(m, x, y, z)
#endif

typedef std::list<CacheObject>::iterator ListIteratorType;
typedef std::unordered_map<CacheObject, ListIteratorType> lruCacheMapType;

/*
  LRU: Least Recently Used eviction
*/
class LRUCache : public Cache {
protected:
  // list for recency order
  // std::list is a container, usually, implemented as a doubly-linked list
  std::list<CacheObject> _cacheList;
  // map to find objects in list
  lruCacheMapType _cacheMap;
  int _requestNum; // request counter for a single cache -- ymj
  std::set<long long> _uniqueFile;

  virtual void hit(lruCacheMapType::const_iterator it, uint64_t size);

public:
  LRUCache() : Cache() { _requestNum = 0; }
  virtual ~LRUCache() {}

  virtual bool lookup(SimpleRequest *req);
  virtual void admit(SimpleRequest *req);
  virtual void evict(SimpleRequest *req);
  virtual void evict();
  virtual SimpleRequest *evict_return();
  virtual int
  requestNum(); // return request number and unique file number   -- ymj
  virtual int uniqueFileNum();
};

static Factory<LRUCache> factoryLRU("LRU");

#endif /*LRU_CACHE_H*/