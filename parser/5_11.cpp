#include <algorithm>
#include <iostream>
#include <vector>
#include "5_10.cpp"

// Construction of canonical set of LR(k) tables

// Значения f:
// - перенос - -1
// - свертка - номер
// - допуск  - -2
// - ошибка  - -3

// Значения g:
// -1 - ошибка
// Другое число - номер таблицы


void Find_Sigma_star_k(const std::set<char>& characters, int k, std::vector<std::vector<char>>& result, std::vector<char>& current, int length) {
  result.push_back(current);
  if (length < k) {
    for (char c : characters) {
      current.push_back(c);
      Find_Sigma_star_k(characters, k, result, current, length + 1);
      current.pop_back();
    }
  }
}

bool compareVectors(const std::vector<char>& a, const std::vector<char>& b) {
  if (a.size() < b.size()) return true;
  else if (a.size() > b.size()) return false;

  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] < b[i]) return true;
    else if (a[i] > b[i]) return false;
  }
  return false;
}

void Augmenting(Grammar& G) {
  G.nonterminals.insert('$');
  G.rules.insert(G.rules.begin(), {'$', {{{G.start, true}}}});
  G.start = '$';
}


bool FirstCondition(std::vector<char> u, V_k_G A, Grammar G, size_t k) {
  for (LR_k_situation elem : A.set) {
    if (elem.point < elem.rule.right_part.str_.size()) {
      sentential_form betta_v = elem.rule.right_part;
      betta_v.str_.erase(betta_v.str_.begin(), betta_v.str_.begin() + elem.point);
      for (size_t i = 0; i < elem.u.size(); ++i) {
        betta_v.str_.push_back({elem.u[i], false});
      }
      First_k eff = EFF(k, betta_v, G);
      if (eff.set.find(u) != eff.set.end()) {
        return true;
      }
    }
  }
  return false;
}

std::pair<bool, size_t> SecondCondition(std::vector<char> u, Grammar G, V_k_G A) {
  for (LR_k_situation elem : A.set) {
    if (elem.point == elem.rule.right_part.str_.size() && elem.u == u && elem.rule.left_part != '$') {
      for (size_t i = 0; i < G.rules.size(); ++i) {
        if (G.rules[i] == elem.rule) {
          return {true, i};
        }
      }
    }
  }
  return {false, 0};
}

bool ThirdCondition(std::vector<char> u, Grammar G, V_k_G A) {
  for (LR_k_situation elem : A.set) {
    if (elem.point == elem.rule.right_part.str_.size() && elem.u == u && elem.rule.left_part == '$') {
      return true;
    }
  }
  return false;
}

int f(std::vector<char> u, Grammar G, V_k_G A, size_t k) {
  if (FirstCondition(u, A, G, k))          return -1;
  else if (SecondCondition(u, G, A).first) return SecondCondition(u, G, A).second;
  else if (ThirdCondition(u, G, A))        return -2;
                                           return -3;
}

int g(symbol X, Grammar G, V_k_G A, size_t k, std::vector<V_k_G> Y) {
  if (GOTO(A, X, G, k).set.size()) {
    for (size_t i = 0; i < Y.size(); ++i) {
      if (GOTO(A, X, G, k).set == Y[i].set) {
        return i;
      }
    }
  }
  return -1;
}

LR_k_table LR_k_Table(V_k_G A, std::vector<std::vector<char>> Sigma_star_k, Grammar G, size_t k, std::vector<V_k_G> Y) {

  LR_k_table t;

  for (size_t i = 0; i < Sigma_star_k.size(); i++) {
    t.f[Sigma_star_k[i]] = f(Sigma_star_k[i], G, A, k);
  }

  for (char X : G.nonterminals) {
    if (X != '$') {
      t.g[X] = g({X, true}, G, A, k, Y);
    }
  }

  for (char x : G.terminals) {
    t.g[x] = g({x, false}, G, A, k, Y);
  }

  return t;
}

std::vector<LR_k_table> TableCollection(Grammar G) {

  Augmenting(G);
  V_k_G_collection Y = FindCollection(G, FindCorrectK(G));

  std::vector<V_k_G> Y_vec;
  for (auto elem : Y.collection) {
    Y_vec.push_back(elem);
  }
  
  std::vector<std::vector<char>> Sigma_star_k;
  std::vector<char> current;
  Find_Sigma_star_k(G.terminals, FindCorrectK(G), Sigma_star_k, current, 0);
  
  std::sort(Sigma_star_k.begin(), Sigma_star_k.end(), compareVectors);

  size_t sds = 0;
  std::vector<LR_k_table> T;

  for (V_k_G A : Y_vec) {
    LR_k_table t = LR_k_Table(A, Sigma_star_k, G, FindCorrectK(G), Y_vec);
    T.push_back(t);
    sds++;
  }

  return T;
}
