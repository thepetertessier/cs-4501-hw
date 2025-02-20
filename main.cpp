#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>

#define MAX_N 100000
#define MAX_Q 50000

using namespace std;
using Matrix = array<array<double, 3>, 3>;

inline Matrix get_identity() {
    return {{
        {1,0,0},
        {0,1,0},
        {0,0,1}
    }};
}

inline Matrix multiply_matrices(Matrix &A, Matrix &B) {
    Matrix result = {};
    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++)
            for (int j = 0; j < 3; j++)
                result[i][j] += A[i][k] * B[k][j];
    return result;
}

inline unsigned int get_left_child_index(unsigned int index) {
    return 2*index + 1;
}

inline unsigned int get_right_child_index(unsigned int index) {
    return 2*index + 2;
}

struct SegmentTree {
    unsigned int n;
    vector<Matrix> t_tranformations;

    SegmentTree(vector<Matrix> a, unsigned int _n) {
        n = _n;
        t_tranformations = vector<Matrix>(4*MAX_N);
        build(a);
    }

    void build(vector<Matrix> a, int v, int tl, int tr) {
        if (tl == tr) {
            t_tranformations[v] = a[tl];
        } else {
            int tm = (tl + tr) / 2;
            int il = get_left_child_index(v);
            int ir = get_right_child_index(v);
            build(a, il, tl, tm);
            build(a, ir, tm+1, tr);
            t_tranformations[v] = multiply_matrices(t_tranformations[ir], t_tranformations[il]);
        }
    }

    inline void build(vector<Matrix> a) {
        build(a, 0, 0, n-1);
    }

    Matrix get_composed(int v, int tl, int tr, int l, int r) {
        if (l > r) {
            return get_identity();
        }
        if (l == tl && r == tr) {
            return t_tranformations[v];
        }
        int tm = (tl + tr) / 2;
        auto L = get_composed(get_left_child_index(v), tl, tm, l, min(r, tm));
        auto R = get_composed(get_right_child_index(v), tm+1, tr, max(tm+1, l), r);
        return multiply_matrices(R, L);
    }

    inline Matrix get_composed(int l, int r) {
        return get_composed(0, 0, n-1, l, r);
    }

    void update(int v, int tl, int tr, int pos, Matrix new_transformation) {
        if (tl == tr) {
            t_tranformations[v] = new_transformation;
        } else {
            int tm = (tl + tr) / 2;
            int il = get_left_child_index(v);
            int ir = get_right_child_index(v);
            if (pos <= tm) {
                update(il, tl, tm, pos, new_transformation);
            } else {
                update(ir, tm+1, tr, pos, new_transformation);
            }
            t_tranformations[v] = multiply_matrices(t_tranformations[ir], t_tranformations[il]);
        }
    }

    inline void update(int pos, Matrix matrix) {
        update(0, 0, n-1, pos, matrix);
    }
};

inline tuple<double, double, double> apply_matrix(Matrix A, int x, int y, int z) {
    double new_x = A[0][0] * x + A[0][1] * y + A[0][2] * z;
    double new_y = A[1][0] * x + A[1][1] * y + A[1][2] * z;
    double new_z = A[2][0] * x + A[2][1] * y + A[2][2] * z;
    return {new_x, new_y, new_z};
}

inline pair<double, double> apply_transformations(int x, int y, int l, int r, SegmentTree &st) {
    auto composed_transformation = st.get_composed(l, r);
    auto [new_x, new_y, new_z] = apply_matrix(composed_transformation, x, y, 1);
    return {new_x, new_y};
}

Matrix get_tranformation_matrix_from_cin(std::string &op) {
    if (op == "Translate") {
        double dx, dy;
        cin >> dx >> dy;
        return {{
            {  1,   0,  dx},
            {  0,   1,  dy},
            {  0,   0,   1}
        }};
    } else if (op == "Scale") {
        double sx, sy;
        cin >> sx >> sy;
        return {{
            { sx,   0,   0},
            {  0,  sy,   0},
            {  0,   0,   1}
        }};
    } else if (op == "Rotate") {
        double d;
        cin >> d;
        d *= (M_PI / 180); // convert to radians
        return {{
            { cos(d),-sin(d), 0},
            { sin(d), cos(d), 0},
            {      0,      0, 1}
        }};
    } else {
        throw invalid_argument("Unrecognized matrix operation.");
    }
}

int main() {
    unsigned int n, q;
    cin >> n >> q;

    vector<Matrix> a_transformations(MAX_N);

    for (unsigned int i = 0; i < n; i++) {
        string op;
        cin >> op;
        a_transformations[i] = get_tranformation_matrix_from_cin(op);
    }

    SegmentTree segment_tree(a_transformations, n);

    for (unsigned int i = 0; i < q; i++) {
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
            auto new_tranformation = get_tranformation_matrix_from_cin(op);
            segment_tree.update(pos, new_tranformation);
        }
    }
    return 0;
}
