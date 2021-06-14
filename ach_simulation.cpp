#include "caches/cluster_variants.h"
#include "caches/gd_variants.h"
#include "caches/lru_variants.h"
#include "request.h"
#include <chrono>
#include <fstream>
#include <regex>
#include <string>

using namespace std;
ofstream outTp;

int main(int argc, char* argv[]) {

    // output help if insufficient params
    auto wall_clock = chrono::steady_clock::now();
    if (argc < 4) {
        cerr << "webcachesim traceFile cacheType cacheSizeBytes [cacheParams]"
            << endl;
        return 1;
    }
    cerr << "beginning..." << endl;

    // trace properties
    const char* path = argv[1];

    // create cache
    const string cacheType = argv[2];
    unique_ptr<Cache> webcache = Cache::create_unique(cacheType);
    if (webcache == nullptr)
        return 1;
    // configure cache size
    const uint64_t cache_size = std::stoull(argv[3]);
    webcache->setSize(cache_size);

    bool file_size = false;
    // parse cache parameters(cache number)
    regex opexp("(.*)=(.*)");
    cmatch opmatch;
    string paramSummary;
    for (int i = 4; i < argc; i++) {
        regex_match(argv[i], opmatch, opexp);
        if (opmatch.size() != 3) {
            std::cerr << "each cacheParam needs to be in form name=value" << endl;
            return 1;
        }
        if (opmatch[1].compare("filesize") == 0) {
            if (opmatch[2].compare("true") == 0)
                file_size = true;
        }
        else {
            webcache->setPar(opmatch[1], opmatch[2]);
        }
        paramSummary += opmatch[2];
        paramSummary += "\t";
    }

    ifstream infile;
    long long reqs = 0, hits = 0;
    long long reqs_size = 0, hits_size = 0;
    long long id, size;
    SimpleRequest* req = new SimpleRequest(0, 0);

    cerr << "running..." << endl;

    infile.open(path);

    if (cacheType.compare("CH") && cacheType.compare("ACH")) {
        cout << cacheType << endl;
        if (file_size) {  // real size, careful for the param cache size and threshold
            while (infile >> id >> size) {
                reqs++;
                reqs_size += size;
                req->reinit(id, size);
                if (webcache->lookup(req)) {
                    hits++;
                    hits_size += size;
                }
                else {
                    webcache->admit(req);
                }
            }
        }
        else {
            while (infile >> id >> size) {
                size = 1;     // size control by Tianxing
                reqs++;
                reqs_size += size;
                req->reinit(id, size);
                if (webcache->lookup(req)) {
                    hits++;
                    hits_size += size;
                }
                else {
                    webcache->admit(req);
                }
            }
        }
    }
    else { // cluster variants
        cout << cacheType << endl;
        webcache->init_mapper();
        if (file_size) {
            while (infile >> id >> size) {
                reqs++;
                reqs_size += size;
                req->reinit(id, size);
                if (webcache->request(req)) {
                    hits++;
                    hits_size += size;
                }
            }
        }
        else {
            while (infile >> id >> size) {
                size = 1;     // size control by Tianxing
                reqs++;
                reqs_size += size;
                req->reinit(id, size);
                if (webcache->request(req)) {
                    hits++;
                    hits_size += size;
                }                
            }
        }
    }
    delete req;


    infile.close();
    outTp.open("results.txt", ofstream::app);
    const long timeElapsed = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - wall_clock)
        .count();
    cout << "time duration : " << timeElapsed << "ms" << endl;
    cout << "trace : " << path << endl;
    cout << "cacheType : " << cacheType << "\tcache_size : " << cache_size
        << "\tparamSummary : " << paramSummary << endl;
    cout << "reqs : " << reqs << "\thits : " << hits
        << "\thit rate : " << double(hits) / reqs << endl;
    cout << "reqs size : " << reqs_size << "\thit size : " << hits_size
        << "\thit size rate : " << double(hits_size) / reqs_size << endl;
    

    outTp << endl;
    outTp << "time duration : " << timeElapsed << "ms" << endl;
    outTp << "trace : " << path << endl;
    outTp << "cacheType : " << cacheType << "\tcache_size : " << cache_size
        << "\tparamSummary : " << paramSummary << endl;
    outTp << "reqs : " << reqs << "\thits : " << hits
        << "\thit rate : " << double(hits) / reqs << endl;
    outTp << "reqs size : " << reqs_size << "\thit size : " << hits_size
        << "\thit size rate : " << double(hits_size) / reqs_size << endl;

    outTp.close();
    return 0;
}
