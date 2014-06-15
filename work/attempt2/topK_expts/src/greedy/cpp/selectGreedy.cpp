/**
 * Complete functionality NOT implemented yet
 * use selectGreedy.py
 * only for stone chariot experiement
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/time.h>
#include <ctime>
#include <numeric>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
typedef unsigned long long uint64;

#define MAX_N 2000
#define INF 9999999999
double E[MAX_N][MAX_N];
uint64 GetTimeMs64();

using namespace std;

/**
 * @return N number of elements in complete set
 */
int readE(string errfile, double E[][MAX_N], int useN = MAX_N) {
    if (useN != MAX_N) {
        cerr << "Using useN = " << useN << endl;
    }
    ifstream fin(errfile.c_str());
    string line;
    int lineno = 0;
    while (getline(fin, line)) {
        if (lineno >= useN) break;
        istringstream iss(line);
        int col = 0;
        string val;
        while(iss >> val) {
            if (col >= useN) break;
            if (val.compare("inf") == 0) {
                E[lineno][col] = INF;
            } else {
                E[lineno][col] = stod(val);
            }
            col++;
        }
        lineno++;
    }
    fin.close();
    return lineno;
}

vector<int> readTest(string testfile) {
    ifstream fin(testfile.c_str());
    string line;
    vector<int> res;
    while (getline(fin, line)) {
        res.push_back(stoi(line));
    }
    fin.close();
    return res;
}

vector<bool> selectGreedy(double E[][MAX_N], 
        vector<int> train, 
        vector<int> test,
        int K, 
        int N) { 
    vector<double> errs(train.size(), INF);
    vector<bool> selected(N, false);
    while (K--) {
        double max_inc = 0;
        int max_i = 0;
        for (int i = 0; i < train.size(); i++) {
            if (selected[train[i]]) continue;
            double inc = 0;
            for (int j = 0; j < train.size(); j++) {
                double approx_j_by_i = E[train[j]][train[i]];
                if (errs[j] > approx_j_by_i) {
                    inc += errs[j] - approx_j_by_i;
                }
            }
            if (inc > max_inc) {
                max_inc = inc;
                max_i = i;
            }
        }
        selected[train[max_i]] = true;
        for (int i = 0; i < train.size(); i++) {
            if (errs[i] > E[train[i]][train[max_i]]) {
                errs[i] = E[train[i]][train[max_i]];
            }
        }
    }
    cerr << "Train error : " << accumulate(errs.begin(), errs.end(), 0.0f) << endl;
    return selected;
}

double computeTestErr(double E[][MAX_N], 
        vector<int> train,
        vector<int> tests,
        vector<bool> selected) {
    double res = 0;
    for (int i : tests) {
        double minerr = INF;
        for (int j : train) {
            if (selected[j]) minerr = min(minerr, E[i][j]);
        }
        res += minerr;
    }
    return res / tests.size();
}

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "Show this help message")
        ("err-file,e", po::value<string>()->required(), "Input Error mat file")
        ("tests-file,t", po::value<string>()->required(), "file with test files list")
        ("num-select,K", po::value<int>()->required(), "Number of elements to select")
        ("use-N,N", po::value<int>(), "Use only first NxN size submat from the mat file")
        ;
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error &e) {
        cerr << e.what() << endl;
        return -1;
    } 
    int N;
    if (vm.count("use-N") > 0) {
        int useN = vm["use-N"].as<int>();
        N = readE(vm["err-file"].as<string>(), E, useN);
    } else {
        N = readE(vm["err-file"].as<string>(), E);
    }
    
    vector<int> tests = readTest(vm["tests-file"].as<string>());
    sort(tests.begin(), tests.end());
    vector<int> train;
    for (int i = 0; i < N; i++) {
        if (! binary_search(tests.begin(), 
                tests.end(), 
                i)) {
            train.push_back(i);
        }
    }
    int K = vm["num-select"].as<int>();
    
    uint64 start = GetTimeMs64(); 
    vector<bool> selected = selectGreedy(E, train, tests, K, N);
    cerr << "time elapsed in greedy select: " << (GetTimeMs64() - start) / 1000.0 << "sec" << endl;

    cout << computeTestErr(E, train, tests, selected) << endl;
    return 0;
}

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
 * windows and linux. */

uint64 GetTimeMs64()
{
#ifdef WIN32
    /* Windows */
    FILETIME ft;
    LARGE_INTEGER li;

    /* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
     * to a LARGE_INTEGER structure. */
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    uint64 ret = li.QuadPart;
    ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
    ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

    return ret;
#else
    /* Linux */
    struct timeval tv;

    gettimeofday(&tv, NULL);

    uint64 ret = tv.tv_usec;
    /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
    ret /= 1000;

    /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
    ret += (tv.tv_sec * 1000);

    return ret;
#endif
}
