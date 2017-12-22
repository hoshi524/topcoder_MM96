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
int H, W, N;
int ps;
int type[MAX_X];
int color[MAX_X];
int position[MAX_X];
int remain[6][MAX_C];

void print() {
  for (int i = 0; i < H; ++i) {
    for (int j = 0; j < W; ++j) {
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
    if (a == i || b == i) continue;
    if (m < remain[t][i]) {
      m = remain[t][i];
      x = i;
    }
  }
  return x;
}

int put(int p, int t) {
  constexpr int D[6][2] = {{1, ROW},  {-1, ROW}, {-1, -ROW},
                           {1, -ROW}, {-1, 1},   {-ROW, ROW}};
  int c = selectColor(color[p + D[t][0]], color[p + D[t][1]], t);
  if (type[p] == -1) position[ps++] = p;
  type[p] = t;
  color[p] = c;
  --remain[t][c];
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
    }
    {  // solve
      int p = (H >> 1) * ROW + (W >> 1);
      put(p, 0);
      put(p + 1, 1);
      put(p + ROW, 3);
      put(p + ROW + 1, 2);
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
          int p = i * ROW + j;
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
