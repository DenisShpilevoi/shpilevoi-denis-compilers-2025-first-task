#include <algorithm>
#include <iostream>
#include <vector>
#include "lib.cpp"

// Computation of FIRST and EFF


void f_(const std::vector<std::vector<std::vector<char>>>& arg, size_t id, std::vector<std::vector<std::vector<char>>>& ans) {
  if (id == arg.size()) {
    return;
  }
  if (id == 0) {
    for (const auto& item : arg[id]) {
      ans.emplace_back(std::vector<std::vector<char>>(1, item));
    }
    f_(arg, id + 1, ans);
    return;
  }
  auto cur_ans = std::move(ans);
  ans.clear();
  for (const auto& cur_v : cur_ans) {
    for (const auto& item : arg[id]) {
      auto tmp_v = cur_v;
      tmp_v.emplace_back(item);
      ans.emplace_back(std::move(tmp_v));
    }
  }
  f_(arg, id + 1, ans);
}

std::set<std::vector<char>> addPrefixes(const std::vector<std::vector<char>>& strings, int k) {
  std::set<std::vector<char>> prefixes;

  for (const auto& string : strings) {
    if (string.size() >= k) {
      prefixes.insert(std::vector<char>(string.begin(), string.begin() + k));
    } else {
      prefixes.insert(string);
    }
  }

  return prefixes;
}


void F_i_refill(size_t k, First_k& F, std::vector<First_k>& firsts) {
  std::vector<std::vector<std::vector<char>>> v_firsts;
  v_firsts.resize(firsts.size());
  
  for (size_t i = 0; i < firsts.size(); ++i) {
    v_firsts[i].assign(firsts[i].set.begin(), firsts[i].set.end()); 
  }
  std::vector<std::vector<std::vector<char>>> sum_of_firsts;
  f_(v_firsts, 0, sum_of_firsts);
  
  // for (size_t i = 0; i < sum_of_firsts.size(); i++) {
  //   for (size_t j = 0; j < sum_of_firsts[i].size(); j++) {
  //     if (sum_of_firsts[i][j].empty() && sum_of_firsts[i].size() > 1) {
  //       sum_of_firsts[i].erase(sum_of_firsts[i].begin() + j);
  //       j--;
  //     }
  //   }
  // }
  
  std::vector<std::vector<char>> strs;
  std::vector<char> str;
  if (!sum_of_firsts.empty()) {
    for (size_t i = 0; i < sum_of_firsts.size(); i++) {
      for (size_t j = 0; j < sum_of_firsts[i].size(); j++) {
        for (size_t m = 0; m < sum_of_firsts[i][j].size(); m++) {
          str.push_back(sum_of_firsts[i][j][m]);
        }
      }
      strs.push_back(str);
      str.clear();
    }
    std::set<std::vector<char>> sum_k_of_firsts = addPrefixes(strs, k);
    F.set.insert(sum_k_of_firsts.begin(), sum_k_of_firsts.end());
  }
}

First_k F_i(size_t k, symbol X, Grammar G, size_t m) {
  if (X.type_ == false) {
    return {{{X.name_}}};
  }

  First_k F;

  if (m == 0) {
    for (size_t r_num = 0; r_num < G.rules.size(); r_num++) {
      if (G.rules[r_num].left_part == X.name_) {
        std::vector<char> f;
        if (G.rules[r_num].right_part.str_.size() == 0) {
          F.set.insert(std::vector<char>());
          continue;
        }
        for (size_t s_num = 0; s_num < G.rules[r_num].right_part.str_.size() && s_num < k; s_num++) {
          
            if (G.rules[r_num].right_part.str_[s_num].type_) {
              f.clear();
              break;
            }

            f.push_back(G.rules[r_num].right_part.str_[s_num].name_);

            if (s_num == k - 1 && k <= G.rules[r_num].right_part.str_.size() ||
                s_num == G.rules[r_num].right_part.str_.size() - 1 && k >= G.rules[r_num].right_part.str_.size()) {
              F.set.insert(f);
              f.clear();
              break;
            }
           
        }
      }
    }
    return F;
  }

  First_k F_prev_i = F_i(k, X, G, m - 1);
  F = First_k(F_prev_i);

  for (size_t r_num = 0; r_num < G.rules.size(); r_num++) {
    if (G.rules[r_num].left_part == X.name_) {
      std::vector<First_k> firsts;
      for (size_t s_num = 0; s_num < G.rules[r_num].right_part.str_.size(); s_num++) {
        firsts.push_back(F_i(k, G.rules[r_num].right_part.str_[s_num], G, m - 1));
      }
      F_i_refill(k, F, firsts);
      firsts.clear();
    }
  }
  return F;
}

First_k FIRST_X(size_t k, symbol X, Grammar G) {
  std::vector<First_k> F_new, F_old;
  for (char elem : G.nonterminals) {
    F_old.push_back(F_i(k, {elem, true}, G, 0));
    F_new.push_back(F_i(k, {elem, true}, G, 1));
  }
  size_t i = 1;
  
  for (size_t j = 0; j < F_old.size(); j++) {
    if (F_old[j].set != F_new[j].set) {
      i++;
      F_old = F_new;
      F_new.clear();
      for (char elem : G.nonterminals) {
        F_new.push_back(F_i(k, {elem, true}, G, i));
      }
      j = -1;
      continue;
    }
  }

  return F_i(k, X, G, i);
}

First_k FIRST(size_t k, sentential_form alpha, Grammar G) {
  if (alpha.str_.size() == 0) {
    return {{{}}};
  }
  First_k first;
  std::vector<First_k> firsts;

  for (size_t i = 0; i < alpha.str_.size(); i++) {
    First_k F = FIRST_X(k, alpha.str_[i], G);
    firsts.push_back(F);
  }

  std::vector<std::vector<std::vector<char>>> v_firsts;
  v_firsts.resize(firsts.size());
  
  for (size_t i = 0; i < firsts.size(); ++i) {
    v_firsts[i].assign(firsts[i].set.begin(), firsts[i].set.end()); 
  }
  std::vector<std::vector<std::vector<char>>> sum_of_firsts;
  f_(v_firsts, 0, sum_of_firsts);
  
  std::vector<std::vector<char>> strs;
  std::vector<char> str;
  if (!sum_of_firsts.empty()) {
    for (size_t i = 0; i < sum_of_firsts.size(); i++) {
      for (size_t j = 0; j < sum_of_firsts[i].size(); j++) {
        for (size_t m = 0; m < sum_of_firsts[i][j].size(); m++) {
          str.push_back(sum_of_firsts[i][j][m]);
        }
      }
      strs.push_back(str);
      str.clear();
    }
    std::set<std::vector<char>> sum_k_of_firsts = addPrefixes(strs, k);
    first.set.insert(sum_k_of_firsts.begin(), sum_k_of_firsts.end());
  }
  return first;
}

std::set<std::vector<char>> addPrefixes_(const std::vector<std::vector<char>>& strings, int k) {
  std::set<std::vector<char>> prefixes;

  for (const auto& string : strings) {
    if (string.size() >= k) {
      prefixes.insert(std::vector<char>(string.begin(), string.begin() + k));
    } else {
      prefixes.insert(string);
    }
  }

  return prefixes;
}


void F_i_refill_(size_t k, First_k& F, std::vector<First_k>& firsts) {
  std::vector<std::vector<std::vector<char>>> v_firsts;
  v_firsts.resize(firsts.size());
  
  for (size_t i = 0; i < firsts.size(); ++i) {
    v_firsts[i].assign(firsts[i].set.begin(), firsts[i].set.end()); 
  }
  std::vector<std::vector<std::vector<char>>> sum_of_firsts;
  f_(v_firsts, 0, sum_of_firsts);
  
  // for (size_t i = 0; i < sum_of_firsts.size(); i++) {
  //   for (size_t j = 0; j < sum_of_firsts[i].size(); j++) {
  //     if (sum_of_firsts[i][j].empty() && sum_of_firsts[i].size() > 1) {
  //       sum_of_firsts[i].erase(sum_of_firsts[i].begin() + j);
  //       j--;
  //     }
  //   }
  // }
  
  std::vector<std::vector<char>> strs;
  std::vector<char> str;
  if (!sum_of_firsts.empty()) {
    for (size_t i = 0; i < sum_of_firsts.size(); i++) {
      for (size_t j = 0; j < sum_of_firsts[i].size(); j++) {
        for (size_t m = 0; m < sum_of_firsts[i][j].size(); m++) {
          str.push_back(sum_of_firsts[i][j][m]);
        }
      }
      strs.push_back(str);
      str.clear();
    }
    std::set<std::vector<char>> sum_k_of_firsts = addPrefixes_(strs, k);
    F.set.insert(sum_k_of_firsts.begin(), sum_k_of_firsts.end());
  }
}

First_k F_i_(size_t k, symbol X, Grammar G, size_t m) {
  if (X.type_ == false) {
    return {{{X.name_}}};
  }

  First_k F;

  if (m == 0) {
    for (size_t r_num = 0; r_num < G.rules.size(); r_num++) {
      if (G.rules[r_num].left_part == X.name_ && G.rules[r_num].right_part.str_.size()) {
        std::vector<char> f_;
        if (G.rules[r_num].right_part.str_.size() == 0) {
          F.set.insert(std::vector<char>());
          continue;
        }
        for (size_t s_num = 0; s_num < G.rules[r_num].right_part.str_.size() && s_num < k; s_num++) {
          
            if (G.rules[r_num].right_part.str_[s_num].type_) {
              f_.clear();
              break;
            }

            f_.push_back(G.rules[r_num].right_part.str_[s_num].name_);

            if (s_num == k - 1 && k <= G.rules[r_num].right_part.str_.size() ||
                s_num == G.rules[r_num].right_part.str_.size() - 1 && k >= G.rules[r_num].right_part.str_.size()) {
              F.set.insert(f_);
              f_.clear();
              break;
            }
           
        }
      }
    }
    return F;
  }

  First_k F_prev_i = F_i_(k, X, G, m - 1);
  F = First_k(F_prev_i);

  for (size_t r_num = 0; r_num < G.rules.size(); r_num++) {
    if (G.rules[r_num].left_part == X.name_ && G.rules[r_num].right_part.str_.size()) {
      std::vector<First_k> firsts;
      firsts.push_back(F_i_(k, G.rules[r_num].right_part.str_[0], G, m - 1));
      for (size_t s_num = 1; s_num < G.rules[r_num].right_part.str_.size(); s_num++) {
        firsts.push_back(F_i(k, G.rules[r_num].right_part.str_[s_num], G, m - 1));
      }
      F_i_refill_(k, F, firsts);
      firsts.clear();
    }
  }
  return F;
}

First_k FIRST_X_(size_t k, symbol X, Grammar G) {
  std::vector<First_k> F_new, F_old;
  for (char elem : G.nonterminals) {
    F_old.push_back(F_i_(k, {elem, true}, G, 0));
    F_new.push_back(F_i_(k, {elem, true}, G, 1));
  }
  size_t i = 1;

  for (size_t j = 0; j < F_old.size(); j++) {
    if (F_old[j].set != F_new[j].set) {
      i++;
      F_old = F_new;
      F_new.clear();
      for (char elem : G.nonterminals) {
        F_new.push_back(F_i_(k, {elem, true}, G, i));
      }
      j = -1;
      continue;
    }
  }

  return F_i_(k, X, G, i);
}

First_k EFF_0(size_t k, symbol X, Grammar G) {
  std::vector<First_k> F_new, F_old;
  for (char elem : G.nonterminals) {
    F_old.push_back(F_i_(k, {elem, true}, G, 0));
    F_new.push_back(F_i_(k, {elem, true}, G, 1));
  }
  size_t i = 1;

  for (size_t j = 0; j < F_old.size(); j++) {
    if (F_old[j].set != F_new[j].set) {
      i++;
      F_old = F_new;
      F_new.clear();
      for (char elem : G.nonterminals) {
        F_new.push_back(F_i_(k, {elem, true}, G, i));
      }
      j = -1;
      continue;
    }
  }

  return F_i_(k, X, G, i);
}

First_k EFF(size_t k, sentential_form alpha, Grammar G) {
  if (alpha.str_.size() == 0) {
    return {{{}}};
  }
  if (!alpha.str_[0].type_) {
    return FIRST(k, alpha, G);
  }
  First_k first;
  std::vector<First_k> firsts;

  firsts.push_back(EFF_0(k, alpha.str_[0], G));

  for (size_t i = 1; i < alpha.str_.size(); i++) {
    First_k F = FIRST_X_(k, alpha.str_[i], G);
    firsts.push_back(F);
  }

  std::vector<std::vector<std::vector<char>>> v_firsts;
  v_firsts.resize(firsts.size());
  
  for (size_t i = 0; i < firsts.size(); ++i) {
    v_firsts[i].assign(firsts[i].set.begin(), firsts[i].set.end()); 
  }
  std::vector<std::vector<std::vector<char>>> sum_of_firsts;
  f_(v_firsts, 0, sum_of_firsts);
  
  std::vector<std::vector<char>> strs;
  std::vector<char> str;
  if (!sum_of_firsts.empty()) {
    for (size_t i = 0; i < sum_of_firsts.size(); i++) {
      for (size_t j = 0; j < sum_of_firsts[i].size(); j++) {
        for (size_t m = 0; m < sum_of_firsts[i][j].size(); m++) {
          str.push_back(sum_of_firsts[i][j][m]);
        }
      }
      strs.push_back(str);
      str.clear();
    }
    std::set<std::vector<char>> sum_k_of_firsts = addPrefixes_(strs, k);
    first.set.insert(sum_k_of_firsts.begin(), sum_k_of_firsts.end());
  }
  return first;
}
