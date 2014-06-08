#include <iostream>
#include "NVMUtils.hpp"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help", "Show this help message")
        ("nvm-file,n", po::value<string>()->required(), "input NVM fpath")
        ;
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error &e) {
        cerr << e.what() << endl;
        return -1;
    }
    
    map<int, map<int, pair<float, float>>> img2pts;
    readKptsFromNVM(vm["nvm-file"].as<string>(), img2pts);
    return 0;
}
