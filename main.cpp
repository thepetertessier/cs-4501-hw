#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "main.h"

#define MAX_N 100000
#define MAX_Q 50000

using namespace std;

const double IDENTITY[3][3] = {
    {1,0,0},
    {0,1,0},
    {0,0,1}
};
double transformations[MAX_N][3][3];

void multiply_matrices(double A[3][3], double B[3][3], double result[3][3]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++i) {
            result[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

unsigned int get_left_child_index(unsigned int index) {
    return 2*(index+1) - 1;
}

unsigned int get_right_child_index(unsigned int index) {
    return 2*(index+1);
}

struct SegmentTree {
    unsigned int n;
    double t[4*MAX_N][3][3];

    SegmentTree(int _n) {
        n = _n;
        for (int i = 0; i < 4 * MAX_N; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    t[i][j][k] = 0.0;
                }
            }
        }
    }

    void build(double a[MAX_N][3][3], int v, int tl, int tr) {
        if (tl == tr) {
            set_matrix(t[v], a[tl]);
        } else {
            int tm = (tl + tr) / 2;
            int il = get_left_child_index(v);
            int ir = get_right_child_index(v);
            build(a, il, tl, tm);
            build(a, ir, tm+1, tr);
            double matrix[3][3];
            multiply_matrices(t[il], t[ir], matrix);
            set_matrix(t[v], matrix);
        }
    }

    void build(double a[MAX_N][3][3]) {
        build(a, 0, 0, n-1);
    }

    void get_composed(int v, int tl, int tr, int l, int r, double result[3][3]) {
        if (l > r) {
            set_matrix(result, IDENTITY);
        }
        if (l == tl && r == tr) {
            set_matrix(result, t[v]);
        }
        int tm = (tl + tr) / 2;
        double L[3][3], R[3][3];
        get_composed(get_left_child_index(v), tl, tm, l, min(r, tm), L);
        get_composed(get_right_child_index(v), tm+1, tr, max(tm+1,tl), r, R);
        multiply_matrices(L, R, result);
    }

    void get_composed(int l, int r, double result[3][3]) {
        get_composed(0, 0, n-1, l, r, result);
    }

    void update(int v, int tl, int tr, int pos, double new_matrix[3][3]) {
        if (tl == tr) {
            set_matrix(t[v], new_matrix);
        } else {
            int tm = (tl + tr) / 2;
            int il = get_left_child_index(v);
            int ir = get_right_child_index(v);
            if (pos <= tm)
                update(il, tl, tm, pos, new_matrix);
            else
                update(ir, tm+1, tr, pos, new_matrix);
            double matrix[3][3];
            multiply_matrices(t[il], t[ir], matrix);
            set_matrix(t[v], matrix);
        }
    }

    void update(int pos, double new_matrix[3][3]) {
        update(0, 0, n-1, pos, new_matrix);
    }
};

tuple<double, double, double> apply_matrix(double A[3][3], int x, int y, int z) {
    double new_x = A[0][0] * x + A[0][1] * y + A[0][2] * z;
    double new_y = A[1][0] * x + A[1][1] * y + A[1][2] * z;
    double new_z = A[2][0] * x + A[2][1] * y + A[2][2] * z;
    return {new_x, new_y, new_z};
}

pair<double, double> apply_transformations(int x, int y, int l, int r, SegmentTree st) {
    double composed_transformation[3][3];
    st.get_composed(l, r, composed_transformation);
    auto [new_x, new_y, new_z] = apply_matrix(composed_transformation, x, y, 1);
    return {new_x, new_y};
}

int main() {
    int n, q;
    cin >> n >> q;
    
    for (int i = 0; i < n; i++) {
        string op;
        cin >> op;
        set_transformation(op, i);
    }

    SegmentTree segment_tree(n);
    segment_tree.build(transformations);
    
    for (int i = 0; i < q; i++) {
        char type;
        cin >> type;
        if (type == 'Q') {
            int x, y, l, r;
            cin >> x >> y >> l >> r;
            auto [new_x, new_y] = apply_transformations(x, y, l, r, segment_tree);
            cout << fixed << setprecision(5);
            cout << "(" << x << "," << y << "): " << new_x << " " << new_y << "\n";
        } else if (type == 'U') {
            int pos;
            string op;
            cin >> pos >> op;
            set_transformation(op, pos);
            segment_tree.update(pos, transformations[pos]);
        }
    }
    return 0;
}

void set_matrix(double matrix[3][3], const double values[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = values[i][j];
        }
    }
}

void set_transformation(std::string &op, int idx)
{
    if (op == "Translate")
    {
        double dx, dy;
        cin >> dx >> dy;
        const double new_values[3][3] = {
            {  1,   0,  dx},
            {  0,   1,  dy},
            {  0,   0,   1}
        };
        set_matrix(transformations[idx], new_values);
    }
    else if (op == "Scale")
    {
        double sx, sy;
        cin >> sx >> sy;
        const double new_values[3][3] = {
            { sx,   0,   0},
            {  0,  sy,   0},
            {  0,   0,   1}
        };
        set_matrix(transformations[idx], new_values);
    }
    else if (op == "Rotate")
    {
        double d;
        cin >> d;
        const double new_values[3][3] = {
            { cos(d),-sin(d), 0},
            { sin(d), cos(d), 0},
            {      0,      0, 1}
        };
        set_matrix(transformations[idx], new_values);
    }
}
