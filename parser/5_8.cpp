#include <algorithm>
#include <iostream>
#include <vector>
#include "5_5.cpp"

// Construction of V_k_G

V_k_G GOTO(V_k_G A_old, symbol X, Grammar G, size_t k) {
  V_k_G A;
  A.active_prefix = A_old.active_prefix;
  A.active_prefix.str_.push_back(X);

  for (auto elem : A_old.set) {
    if (elem.rule.right_part.str_.size() && elem.rule.right_part.str_[elem.point].name_ == X.name_) { // может быть сег фолт
      A.set.insert({elem.rule, elem.point + 1, elem.u});
    }
  }
  V_k_G A_prev = A;
  
  while (true) {
    for (auto elem : A_prev.set) {
      if (elem.rule.right_part.str_.size() && elem.rule.right_part.str_[elem.point].type_) {
        for (auto rule : G.rules) {
          if (elem.rule.right_part.str_[elem.point].name_ == rule.left_part) {

            sentential_form alpha_u = elem.rule.right_part;
            alpha_u.str_.erase(alpha_u.str_.begin(), alpha_u.str_.begin() + elem.point + 1);

            for (size_t i = 0; i < elem.u.size(); i++) {
                alpha_u.str_.push_back({elem.u[i], false});
            }
            First_k f = FIRST(k, alpha_u, G);

            for (auto fir : f.set) {
                A.set.insert({rule, 0, fir});
            }
          }
        }
      }
    }
    if (A.set.size() == A_prev.set.size()) {
      break;
    }
    A_prev = A;    
  }
  return A;
}

V_k_G V_eps(Grammar G, size_t k) {
  
  V_k_G A;
  A.active_prefix = {};
  for (CFL_rule elem : G.rules) {
    if (elem.left_part == G.start) {
      LR_k_situation s = {elem, 0, std::vector<char>()};
      A.set.insert(s);
    }
  }
  V_k_G A_prev = A;
  
  while (true) {
    for (auto elem : A_prev.set) {
      if (elem.rule.right_part.str_.size() && elem.rule.right_part.str_[0].type_) {
        for (auto rule : G.rules) {
          if (elem.rule.right_part.str_[0].name_ == rule.left_part) {

            sentential_form alpha_u = elem.rule.right_part;
            alpha_u.str_.erase(alpha_u.str_.begin());

            for (size_t i = 0; i < elem.u.size(); i++) {
                alpha_u.str_.push_back({elem.u[i], false});
            }
            First_k f = FIRST(k, alpha_u, G);

            for (auto fir : f.set) {
                A.set.insert({rule, 0, fir});
            }
          }
        }
      }
    }
    if (A.set.size() == A_prev.set.size()) {
      break;
    }
    A_prev = A;    
  }
  return A;
}
