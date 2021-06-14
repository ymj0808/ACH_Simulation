# cache simulatior for paper ACH

## Compiling 

GCC 4.8.1 upwards (with -std=c++11). Ubuntu 18.04 is recommended.

The Makefile is offered

    make

## Using an exisiting policy

The basic interface is

    ./cache_sim traceFile cacheType cacheSize [cacheParams]

where

 - traceFile: a request trace (see below)
 - cacheType: one of the caching policies (see below)
 - cacheSize: the cache capacity in bytes
 - cacheParams: optional cache parameters, can be used to tune cache policies (see below)

### Request trace format

Request traces must be given in a space-separated format with **TWO** colums

- id should be a long long int, used to uniquely identify objects
- size should be a long long int, this is object's size in bytes

|  id | size |
| --- | ---- |
|  1  |  120 |
|  2  |   64 |
|  1  |  120 |
|  3  |  14  |
|  1 |  120 |

Example trace in file "test.tr".

### Content Size and Cache Size

The last column in trace file is the content size, now can run experiment in 2 ways : use real size in trace file and set the cache size as the real memory size in bytes, or regard all size as 1 and set the cache size as the maximun number of contents to cache. Set the param as `filesize=true` to use real content size, default as not use content size.

### Available single caching policies

There are currently ten caching policies. This section describes each one, in turn, its parameters, and how to run it on the "test.tr" example trace with cache size 1000 Bytes.

#### Example usage

    ./cache_sim trace_file_path cache_policy [paramName=paramValue]

#### LRU

does: least-recently used eviction

params: none

example usage:

    ./cache_sim test.tr LRU 1000

or use real content size:

    ./cache_sim test.tr LRU 1000 filesize=true     

## Distributed cache policy

#### Consistent hash
**params**: `n` - the number of caches in cluster, `vnode` - the number of virtual nodes for each server 

    ./cache_sim test.tr CH 1000 n=4 vnode=40

#### Adaptive Consistent hash
**params**: `n` - the number of caches in cluster

    ./cache_sim test.tr ACH 1000 n=4 vnode=40 alpha=5 W=10000 t=1000
