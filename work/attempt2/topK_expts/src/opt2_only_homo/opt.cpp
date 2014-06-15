/**
 * opt.h
 * Without costs optimization, cost limited in constraints 
 */

#include <glpk.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

#define E_FILE "../../homos/par/E_bob.txt"
#define ACT_N 96
#define N 10
#define K 60

double MAX = 9999.0;
double INF = 10 * MAX;

using namespace std;

float E[ACT_N][ACT_N];
const int Na = N + 6 * (N * N);
int ia[Na], ja[Na];
double ar[Na];

string itos(int a) {
    stringstream ss;
    ss << a;
    return ss.str();
}

string ctos(char a) {
    stringstream ss;
    ss << a;
    return ss.str();
}

void writeMIP() {
    glp_prob *lp;
    lp = glp_create_prob();
    string prob_name = "top_K_" + itos(N) + "_" + itos(K);
    glp_set_prob_name(lp, prob_name.c_str());
    glp_set_obj_dir(lp, GLP_MIN);

    /**
     * Total variables (in order)
     *
     * e1, e2, ... , en (N)
     * x1 ... xn (N)
     * Z11 Z12 ... Znn (N * N)
     */
    glp_add_cols(lp, N + N + N * N);
    int col_num = 1;
    // for e_i
    for (int i = 1; i <= N; i++) {
        string vname = string("e_") + itos(i);
        glp_set_col_name(lp, col_num, vname.c_str());
        glp_set_col_bnds(lp, col_num, GLP_LO, 0.0f, INF);
        glp_set_obj_coef(lp, col_num, 1.0f);
        glp_set_col_kind(lp, col_num, GLP_CV);
        col_num ++;
    }
    // for x
    for (int i = 1; i <= N; i++) {
        string vname = 'x' + string("_") + itos(i);
        glp_set_col_name(lp, col_num, vname.c_str());
        glp_set_col_kind(lp, col_num, GLP_BV);
        col_num ++;
    }
    // for Z1
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            string vname = string("Z") + string("_")
                + itos(i) + string("_") + itos(j);
            glp_set_col_name(lp, col_num, vname.c_str());
            glp_set_obj_coef(lp, col_num, 0.0f);
            glp_set_col_kind(lp, col_num, GLP_BV);
            col_num ++;
        }
    }

    /**
     * Total Constraints (in order)
     *
     * sum_i {x_i} <= K (1)
     * sum_j { Z_{ij} } = 1  forall i (N)
     * Z_ij <= x_j forall i,j (N * N)
     * e_i >= E_ij * a_j + (-INF)... forall i (N^2)
     */

    int total_rows = 
        1 + 
        (N + N * N) + 
        (N * N); 
    glp_add_rows(lp, total_rows);
    
    int row_num = 1;
    int idx = 1;  // VERY IMPORTANT!! IF SET 0, FIRST CONSTT. MISSED!!!

    // sum (x_i) <= K
    glp_set_row_name(lp, row_num, "limit-k");
    glp_set_row_bnds(lp, row_num, GLP_DB, 0, K);
    for (int i = 1; i <= N; i++) {
        ia[idx] = row_num;
        ja[idx] = N + i;
        ar[idx] = 1;
        idx ++;
    }
    row_num ++;

    // sum_j { Z_{ij} } = 1  forall i (N)
    for (int i = 1; i <= N; i++) {
        glp_set_row_name(lp, row_num, (string("limit-Z") + itos(i)).c_str());
        glp_set_row_bnds(lp, row_num, GLP_FX, 1, 1);
        for (int j = 1; j <= N; j++) {
            ia[idx] = row_num;
            ja[idx] = 2 * N + N * (i - 1) + j;
            ar[idx] = 1;
            idx ++;
        }
        row_num ++;
    }
    
    // Z_ij - x_j <= 0
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            glp_set_row_bnds(lp, row_num, GLP_UP, -INF, 0);
            ia[idx] = row_num;
            ja[idx] = 2 * N + N * (i - 1) + j;
            ar[idx] = 1;
            idx ++;
            ia[idx] = row_num;
            ja[idx] = N + j;
            ar[idx] = -1;
            idx ++;
            row_num ++;
        }
    }

    // E_ij * x_j + (-M)...
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            glp_set_row_bnds(lp, row_num, GLP_UP, 0, 2 * INF);
            ia[idx] = row_num; ja[idx] = i; ar[idx] = -1;
            idx ++;
            ia[idx] = row_num; ja[idx] = N + j; 
            ar[idx] = E[i-1][j-1] + INF;
            idx ++;
            ia[idx] = row_num; ja[idx] = 2 * N + N * (i - 1) + j;
            ar[idx] = INF;
            idx ++;

            row_num ++;
        }
    }
    
    glp_load_matrix(lp, Na-1, ia, ja, ar);
    glp_iocp param;
    glp_init_iocp(&param);
    param.presolve = GLP_ON;
    string path_cplex = "cplex_files/" + prob_name + ".lp";
    glp_write_lp(lp, NULL, path_cplex.c_str());
    
    glp_delete_prob(lp);
}

void readEFile(const char *fname) {
    ifstream fin;
    string s;
    fin.open(fname);
    for (int i = 0; i < ACT_N; i++) {
        for (int j = 0; j < ACT_N; j++) {
            fin >> s;
            if (s.compare("inf") != 0) {
                E[i][j] = atof(s.c_str());
            } else {
                E[i][j] = INF;
            }
            E[i][j] = E[i][j];
            if (E[i][j] > MAX) {
                E[i][j] = MAX;
            }
        }
    }
    fin.close();
}

int main() {
    readEFile(E_FILE);
    cout << "Read E File" << endl;
    INF = 10 * MAX;
    writeMIP();
}
