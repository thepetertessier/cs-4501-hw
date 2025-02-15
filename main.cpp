#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "main.h"

#define MAX_N 100000
#define MAX_Q 50000

using namespace std;

const double PI = acos(-1.0);

struct Transformation {
    char type;
    double a, b; // Parameters: dx, dy for Translate, sx, sy for Scale, d for Rotate
};

Transformation transformations[MAX_N];

pair<double, double> apply_transformations(double x, double y, int l, int r) {
    for (int i = l; i <= r; i++) {
        auto t = transformations[i];
        if (t.type == 'T') { // Translate
            x += t.a;
            y += t.b;
        } else if (t.type == 'S') { // Scale
            x *= t.a;
            y *= t.b;
        } else if (t.type == 'R') { // Rotate
            double rad = t.a * PI / 180.0;
            double new_x = x * cos(rad) - y * sin(rad);
            double new_y = x * sin(rad) + y * cos(rad);
            x = new_x;
            y = new_y;
        }
    }
    return {x, y};
}

int main() {
    int n, q;
    cin >> n >> q;
    
    for (int i = 0; i < n; i++) {
        string op;
        cin >> op;
        set_transformation(op, i);
    }
    
    for (int i = 0; i < q; i++) {
        char type;
        cin >> type;
        if (type == 'Q') {
            int x, y, l, r;
            cin >> x >> y >> l >> r;
            auto [new_x, new_y] = apply_transformations(x, y, l, r);
            cout << fixed << setprecision(5);
            cout << "(" << x << "," << y << "): " << new_x << " " << new_y << "\n";
        } else if (type == 'U') {
            int idx;
            string op;
            cin >> idx >> op;
            set_transformation(op, idx);
        }
    }
    return 0;
}

void set_transformation(std::string &op, int idx)
{
    if (op == "Translate")
    {
        double dx, dy;
        cin >> dx >> dy;
        transformations[idx] = {'T', dx, dy};
    }
    else if (op == "Scale")
    {
        double sx, sy;
        cin >> sx >> sy;
        transformations[idx] = {'S', sx, sy};
    }
    else if (op == "Rotate")
    {
        double d;
        cin >> d;
        transformations[idx] = {'R', d, 0};
    }
}
