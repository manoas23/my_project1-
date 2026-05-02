#include <iostream>
using namespace std;

const int SIZE = 10;
const int MAX = 10000;
const int FUEL = 20;

// ---------- STATE ----------
class State {
public:
    int x, y, fuel;
    char c1, c2, c3, c4;

    void show() {
        cout << "(" << x << "," << y << "," << fuel << ","
             << c1 << "," << c2 << "," << c3 << "," << c4 << ")";
    }

    bool goal() {
        return (x == 1 && y == 1 &&
                c1 == 't' && c2 == 't' &&
                c3 == 't' && c4 == 't');
    }
};

// ---------- NODE ----------
class Node {
public:
    State s;
    int parent;
    int g, h, f;
};

// ---------- GAME ----------
class Game {
public:
    Node arr[MAX];
    int count;

    int cx[4] = {2, 5, 9, 6};
    int cy[4] = {3, 2, 8, 9};

    int fx = 7, fy = 4;

    int abs1(int n) {
        if (n < 0) return -n;
        return n;
    }

    int dist(int x1, int y1, int x2, int y2) {
        return abs1(x1 - x2) + abs1(y1 - y2);
    }

    bool same(State a, State b) {
        return (a.x == b.x && a.y == b.y && a.fuel == b.fuel &&
                a.c1 == b.c1 && a.c2 == b.c2 &&
                a.c3 == b.c3 && a.c4 == b.c4);
    }

    bool seen(State s) {
        for (int i = 0; i < count; i++) {
            if (same(arr[i].s, s)) return true;
        }
        return false;
    }

    bool wall(int x, int y) {
        return ((x == 1 && y == 2) || (x == 3 && y == 4) ||
                (x == 4 && y == 7) || (x == 6 && y == 3));
    }

    void takeCoin(State &s) {
        if (s.x == cx[0] && s.y == cy[0]) s.c1 = 't';
        if (s.x == cx[1] && s.y == cy[1]) s.c2 = 't';
        if (s.x == cx[2] && s.y == cy[2]) s.c3 = 't';
        if (s.x == cx[3] && s.y == cy[3]) s.c4 = 't';
    }

    void fillFuel(State &s) {
        if (s.x == fx && s.y == fy) s.fuel = FUEL;
    }

    int h1(State s) {
        int sum = 0;

        bool t[4] = {
            s.c1 == 't',
            s.c2 == 't',
            s.c3 == 't',
            s.c4 == 't'
        };

        int x = s.x, y = s.y;

        while (true) {
            int best = 1000, id = -1;

            for (int i = 0; i < 4; i++) {
                if (!t[i]) {
                    int d = dist(x, y, cx[i], cy[i]);
                    if (d < best) {
                        best = d;
                        id = i;
                    }
                }
            }

            if (id == -1) break;

            sum += best;
            x = cx[id];
            y = cy[id];
            t[id] = true;
        }

        sum += dist(x, y, 1, 1);
        return sum;
    }

    int h2(State s) {
        int m = dist(s.x, s.y, 1, 1);

        if (s.c1 == 'f') m = max(m, dist(s.x, s.y, cx[0], cy[0]));
        if (s.c2 == 'f') m = max(m, dist(s.x, s.y, cx[1], cy[1]));
        if (s.c3 == 'f') m = max(m, dist(s.x, s.y, cx[2], cy[2]));
        if (s.c4 == 'f') m = max(m, dist(s.x, s.y, cx[3], cy[3]));

        return m;
    }

    int bestG() {
        int b = -1;
        for (int i = 0; i < count; i++) {
            if (arr[i].h != 1000 &&
                (b == -1 || arr[i].h < arr[b].h)) {
                b = i;
            }
        }
        return b;
    }

    int bestA() {
        int b = -1;
        for (int i = 0; i < count; i++) {
            if (arr[i].f != 1000 &&
                (b == -1 || arr[i].f < arr[b].f)) {
                b = i;
            }
        }
        return b;
    }

    void printPath(int i) {
        int p[200], n = 0;

        while (i != -1) {
            p[n++] = i;
            i = arr[i].parent;
        }

        cout << "\nPath:\n";
        for (int j = n - 1; j >= 0; j--) {
            arr[p[j]].s.show();
            cout << endl;
        }

        cout << "Cost = " << n - 1 << endl;
    }

    void greedy(State st, int type) {
        count = 0;

        arr[0].s = st;
        arr[0].parent = -1;
        arr[0].g = 0;
        arr[0].h = (type == 1 ? h1(st) : h2(st));
        arr[0].f = arr[0].h;

        count++;

        while (true) {
            int cur = bestG();
            if (cur == -1) break;

            State s = arr[cur].s;

            cout << "Visited: ";
            s.show();
            cout << endl;

            if (s.goal()) {
                cout << "Goal (Greedy)\n";
                printPath(cur);
                return;
            }

            arr[cur].h = 1000;

            int dx[4] = {-1, 1, 0, 0};
            int dy[4] = {0, 0, -1, 1};

            for (int i = 0; i < 4; i++) {
                State nx = s;

                nx.x += dx[i];
                nx.y += dy[i];
                nx.fuel--;

                if (nx.x < 0 || nx.x >= SIZE ||
                    nx.y < 0 || nx.y >= SIZE) continue;

                if (wall(nx.x, nx.y)) continue;
                if (nx.fuel < 0) continue;

                takeCoin(nx);
                fillFuel(nx);

                if (!seen(nx)) {
                    arr[count].s = nx;
                    arr[count].parent = cur;
                    arr[count].g = arr[cur].g + 1;
                    arr[count].h = (type == 1 ? h1(nx) : h2(nx));
                    arr[count].f = arr[count].h;
                    count++;
                }
            }
        }
    }

    void astar(State st, int type) {
        count = 0;

        arr[0].s = st;
        arr[0].parent = -1;
        arr[0].g = 0;
        arr[0].h = (type == 1 ? h1(st) : h2(st));
        arr[0].f = arr[0].h;

        count++;

        while (true) {
            int cur = bestA();
            if (cur == -1) break;

            State s = arr[cur].s;

            cout << "Visited: ";
            s.show();
            cout << endl;

            if (s.goal()) {
                cout << "Goal (A*)\n";
                printPath(cur);
                return;
            }

            arr[cur].f = 1000;

            int dx[4] = {-1, 1, 0, 0};
            int dy[4] = {0, 0, -1, 1};

            for (int i = 0; i < 4; i++) {
                State nx = s;

                nx.x += dx[i];
                nx.y += dy[i];
                nx.fuel--;

                if (nx.x < 0 || nx.x >= SIZE ||
                    nx.y < 0 || nx.y >= SIZE) continue;

                if (wall(nx.x, nx.y)) continue;
                if (nx.fuel < 0) continue;

                takeCoin(nx);
                fillFuel(nx);

                if (!seen(nx)) {
                    arr[count].s = nx;
                    arr[count].parent = cur;
                    arr[count].g = arr[cur].g + 1;
                    arr[count].h = (type == 1 ? h1(nx) : h2(nx));
                    arr[count].f = arr[count].g + arr[count].h;
                    count++;
                }
            }
        }
    }
};

// ---------- MAIN ----------
int main() {
    Game g;
    State s;

    cout << "x y fuel: ";
    cin >> s.x >> s.y >> s.fuel;

    s.c1 = s.c2 = s.c3 = s.c4 = 'f';

    g.takeCoin(s);
    g.fillFuel(s);

    cout << "\nGreedy h1\n";
    g.greedy(s, 1);

    cout << "\nGreedy h2\n";
    g.greedy(s, 2);

    cout << "\nA* h1\n";
    g.astar(s, 1);

    cout << "\nA* h2\n";
    g.astar(s, 2);
}
