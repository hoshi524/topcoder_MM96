#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;
typedef long long ll;

inline unsigned get_random() {
  static unsigned y = 2463534242;
  return y ^= (y ^= (y ^= y << 13) >> 17) << 5;
}

constexpr int STATE_MAX = 1 << 4;
constexpr int ROW = 1 << 7;
constexpr int MAX_X = ROW * ROW;
constexpr int MAX_C = 4;
constexpr int D[6][2] = {{1, ROW},  {-1, ROW}, {-1, -ROW},
                         {1, -ROW}, {-1, 1},   {-ROW, ROW}};
int H, W, N, C;

struct State;
struct Neighbors {
  State *state;
  ll v;
  int16_t p1, p2, p3, p4;
  int8_t t1, t2, t3, t4;
  int8_t c1, c2, c3, c4;
};
Neighbors neighbors_[STATE_MAX];
Neighbors *neighbors[STATE_MAX];
int nsize;

struct State {
  int8_t type[MAX_X];
  int8_t color[MAX_X];
  int16_t remain[6][MAX_C];

  void init(vector<string> &lights) {
    memset(type, -1, sizeof(type));
    memset(color, -1, sizeof(color));
    memset(remain, 0, sizeof(remain));
    for (string s : lights) {
      int t = s[0] - '0';
      int c = s[1] - 'a';
      ++remain[t][c];
      if (C < c + 1) C = c + 1;
    }
    for (int i = 0; i < H + 2; ++i) {
      type[i * ROW] = type[i * ROW + W + 1] = -2;
    }
    for (int i = 0; i < W + 2; ++i) {
      type[i] = type[(H + 1) * ROW + i] = -2;
    }
    int p = (H >> 1) * ROW + (W >> 1);
    put(p, 0);
    put(p + 1, 1);
    put(p + ROW, 3);
    put(p + ROW + 1, 2);
  }

  void next(Neighbors **neighbors) {
    {
      int minR = INT_MAX, maxR = INT_MIN;
      int minC = INT_MAX, maxC = INT_MIN;
      for (int i = 1; i <= H; ++i) {
        for (int j = 1; j <= W; ++j) {
          int p = i * ROW + j;
          if (type[p] == -1) continue;
          if (minC > i) minC = i;
          if (maxC < i) maxC = i;
          if (minR > j) minR = j;
          if (maxR < j) maxR = j;
        }
      }
      if (minR == 1 && maxR < W) {
        for (int i = 1; i <= H; ++i) {
          for (int j = W; j > 1; --j) {
            int p = i * ROW + j;
            type[p] = type[p - 1];
            color[p] = color[p - 1];
            type[p - 1] = -1;
            color[p - 1] = -1;
          }
        }
      }
      if (minR > 1 && maxR == W) {
        for (int i = 1; i <= H; ++i) {
          for (int j = 1; j < W; ++j) {
            int p = i * ROW + j;
            type[p] = type[p + 1];
            color[p] = color[p + 1];
            type[p + 1] = -1;
            color[p + 1] = -1;
          }
        }
      }
      if (minC == 1 && maxC < H) {
        for (int i = H; i > 1; --i) {
          for (int j = 1; j <= W; ++j) {
            int p = i * ROW + j;
            type[p] = type[p - ROW];
            color[p] = color[p - ROW];
            type[p - ROW] = -1;
            color[p - ROW] = -1;
          }
        }
      }
      if (minC > 1 && maxC == H) {
        for (int i = 1; i < H; ++i) {
          for (int j = 1; j <= W; ++j) {
            int p = i * ROW + j;
            type[p] = type[p + ROW];
            color[p] = color[p + ROW];
            type[p + ROW] = -1;
            color[p + ROW] = -1;
          }
        }
      }
    }
    bool ok = false;
    for (int o = 0; o < 10; ++o) {
      for (int a = 0; a < MAX_X; ++a) {
        if (type[a] < 0) continue;
        for (int k = 0; k < 2; ++k) {
          int b = a + D[type[a]][k];
          if (a > b) continue;
          int pat = type[a], pac = color[a];
          int pbt = type[b], pbc = color[b];
          del({a, b});
          auto next = [&](int m, int at, int bt, int ct, int dt) {
            int c = a + m;
            int d = b + m;
            if (type[c] != -1 || type[d] != -1) return;
            if (put(a, at) && put(b, bt) && put(c, ct) && put(d, dt)) {
              ok = true;
              auto value = [&]() {
                ll x = 0;
                auto f = [](int x) { return x < 1 ? 0 : x * x; };
                for (int i = 0; i < 6; ++i)
                  for (int j = 0; j < C; ++j) x -= f(200 - remain[i][j]);
                return x;
              };
              ll v = value() * UINT_MAX + get_random();
              int in = nsize;
              while (in > 0 && neighbors[in - 1]->v < v) --in;
              if (in < STATE_MAX) {
                Neighbors *n;
                if (nsize == STATE_MAX) {
                  n = neighbors[nsize - 1];
                } else {
                  n = neighbors[nsize++];
                }
                n->state = this;
                n->v = v;
                n->p1 = a, n->t1 = at, n->c1 = color[a];
                n->p2 = b, n->t2 = bt, n->c2 = color[b];
                n->p3 = c, n->t3 = ct, n->c3 = color[c];
                n->p4 = d, n->t4 = dt, n->c4 = color[d];
                for (int t = nsize - 1; t > in; --t)
                  neighbors[t] = neighbors[t - 1];
                neighbors[in] = n;
              }
            }
            del({a, b, c, d});
          };
          if (a + 1 == b) {
            {
              static int8_t DA[] = {5, -1, -1, 5, 2, -1};
              static int8_t DB[] = {-1, 5, 5, -1, 3, -1};
              next(-ROW, DA[pat], DB[pbt], 0, 1);
            }
            {
              static int8_t DA[] = {5, -1, -1, 5, 1, -1};
              static int8_t DB[] = {-1, 5, 5, -1, 0, -1};
              next(ROW, DA[pat], DB[pbt], 3, 2);
            }
          } else {
            {
              static int8_t DA[] = {4, 4, -1, -1, -1, 2};
              static int8_t DB[] = {-1, -1, 4, 4, -1, 1};
              next(-1, DA[pat], DB[pbt], 0, 3);
            }
            {
              static int8_t DA[] = {4, 4, -1, -1, -1, 3};
              static int8_t DB[] = {-1, -1, 4, 4, -1, 0};
              next(1, DA[pat], DB[pbt], 1, 2);
            }
          }
          put(a, pat, pac);
          put(b, pbt, pbc);
        }
      }
      if (ok) return;
      for (int j = 0; j < MAX_X; ++j) {
        if (type[j] < 0) continue;
        if (get_random() & 1) continue;
        int p = j;
        int pt = type[p];
        int np = p + D[pt][0] + D[pt][1];
        auto next = [&](int nt, int8_t *DA, int8_t *DB) {
          if (type[np] == -1) {
            int a = p + D[pt][0];
            int b = p + D[pt][1];
            int pat = type[a], pbt = type[b];
            int pac = color[a], pbc = color[b];
            del({a, b});
            if (put(a, DA[pat]) && put(b, DB[pbt]) && put(np, nt)) {
              del(p);
            } else {
              del({a, b});
              put(a, pat, pac);
              put(b, pbt, pbc);
            }
          }
        };
        if (pt == 0) {
          static int8_t DA[] = {-1, -1, 5, -1, 0, -1};
          static int8_t DB[] = {-1, -1, 4, -1, -1, 0};
          next(2, DA, DB);
        } else if (pt == 2) {
          static int8_t DA[] = {5, -1, -1, -1, 2, -1};
          static int8_t DB[] = {4, -1, -1, -1, -1, 2};
          next(0, DA, DB);
        } else if (pt == 1) {
          static int8_t DA[] = {-1, -1, -1, 5, 1, -1};
          static int8_t DB[] = {-1, -1, -1, 4, -1, 1};
          next(3, DA, DB);
        } else if (pt == 3) {
          static int8_t DA[] = {-1, 5, -1, -1, 3, -1};
          static int8_t DB[] = {-1, 4, -1, -1, -1, 3};
          next(1, DA, DB);
        }
      }
    }
  }

  void print() {
    for (int i = 1; i <= H; ++i) {
      for (int j = 1; j <= W; ++j) {
        int p = i * ROW + j;
        if (type[p] == -1) {
          cerr << "   ";
        } else {
          cerr << (int)type[p] << (int)color[p] << " ";
        }
      }
      cerr << endl;
    }
    cerr << endl;
  }

  int selectColor(int a, int b, int t) {
    int x = -1, m = 0;
    for (int i = 0; i < C; ++i) {
      if (a == i || b == i || remain[t][i] < 1) continue;
      int v = (remain[t][i] << 16) + (get_random() & 0xffff);
      if (m < v) {
        m = v;
        x = i;
      }
    }
    return x;
  }

  bool put(int p, int t, int c) {
    type[p] = t;
    color[p] = c;
    assert(remain[t][c] > 0);
    --remain[t][c];
  }

  bool put(int p, int t) {
    int c = selectColor(color[p + D[t][0]], color[p + D[t][1]], t);
    if (c == -1) return false;
    put(p, t, c);
    return true;
  }

  void del(int p) {
    if (type[p] != -1) {
      ++remain[type[p]][color[p]];
      type[p] = -1;
      color[p] = -1;
    }
  }

  void del(initializer_list<int> list) {
    for (int x : list) del(x);
  }

  void neighbors(Neighbors &n) {
    del({n.p1, n.p2, n.p3, n.p4});
    put(n.p1, n.t1, n.c1);
    put(n.p2, n.t2, n.c2);
    put(n.p3, n.t3, n.c3);
    put(n.p4, n.t4, n.c4);
  }
};
State state[2][STATE_MAX];
int ssize = 1;

class GarlandOfLights {
 public:
  vector<int> create(int H_, int W_, vector<string> lights) {
    {  // init
      H = H_;
      W = W_;
      N = H * W;
      C = 0;
      for (int i = 0; i < STATE_MAX; ++i) neighbors[i] = &neighbors_[i];
      state[0][0].init(lights);
    }
    State *best;
    {  // solve
      for (int i = 0;; ++i) {
        int a = i & 1;
        int b = (i + 1) & 1;
        best = &state[a][0];
        nsize = 0;
        for (int j = 0; j < ssize; ++j) {
          state[a][j].next(neighbors);
        }
        if (nsize == 0) break;
        ssize = nsize;
        for (int j = 0; j < ssize; ++j) {
          memcpy(&state[b][j], neighbors[j]->state, sizeof(state[b][j]));
          state[b][j].neighbors(*neighbors[j]);
        }
      }
    }
    {  // output
      int position[6][MAX_C][MAX_X];
      int size[6][MAX_C];
      memset(position, -1, sizeof(position));
      memset(size, 0, sizeof(size));
      for (int i = 0, s = lights.size(); i < s; ++i) {
        int t = lights[i][0] - '0';
        int c = lights[i][1] - 'a';
        position[t][c][size[t][c]++] = i;
      }
      memset(size, 0, sizeof(size));
      bool used[MAX_X];
      memset(used, false, sizeof(used));
      vector<int> ret(N, -1);
      for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
          int p = (i + 1) * ROW + (j + 1);
          if (best->type[p] == -1) continue;
          int t = best->type[p];
          int c = best->color[p];
          int x = position[t][c][size[t][c]++];
          ret[i * W + j] = x;
          used[x] = true;
        }
      }
      for (int i = 0, j = 0; i < N; ++i) {
        if (used[i]) continue;
        while (ret[j] != -1) ++j;
        ret[j] = i;
      }
      return ret;
    }
  }
};
// -------8<------- end of solution submitted to the website -------8<-------

int main() {
  GarlandOfLights gl;
  int H, W;
  cin >> H >> W;
  vector<string> lights(H * W);
  for (int k = 0; k < H * W; ++k) {
    cin >> lights[k];
  }

  vector<int> ret = gl.create(H, W, lights);
  cout << ret.size() << endl;
  for (int i = 0; i < (int)ret.size(); ++i) cout << ret[i] << endl;
  cerr.flush();
  cout.flush();
}
