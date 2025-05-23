#include <algorithm>
#include <iostream>
#include <vector>
#include "5_8.cpp"

// Collection of sets of valid LR(k) items to G

bool FindInCollection(V_k_G A, V_k_G_collection Y) {
  for (V_k_G elem : Y.collection) {
    if (A.set == elem.set) return false;
  }
  return true;
}

V_k_G_collection FindCollection(Grammar G, size_t k) {
  V_k_G_collection Y;
  Y.collection.insert(V_eps(G, k));
  V_k_G_collection Y_prev = Y;
  
  while (true) {
    for (auto elem : Y_prev.collection) {
      if (!elem.mark) {
        for (char X : G.nonterminals) {
          if (X != '$') {
            V_k_G A = GOTO(elem, {X, true}, G, k);
            if (FindInCollection(A, Y)) {
              if (A.set.size()) {
                Y.collection.insert({A.set, false, A.active_prefix});
              }
            }
          }
        }
        for (char X : G.terminals) {
          V_k_G A = GOTO(elem, {X, false}, G, k);
          if (FindInCollection(A, Y)) {
            if (A.set.size()) {
              Y.collection.insert({A.set, false, A.active_prefix});
            }
          }
        }
        Y.collection.erase(elem);
        Y.collection.insert({elem.set, true, elem.active_prefix});
      }
    }
    bool all = true;
    for (auto elem : Y.collection) {
      if(!elem.mark) all = false;
    }
    Y_prev = Y;
    if (all) break;
  }
  return Y;
}
