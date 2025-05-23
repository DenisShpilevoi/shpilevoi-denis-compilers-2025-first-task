#include <algorithm>
#include <iostream>
#include <vector>
#include "5_9.cpp"

// Test for LR(k)-ness

bool Check_k(Grammar G, size_t k) {
  V_k_G_collection Y = FindCollection(G, k);
  for (auto s : Y.collection) {
    for (auto sit1 : s.set) {
      for (auto sit2 : s.set) {
        if (!(sit1 == sit2)) {
          sentential_form  betta_u = sit2.rule.right_part;
          betta_u.str_.erase(betta_u.str_.begin(), betta_u.str_.begin() + sit2.point); //
          for (size_t i = 0; i < sit2.u.size(); i++) {
            betta_u.str_.push_back({sit2.u[i], false});
          }
          First_k eff = EFF(k, betta_u, G);
          if (sit1.point == sit1.rule.right_part.str_.size() && (eff.set.find(sit1.u) != eff.set.end())) {
            return false;
          }
        }
      }
    }
  }
  
  return true;
}


size_t FindCorrectK(Grammar G) {
  
  size_t k = 0;
  while (true) {
    if (Check_k(G, k)) break;
    ++k;
  }
  
  return k;
}
