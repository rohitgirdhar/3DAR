#include <glpk.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#define E_FILE "E.txt"
#define N 285
#define K 10
#define INF 9999999999
#define MU 1.0f
#define LAMBDA 1.0f
#define COST_RECALIB 100.0f
#define COST_HOMO 100.0f

using namespace std;

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

void solve(float E[][N]) {
    glp_prob *lp;
    lp = glp_create_prob();
    glp_set_prob_name(lp, "top-K");
    glp_set_obj_dir(lp, GLP_MIN);

    /**
     * Total variables (in order)
     *
     * e1, e2, ... , en (N)
     * a1 ... an, b1 ... bn, c1 ... cn (3 * N)
     * Z11 ... Znn, Z2_11 ... Z2_nn (2 * N * N)
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
     		if (c == 'b') {
     			glp_set_obj_coef(lp, col_num, LAMBDA * COST_RECALIB);
     		} else if (c == 'c') {
     			glp_set_obj_coef(lp, col_num, MU * COST_HOMO);
     		}
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
     * sum_j { Z2_{ij} } = 1  forall i (N)
     * Z2_ij <= a_j forall i,j (N * N)
     * e_i >= E_ij * a_j + (-INF)... forall i (N^2)
     * "                    (N^2)
     */

     int total_rows = N + 1 + 2 * (N + N * N) + 2 * (N * N);
     glp_add_rows(lp, total_rows);
     int ia[3*N], ja[3*N];
     double ar[3*N];
     int row_num = 1;
     int idx = 0;
     // a_i + b_i ... = 1
     for (int i = 1; i <= N; i++) {
     	string rname = string("sum_") + itos(i);
     	glp_set_row_name(lp, row_num, rname.c_str());
     	glp_set_row_bnds(lp, row_num, GLP_FX, 1.0f, 1.0f);

     	ia[idx] = row_num, ja[idx] = N+1, ar[idx] = 1;
     	idx ++;
     	ia[idx] = row_num, ja[idx] = 2*N+1, ar[idx] = 1;
     	idx ++;
     	ia[idx] = row_num, ja[idx] = 3*N+1, ar[idx] = 1;
     	idx ++;
     }


     glp_iocp param;
     glp_init_iocp(&param);
     param.presolve = GLP_ON;
     int err = glp_intopt(lp, &param);

     glp_delete_prob(lp);
}

void readEFile(const char *fname, float E[][N]) {
    ifstream fin;
    fin.open(fname);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {    
            fin >> E[i][j];
        }
    }
    fin.close();
}

int main() {
    float E[N][N];
    readEFile(E_FILE, E);
    cout << "Read E File" << endl;
    solve(E);
}