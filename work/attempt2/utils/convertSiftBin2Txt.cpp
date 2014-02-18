#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

using namespace std;

struct header {
    int data[5];
};

struct location {
    float data[5];
};

struct sift {
    unsigned char data[128];
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input SIFT binary file> <output txt file>" << endl;
        exit(-1);
    }
    string fin = argv[1];
    string fout = argv[2];
    FILE *f;
    f = fopen(fin.c_str(), "rb");
    header *buf = (header*) malloc (sizeof(header));
    fread(buf, sizeof(header), 1, f);
    int npoints = buf->data[2];
    free(buf);

    location* locs = new location[npoints];
    fread(locs, sizeof(location), npoints, f);

    sift* sifts = new sift[npoints];
    fread(sifts, sizeof(sift), npoints, f);
    
    fclose(f);

    // write to file 
    ofstream f2;
    f2.open(fout.c_str(), ios::out);
    for (int i = 0; i < npoints; i++) {
        f2 << locs[i].data[0] << " " << locs[i].data[1] << " "
           << locs[i].data[2] << " " << locs[i].data[3];
        for (int j = 0; j < 128; j++) {
            if (j % 20 == 0) f2 << endl << " ";
            f2 << (int) sifts[i].data[j] << " ";
        }
        f2 << endl;
    }
    f2.close();

    delete locs; delete sifts;

    return 0;
}
