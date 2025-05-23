#include <algorithm>
#include <iostream>
#include <vector>
#include "5_11.cpp"

// LR(k) parsing algorithm

std::vector<char> lookahead(std::vector<char> w, size_t k, size_t point) {
  if (k == 0) {
    return {};
  }
  if (point < 0 || point >= w.size() || k <= 0 || point + k > w.size()) {
    return std::vector<char>(w.begin() + point, w.end());
  }
  return std::vector<char>(w.begin() + point, w.begin() + point + k);
}

std::vector<int> parsing(std::vector<char> w, size_t k, std::vector<LR_k_table> T, Grammar G) {
  size_t point = 0;
  LR_configuration config;
  config.x = w;
  int t = 0;
  while (true) {
    std::vector<char> u = lookahead(w, k, point);
    int t1 = T[t].f[u]; // t1 = -1 // t1 = 3
    if (t1 == -1) {
      if (point == w.size()) {
        return {-1};
      }
      if (T[t].g[config.x[point]] == -1) {
        config.p.clear();
        config.p.push_back(-1);
        break;
      }
      config.a.push_back({{config.x[point], false}, T[t].g[config.x[point]]}); // a2 to stack
      t = T[t].g[config.x[point]]; // t = 2
      ++point; // point = 1
    } 
    else if (t1 >= 0) {

      config.p.push_back(T[t].f[u]); // 3 to rules
      config.a.resize(config.a.size() - G.rules[T[t].f[u]].right_part.str_.size()); // clear stack
      if (config.a.size()) {
        t = config.a[config.a.size() - 1].second;
      } else {
        t = 0; // t = 0
      }
      config.a.push_back({{G.rules[t1].left_part, true}, T[t].g[G.rules[t1].left_part]});
      t = T[t].g[G.rules[t1].left_part];
    } else if (t1 == -2) {
      if (point != w.size()) return {-1};
      config.p.push_back(0);
      break;
    } else if (t1 == -3) {
      config.p.clear();
      config.p.push_back(-1);
      break;
    }
  }
  std::vector<int> rules = config.p;
  config.a.clear();
  config.x.clear();
  config.p.clear();
  return rules;
}
