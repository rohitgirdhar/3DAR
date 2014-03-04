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

#define E_FILE "E.txt"
#define R_FILE "R.txt"
#define OUT_FILE "output.txt"
#define ACT_N 285
#define N 10
#define K 3
#define INF 9999.0
#define MU 0.1f
#define LAMBDA 0.1f
#define COST_RECALIB 150.0f
#define COST_HOMO 100.0f
#define TOTAL_COST_LIMIT 850.0f
#define AVG_COST_LIMIT (TOTAL_COST_LIMIT / (N - K))

using namespace std;

float E[ACT_N][ACT_N], R[ACT_N][ACT_N];
const int Na = 4 * N + 2 * (N * N + 2 * N * N + 4 * N * N) + 2 * N;
//const int Na = 4 * N +  (N * N + 2 * N * N + 4 * N * N);
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

void solve() {
    glp_prob *lp;
    lp = glp_create_prob();
    string prob_name = "top_K_" + itos(N) + "_" + itos(K) + "_" + itos(TOTAL_COST_LIMIT);
    glp_set_prob_name(lp, prob_name.c_str());
    glp_set_obj_dir(lp, GLP_MIN);

    /**
     * Total variables (in order)
     *
     * e1, e2, ... , en (N)
     * a1 ... an, b1 ... bn, c1 ... cn (3 * N)
     * Z11 Z12 ... Znn, Z2_11 ... Z2_nn (2 * N * N)
     */
    glp_add_cols(lp, N + 3 * N + 2 * N * N);
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
    // for a,b,c
    for (char c = 'a'; c <= 'c'; c++) {
        for (int i = 1; i <= N; i++) {
            string vname = ctos(c) + string("_") + itos(i);
            glp_set_col_name(lp, col_num, vname.c_str());
// No costs in the objective            
//            if (c == 'b') {
//                glp_set_obj_coef(lp, col_num, LAMBDA * COST_RECALIB);
//            } else if (c == 'c') {
//               glp_set_obj_coef(lp, col_num, MU * COST_HOMO);
//            }
            glp_set_col_kind(lp, col_num, GLP_BV);
            col_num ++;
        }
    }
    // for Z1, Z2
    for (int z = 1; z <= 2; z++) {
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                string vname = string("Z") + itos(z) + string("_")
                    + itos(i) + string(":") + itos(j);
                glp_set_col_name(lp, col_num, vname.c_str());
                glp_set_obj_coef(lp, col_num, 0.0f);
                glp_set_col_kind(lp, col_num, GLP_BV);
                col_num ++;
            }
        }
    }

    /**
     * Total Constraints (in order)
     *
     * a_i + b_i + c_i = 1 (N)
     * sum_i {a_i} <= K (1)
     * sum_j { Z_{ij} } = 1  forall i (N)
     * Z_ij <= a_j forall i,j (N * N)
     * e_i >= E_ij * a_j + (-INF)... forall i (N^2)
     * sum_j { Z2_{ij} } = 1  forall i (N)
     * Z2_ij <= a_j forall i,j (N * N)
     * e_i ...                    (N^2)
     * AVG COST contraint (1)
     */

    int total_rows = 
        N + 
        1 + 
        2 * (N + N * N) + 
        2 * (N * N) + 
        1;  /* limit on cost */
    glp_add_rows(lp, total_rows);
    
    int row_num = 1;
    int idx = 1;  // VERY IMPORTANT!! IF SET 0, FIRST CONSTT. MISSED!!!
    // a_i + b_i ... = 1
    for (int i = 1; i <= N; i++) {
        string rname = string("sum_") + itos(i);
        glp_set_row_name(lp, row_num, rname.c_str());
        glp_set_row_bnds(lp, row_num, GLP_FX, 1.0f, 1.0f);

        ia[idx] = row_num, ja[idx] = N+i, ar[idx] = 1.0f;
        idx ++;
        ia[idx] = row_num, ja[idx] = 2*N+i, ar[idx] = 1.0f;
        idx ++;
        ia[idx] = row_num, ja[idx] = 3*N+i, ar[idx] = 1.0f;
        idx ++;
        row_num ++;
    }

    // sum (a_i) <= K
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
        glp_set_row_name(lp, row_num, "limit-Z");
        glp_set_row_bnds(lp, row_num, GLP_FX, 1, 1);
        for (int j = 1; j <= N; j++) {
            ia[idx] = row_num;
            ja[idx] = 4 * N + N * (i - 1) + j;
            ar[idx] = 1;
            idx ++;
        }
        row_num ++;
    }
    
    // Z_ij - a_j <= 0
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            glp_set_row_bnds(lp, row_num, GLP_UP, -INF, 0);
            ia[idx] = row_num;
            ja[idx] = 4 * N + N * (i - 1) + j;
            ar[idx] = 1;
            idx ++;
            ia[idx] = row_num;
            ja[idx] = N + j;
            ar[idx] = -1;
            idx ++;
            row_num ++;
        }
    }

    // E_ij * a_j + (-M)...
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            glp_set_row_bnds(lp, row_num, GLP_UP, 0, 3 * INF);
            ia[idx] = row_num; ja[idx] = i; ar[idx] = -1;
            idx ++;
            ia[idx] = row_num; ja[idx] = N + j; 
            ar[idx] = E[i-1][j-1] + INF;
            idx ++;
            ia[idx] = row_num; ja[idx] = 4 * N + N * (i - 1) + j;
            ar[idx] = INF;
            idx ++;
            ia[idx] = row_num; ja[idx] = 3 * N + i; 
            ar[idx] = INF;
            idx ++;

            row_num ++;
        }
    }
    
    // sum_j { Z2_{ij} } = 1  forall i (N)
    for (int i = 1; i <= N; i++) {
        glp_set_row_name(lp, row_num, "limit-Z2");
        glp_set_row_bnds(lp, row_num, GLP_FX, 1, 1);
        for (int j = 1; j <= N; j++) {
            ia[idx] = row_num;
            ja[idx] = 4 * N + N*N + N * (i - 1) + j;
            ar[idx] = 1;
            idx ++;
        }
        row_num ++;
    }
    
    // Z2_ij - a_j <= 0
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            glp_set_row_bnds(lp, row_num, GLP_UP, -INF, 0);
            ia[idx] = row_num;
            ja[idx] = 4 * N + N*N + N * (i - 1) + j;
            ar[idx] = 1;
            idx ++;
            ia[idx] = row_num;
            ja[idx] = N + j;
            ar[idx] = -1;
            idx ++;
            row_num ++;
        }
    }

    // E_ij * a_j + (-M)...
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            glp_set_row_bnds(lp, row_num, GLP_UP, 0, 3 * INF);
            ia[idx] = row_num; ja[idx] = i; ar[idx] = -1;
            idx ++;
            ia[idx] = row_num; ja[idx] = N + j; 
            ar[idx] = R[i-1][j-1] + INF;
            idx ++;
            ia[idx] = row_num; ja[idx] = 4 * N + N*N + N * (i - 1) + j;
            ar[idx] = INF;
            idx ++;
            ia[idx] = row_num; ja[idx] = 2 * N + i; 
            ar[idx] = INF;
            idx ++;

            row_num ++;
        }
    }
    
    // avg cost constraint 
    glp_set_row_bnds(lp, row_num, GLP_UP, -INF, 0);
    for (int i = 1; i <= N; i++) {
        ia[idx] = row_num; ja[idx] = 2*N + i; ar[idx] = COST_RECALIB - AVG_COST_LIMIT;
        idx++;
        ia[idx] = row_num; ja[idx] = 3*N + i; ar[idx] = COST_HOMO - AVG_COST_LIMIT;
        idx++;
    }
    row_num ++;


    glp_load_matrix(lp, Na-1, ia, ja, ar);
    glp_iocp param;
    glp_init_iocp(&param);
    param.presolve = GLP_ON;
    string path = "cplex_files/" + prob_name + ".cplex";
    glp_write_lp(lp, NULL, path.c_str());
    //int err = glp_intopt(lp, &param);
    


// Debugged why first constraint was not being read!!!
//    int ind[100] = {0};
//    double vals[100] = {0};
//    glp_get_mat_row(lp, 1, ind, vals); 
//    for (int i = 0; i < 100; i++) {
//        cout << ind[i] << "->" << vals[i] << endl;
//    }

//    cout << "printing the problem data" << endl;
//    glp_write_prob(lp, 0, "/dev/stdout");

    ofstream fout;
    fout.open(OUT_FILE);
    //print the objective 
    fout << "# obj value" << endl;
    fout << glp_mip_obj_val(lp) << endl;
    // print a b c s
    fout << "# abc" << endl;
    for (int i = 1; i <= N; i++) {
        fout << glp_mip_col_val(lp, N + i)
             << glp_mip_col_val(lp, 2 * N + i)
             << glp_mip_col_val(lp, 3 * N + i) << endl;
    }
    fout << "# e" << endl;
    for (int i = 1; i <= N; i++) {
        fout << glp_mip_col_val(lp, i) << " ";
    }
    fout << endl;
    fout << "# Z11 Z12 .. Z21 Z22 .." << endl;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            fout << glp_mip_col_val(lp, 4 * N + N * (i - 1) + j) << " ";
        }
        fout << endl;
    }
    fout << "# Z2_11 Z2_12 .. Z2_21 Z2_22 .." << endl;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            fout << glp_mip_col_val(lp, 4 * N + N*N + N * (i - 1) + j) << " ";
        }
        fout << endl;
    }

    fout.close();

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
        }
    }
    fin.close();
}

void readRFile(const char *fname) {
    ifstream fin;
    double s;
    fin.open(fname);
    if (!fin) {
        cerr << "Unable to read " << fname << endl;
    }
    for (int i = 0; i < ACT_N; i++) {
        for (int j = 0; j < ACT_N; j++) {
            fin >> R[i][j];
            R[i][j] = R[i][j];
        }
    }
    fin.close();
}

int main() {
    readEFile(E_FILE);
    cout << "Read E File" << endl;
    readRFile(R_FILE);
    cout << "Read R file" << endl;
    solve();
}
