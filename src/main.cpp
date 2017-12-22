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
int H, W, N;
int ps;
int type[MAX_X];
int color[MAX_X];
int position[MAX_X];
int remain[6][MAX_C];

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
}

int selectColor(int a, int b, int t) {
  int x = -1, m = 0;
  for (int i = 0; i < MAX_C; ++i) {
    if (a == i || b == i || remain[t][i] == 0) continue;
    int v = (remain[t][i] << 16) + (get_random() & 0xff);
    if (m < v) {
      m = v;
      x = i;
    }
  }
  return x;
}

inline void padd(int p) { position[ps++] = p; }

void padd(initializer_list<int> list) {
  for (int x : list) padd(x);
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
    {  // init
      H = H_;
      W = W_;
      N = H * W;
      memset(type, -1, sizeof(type));
      memset(color, -1, sizeof(color));
      memset(position, -1, sizeof(position));
      memset(remain, 0, sizeof(remain));
      for (string s : lights) {
        ++remain[s[0] - '0'][s[1] - 'a'];
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
      padd({p, p + 1, p + ROW, p + ROW + 1});
      std::mt19937 engine(get_random());
    start:
      shuffle(position, position + ps, engine);
      for (int j = 0; j < ps; ++j) {
        for (int k = 0; k < 2; ++k) {
          int a = position[j];
          int b = a + D[type[a]][k];
          if (a > b) swap(a, b);
          int pat = type[a], pbt = type[b];
          int pac = color[a], pbc = color[b];
          del({a, b});
          auto next = [&](int d, int *DA, int *DB, int t1, int t2) {
            if (type[a + d] == -1 && type[b + d] == -1) {
              if (put(a, DA[pat]) && put(b, DB[pbt]) && put(a + d, t1) &&
                  put(b + d, t2)) {
                padd({a + d, b + d});
                return true;
              }
              del({a, b, a + d, b + d});
            }
            return false;
          };
          if (a + 1 == b) {
            {
              static int DA[] = {5, -1, -1, 5, 2, -1};
              static int DB[] = {-1, 5, 5, -1, 3, -1};
              if (next(-ROW, DA, DB, 0, 1)) goto start;
            }
            {
              static int DA[] = {5, -1, -1, 5, 1, -1};
              static int DB[] = {-1, 5, 5, -1, 0, -1};
              if (next(ROW, DA, DB, 3, 2)) goto start;
            }
          } else {
            {
              static int DA[] = {4, 4, -1, -1, -1, 2};
              static int DB[] = {-1, -1, 4, 4, -1, 1};
              if (next(-1, DA, DB, 0, 3)) goto start;
            }
            {
              static int DA[] = {4, 4, -1, -1, -1, 3};
              static int DB[] = {-1, -1, 4, 4, -1, 0};
              if (next(1, DA, DB, 1, 2)) goto start;
            }
          }
          put(a, pat, pac);
          put(b, pbt, pbc);
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
          if (type[p] == -1) continue;
          int t = type[p];
          int c = color[p];
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
