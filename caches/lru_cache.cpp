#include "lru_cache.h"
#include "../random_helper.h"
#include <cassert>
#include <cassert>
#include <cmath>
#include <cmath>
#include <fstream>
#include <limits>
#include <unordered_map>

// golden section search helpers
#define SHFT2(a, b, c)                                                         \
  (a) = (b);                                                                   \
  (b) = (c);
#define SHFT3(a, b, c, d)                                                      \
  (a) = (b);                                                                   \
  (b) = (c);                                                                   \
  (c) = (d);

// math model below can be directly copiedx
// static inline double oP1(double T, double l, double p) {
static inline double oP1(double T, double l, double p) {
  return (l * p * T * (840.0 + 60.0 * l * T + 20.0 * l * l * T * T +
                       l * l * l * T * T * T));
}

static inline double oP2(double T, double l, double p) {
  return (840.0 + 120.0 * l * (-3.0 + 7.0 * p) * T +
          60.0 * l * l * (1.0 + p) * T * T +
          4.0 * l * l * l * (-1.0 + 5.0 * p) * T * T * T +
          l * l * l * l * p * T * T * T * T);
}

/*
  LRU: Least Recently Used eviction
*/
bool LRUCache::lookup(SimpleRequest *req) {
  // update request number and file number  -- ymj
  _requestNum += 1;
  _uniqueFile.insert(req->getId());
  // CacheObject: defined in cache_object.h
  CacheObject obj(req);
  // _cacheMap defined in class LRUCache in lru_variants.h
  auto it = _cacheMap.find(obj);
  if (it != _cacheMap.end()) {
    // log hit
    LOG("h", 0, obj.id, obj.size);
    hit(it, obj.size);
    return true;
  }
  return false;
}

void LRUCache::admit(SimpleRequest *req) {
  const uint64_t size = req->getSize();
  // object feasible to store?
  if (size > _cacheSize) {
    LOG("L", _cacheSize, req->getId(), size);
    return;
  }
  // check eviction needed
  while (_currentSize + size > _cacheSize) {
    evict();
  }
  // admit new object
  CacheObject obj(req);
  _cacheList.push_front(obj);
  _cacheMap[obj] = _cacheList.begin();
  _currentSize += size;
  LOG("a", _currentSize, obj.id, obj.size);
}

void LRUCache::evict(SimpleRequest *req) {
  CacheObject obj(req);
  auto it = _cacheMap.find(obj);
  if (it != _cacheMap.end()) {
    ListIteratorType lit = it->second;
    LOG("e", _currentSize, obj.id, obj.size);
    _currentSize -= obj.size;
    _cacheMap.erase(obj);
    _cacheList.erase(lit);
  }
}

SimpleRequest *LRUCache::evict_return() {
  // evict least popular (i.e. last element)
  if (_cacheList.size() > 0) {
    ListIteratorType lit = _cacheList.end();
    lit--;
    CacheObject obj = *lit;
    LOG("e", _currentSize, obj.id, obj.size);
    SimpleRequest *req = new SimpleRequest(obj.id, obj.size);
    _currentSize -= obj.size;
    _cacheMap.erase(obj);
    _cacheList.erase(lit);
    return req;
  }
  return NULL;
}

void LRUCache::evict() { evict_return(); }

// const_iterator: a forward iterator to const value_type, where
// value_type is pair<const key_type, mapped_type>
void LRUCache::hit(lruCacheMapType::const_iterator it, uint64_t size) {
  // transfers it->second (i.e., ObjInfo) from _cacheList into
  // 	*this. The transferred it->second is to be inserted before
  // 	the element pointed to by _cacheList.begin()
  //
  // _cacheList is defined in class LRUCache in lru_variants.h
  _cacheList.splice(_cacheList.begin(), _cacheList, it->second);
}

int LRUCache::requestNum() // return request number of a single cache -- ymj
{
  return _requestNum;
}
int LRUCache::uniqueFileNum() // return unique file number processed by this
                              // cache -- ymj
{
  return _uniqueFile.size();
}