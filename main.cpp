#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

using namespace std;

// Represent an affine transformation by six doubles.
// This corresponds to the matrix:
//    [ a   b   c ]
//    [ d   e   f ]
//    [ 0   0   1 ]
struct Transformation {
    double a, b, c, d, e, f;
};

inline Transformation get_identity() {
    return {1, 0, 0, 0, 1, 0};
}

// Multiply two transformations T1 and T2.
// Note: This gives the composition T1 * T2, meaning that if you
// first apply T2 then T1, you get the result.
inline Transformation multiply(const Transformation &T1, const Transformation &T2) {
    return {
        T1.a * T2.a + T1.b * T2.d,               // a
        T1.a * T2.b + T1.b * T2.e,               // b
        T1.a * T2.c + T1.b * T2.f + T1.c,        // c
        T1.d * T2.a + T1.e * T2.d,               // d
        T1.d * T2.b + T1.e * T2.e,               // e
        T1.d * T2.c + T1.e * T2.f + T1.f         // f
    };
}

// Segment Tree that stores Transformation objects.
struct SegmentTree {
    int n;
    vector<Transformation> tree;
    
    SegmentTree(const vector<Transformation>& arr) {
        n = arr.size();
        tree.resize(4 * n);
        build(arr, 0, 0, n - 1);
    }
    
    void build(const vector<Transformation>& arr, int v, int tl, int tr) {
        if(tl == tr) {
            tree[v] = arr[tl];
        } else {
            int tm = (tl + tr) / 2;
            int left = 2*v + 1, right = 2*v + 2;
            build(arr, left, tl, tm);
            build(arr, right, tm+1, tr);
            // Note: the order is important!
            tree[v] = multiply(tree[right], tree[left]);
        }
    }
    
    Transformation query(int v, int tl, int tr, int l, int r) {
        if(l > r)
            return get_identity();
        if(l == tl && r == tr)
            return tree[v];
        int tm = (tl + tr) / 2;
        Transformation leftRes = query(2*v+1, tl, tm, l, min(r, tm));
        Transformation rightRes = query(2*v+2, tm+1, tr, max(l, tm+1), r);
        return multiply(rightRes, leftRes);
    }
    
    inline Transformation query(int l, int r) {
        return query(0, 0, n-1, l, r);
    }
    
    void update(int v, int tl, int tr, int pos, const Transformation &newVal) {
        if(tl == tr) {
            tree[v] = newVal;
        } else {
            int tm = (tl + tr) / 2;
            if(pos <= tm)
                update(2*v+1, tl, tm, pos, newVal);
            else
                update(2*v+2, tm+1, tr, pos, newVal);
            tree[v] = multiply(tree[2*v+2], tree[2*v+1]);
        }
    }
    
    inline void update(int pos, const Transformation &newVal) {
        update(0, 0, n-1, pos, newVal);
    }
};

// Apply a transformation to the point (x, y).
inline pair<double, double> apply(const Transformation &T, int x, int y) {
    double new_x = T.a * x + T.b * y + T.c;
    double new_y = T.d * x + T.e * y + T.f;
    return {new_x, new_y};
}

// Read a transformation from input.
Transformation read_transformation() {
    string op;
    cin >> op;
    if(op == "Translate") {
        double dx, dy;
        cin >> dx >> dy;
        return {1, 0, dx, 0, 1, dy};
    } else if(op == "Scale") {
        double sx, sy;
        cin >> sx >> sy;
        return {sx, 0, 0, 0, sy, 0};
    } else if(op == "Rotate") {
        double d;
        cin >> d;
        double rad = d * (M_PI / 180.0);
        double cosd = cos(rad), sind = sin(rad);
        return {cosd, -sind, 0, sind, cosd, 0};
    } else {
        throw invalid_argument("Unrecognized matrix operation.");
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, q;
    cin >> n >> q;
    vector<Transformation> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = read_transformation();
    }
    
    SegmentTree seg(arr);
    
    cout << fixed << setprecision(5);
    for (int i = 0; i < q; i++) {
        char type;
        cin >> type;
        if(type == 'Q') {
            int x, y, l, r;
            cin >> x >> y >> l >> r;
            Transformation comp = seg.query(l, r);
            auto res = apply(comp, x, y);
            cout << "(" << x << "," << y << "): " << res.first << " " << res.second << "\n";
        } else if(type == 'U') {
            int pos;
            cin >> pos;
            Transformation newTrans = read_transformation();
            seg.update(pos, newTrans);
        }
    }
    return 0;
}
