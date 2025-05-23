#include <vector>
#include <set>
#include <unordered_set>
#include <random>
#include <unordered_map>
#include <functional>


struct symbol {
    char name_;
    bool type_;

    bool operator==(const symbol& other) const {
        return name_ == other.name_ && type_ == other.type_;
    }
    bool operator<(const symbol& other) const {
      return name_ < other.name_;
    }
};

struct sentential_form {
    std::vector<symbol> str_;

    bool operator==(const sentential_form& other) const {
        return str_ == other.str_;
    }
    bool operator<(const sentential_form& other) const {
        return str_ < other.str_;
    }
};

struct CFL_rule {
    char left_part;
    sentential_form right_part;

    bool operator==(const CFL_rule& other) const {
        return left_part == other.left_part && right_part == other.right_part;
    }
};

struct Grammar {
  std::set<char> terminals;
  std::set<char> nonterminals;
  char start;
  std::vector<CFL_rule> rules;
};

struct First_k {
    std::set<std::vector<char>> set;
};

struct LR_k_situation {
  CFL_rule rule;
  size_t point;
  std::vector<char> u;

  bool operator==(const LR_k_situation& other) const {
    return rule == other.rule && point == other.point && u == other.u;
  }

  bool operator<(const LR_k_situation& other) const {

    if (rule.left_part < other.rule.left_part) {
      return true;
    } else if (rule.left_part > other.rule.left_part) {
      return false;
    }

    if (point < other.point) {
      return true;
    } else if (point > other.point) {
      return false;
    }

    if (rule.right_part.str_.size() < other.rule.right_part.str_.size()) {
      return true;
    } else if (rule.right_part.str_.size() > other.rule.right_part.str_.size()) {
      return false;
    }

    for (size_t i = 0; i < rule.right_part.str_.size(); ++i) {

      if (rule.right_part.str_[i].name_ < other.rule.right_part.str_[i].name_) {
        return true;
      } else if (rule.right_part.str_[i].name_ > other.rule.right_part.str_[i].name_) {
        return false;
      }

      if (rule.right_part.str_[i].type_ < other.rule.right_part.str_[i].type_) {
        return true;
      } else if (rule.right_part.str_[i].type_ > other.rule.right_part.str_[i].type_) {
        return false;
      }
    }

    if (u.size() < other.u.size()) {
      return true;
    } else if (u.size() > other.u.size()) {
      return false;
    }

    for (size_t i = 0; i < u.size(); ++i) {
      if (u[i] < other.u[i]) {
        return true;
      } else if (u[i] > other.u[i]) {
        return false;
      }
    }

    return false;
  }
};

struct V_k_G {
  std::set<LR_k_situation> set;
  bool mark = false;
  sentential_form active_prefix;
  
  bool operator<(const V_k_G& other) const {

        if (set < other.set) return true;
        if (other.set < set) return false;

        return mark < other.mark;
    }

  bool operator==(const V_k_G& other) const {
    return set == other.set && mark == other.mark;
  }
};

struct V_k_G_collection {
  std::set<V_k_G> collection;
};

struct hash_f {
  size_t operator()(const std::vector<char>& v) const {
    std::hash<char> charHash;
    size_t hash = 0;
    for (char c : v) {
      hash = hash * 31 + charHash(c);
    }
    return hash;
  }
};

struct hash_g {
  size_t operator()(char c) const {
    return static_cast<size_t>(c); 
  }
};

struct LR_k_table {
  std::unordered_map<std::vector<char>, int, hash_f> f;
  std::unordered_map<char, int, hash_g> g;
};

struct LR_configuration {
  std::vector<std::pair<symbol, int>> a;
  std::vector<char> x;
  std::vector<int> p;
};
