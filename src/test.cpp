#include <bits/stdc++.h>
#include <sys/time.h>
using namespace std;

inline unsigned get_random() {
  static unsigned y = 2463534242;
  return y ^= (y ^= (y ^= y << 13) >> 17) << 5;
}

constexpr int ROW = 1 << 7;
constexpr int MAX_X = ROW * ROW;
constexpr int MAX_C = 4;
constexpr int D[6][2] = {{1, ROW},  {-1, ROW}, {-1, -ROW},
                         {1, -ROW}, {-1, 1},   {-ROW, ROW}};
int H, W, N, C;
int ps;
int8_t type[MAX_X];
int8_t color[MAX_X];
int8_t btype[MAX_X];
int8_t bcolor[MAX_X];
int16_t remain[6][MAX_C];

void print() {
  for (int i = 1; i <= H; ++i) {
    for (int j = 1; j <= W; ++j) {
      int p = i * ROW + j;
      if (type[p] == -1) {
        cerr << "   ";
      } else {
        cerr << type[p] << color[p] << " ";
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
    int v = (remain[t][i] << 16) + (get_random() & 0xff);
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

class GarlandOfLights {
 public:
  vector<int> create(int H_, int W_, vector<string> lights) {
    int score = 0;
    for (int l = 0; l < 10; ++l) {
      {  // init
        H = H_;
        W = W_;
        N = H * W;
        C = 0;
        memset(type, -1, sizeof(type));
        memset(color, -1, sizeof(color));
        memset(remain, 0, sizeof(remain));
        for (string s : lights) {
          int t = s[0] - '0';
          int c = s[1] - 'a';
          ++remain[t][c];
          if (C < c + 1) C = c + 1;
        }
        ps = 0;
        for (int i = 0; i < H + 2; ++i) {
          type[i * ROW] = type[i * ROW + W + 1] = -2;
        }
        for (int i = 0; i < W + 2; ++i) {
          type[i] = type[(H + 1) * ROW + i] = -2;
        }
      }
      {  // solve
        int p = (H >> 1) * ROW + (W >> 1);
        put(p, 0);
        put(p + 1, 1);
        put(p + ROW, 3);
        put(p + ROW + 1, 2);
      start:
        for (int o = 0; o < 10; ++o) {
          int v = INT_MIN;
          int p1, p2, p3, p4;
          int t1, t2, t3, t4;
          int c1, c2, c3, c4;
          for (int j = 0; j < MAX_X; ++j) {
            if (type[j] < 0) continue;
            for (int k = 0; k < 2; ++k) {
              int a = j;
              int b = a + D[type[a]][k];
              if (a > b) swap(a, b);
              int pat = type[a], pbt = type[b];
              int pac = color[a], pbc = color[b];
              del({a, b});
              auto next = [&](int d, int at, int bt, int ct, int dt) {
                auto next = [&](int c, int d) {
                  if (type[c] == -1 && type[d] == -1) {
                    if (put(a, at) && put(b, bt) && put(c, ct) && put(d, dt)) {
                      int tv = 0;
                      tv += remain[type[a]][color[a]];
                      tv += remain[type[b]][color[b]];
                      tv += remain[type[c]][color[c]];
                      tv += remain[type[d]][color[d]];
                      tv -= remain[type[pat]][color[pac]];
                      tv -= remain[type[pbt]][color[pbc]];
                      tv = (tv * 0x10000) + (get_random() & 0xffff);
                      if (v < tv) {
                        v = tv;
                        p1 = a, t1 = type[a], c1 = color[a];
                        p2 = b, t2 = type[b], c2 = color[b];
                        p3 = c, t3 = type[c], c3 = color[c];
                        p4 = d, t4 = type[d], c4 = color[d];
                      }
                    }
                    del({a, b, c, d});
                  }
                };
                next(a + d, b + d);
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
          if (v > INT_MIN) {
            del({p1, p2});
            put(p1, t1, c1);
            put(p2, t2, c2);
            put(p3, t3, c3);
            put(p4, t4, c4);
            goto start;
          }
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
      {
        int s = 0;
        for (int i = 0; i < MAX_X; ++i) {
          if (type[i] > -1) ++s;
        }
        if (score < s) {
          score = s;
          memcpy(btype, type, sizeof(type));
          memcpy(bcolor, color, sizeof(color));
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
          if (btype[p] == -1) continue;
          int t = btype[p];
          int c = bcolor[p];
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
