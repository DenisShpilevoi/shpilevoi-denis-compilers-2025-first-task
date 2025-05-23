#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

#include "LR-parser.cpp"


int main() {

  Grammar G;
  
  CFL_rule rule;
  bool find_start = false;

  std::string line;
  while (std::getline(std::cin, line) && line != "/") {
    
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    if (!find_start) {
      G.start = line[0];
      find_start = true;
    }

    if (!line.empty()) {
      rule.left_part = line[0];

      for (size_t i = 3; i < line.size(); ++i) {
        rule.right_part.str_.push_back({line[i], (line[i] >= 'A' && line[i] <= 'Z')});
      }

      G.rules.push_back(rule);

      for (size_t i = 0; i < line.size(); ++i) {
        if (i != 1 && i != 2) {
          if (line[i] >= 'A' && line[i] <= 'Z') { 
            G.nonterminals.insert(line[i]);
          } else {
            G.terminals.insert(line[i]);
          }
        }
      }
    }
    rule.right_part.str_.clear();
  }

  const std::vector<LR_k_table> T = TableCollection(G);
  
  std::vector<char> w;
  char c;
  Augmenting(G);
  
  while (true) {

    while (std::cin.get(c) && c != '\n') {
      w.push_back(c);
    }
    std::vector<int> parse = parsing(w, FindCorrectK(G), T, G);
    
    for (int elem : parse) {
      std::cout << elem;
    }
    std::cout << std::endl;
    std::cout << std::endl;
    parse.clear();
    w.clear();
  }
  return 0;
}
