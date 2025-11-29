/** GRADER!
 * READ THIS
 *
 * Only things that are different between these two files are that instead of
 * AVL, it's hashmap. Everything else is kept the same, just with the names
 * changed to be appropriate.
 *
 * AVL turns into HashTable (this is the only difference between the two!)
 * AVLMap becomes HashMap (these implementations are exactly the same)
 * main functions and everything else is exactly the same
 *
 */

#include <array>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <string>

#define LENGTH 27
/**
 * use occurs(c) to update.
 * increment a counter for character c every time c occurs
 * implement as a vector of 27 counters
 *
 */
namespace m {

template <typename K, typename V> struct pair {
  K first;
  V second;

  bool operator<(const pair &other) const { return this->first < other.first; }
  bool operator>(const pair &other) const { return this->first > other.first; }
  bool operator==(const pair &other) const {
    return this->first == other.first;
  }
};

template <typename T> struct Node {
  T key;
  int height;
  Node *left;
  Node *right;

  Node(T k) {
    key = k;
    left = nullptr;
    right = nullptr;
    height = 1;
  }
  Node() {
    left = nullptr;
    right = nullptr;
    height = 1;
  }

  bool operator<(const Node &other) const { return this->key < other.key; };
  bool operator>(const Node &other) const { return this->key > other.key; };
  bool operator==(const Node &other) const { return this->key == other.key; };
};

template <typename T> class HashTable {
private:
  int capacity;
  int size;
  Node<T> **arr;
  int hash(T &val) {
    // we are just gonna hope and pray that val is a string
    // beacuse that's what this is for
    std::hash<std::string> st;
    return st(val.first) % capacity;
  }
  int resize() {}

public:
  HashTable(int cap = 1000) {
    this->capacity = cap;
    this->size = 0;
    this->arr = new Node<T> *[capacity];

    for (int i = 0; i < capacity; i++) {
      arr[i] = nullptr;
    }
  }

  ~HashTable() {
    for (int i = 0; i < capacity; i++) {
      Node<T> *entry = arr[i];
      while (entry != nullptr) {
        Node<T> *prev = entry;
        entry = entry->right;
        delete prev;
      }
    }
    delete[] arr;
  }

  int get_size() { return size; }
  T *find(T key) {
    int hashVal = hash(key);
    Node<T> *entry = arr[hashVal];

    while (entry != nullptr) {
      if (entry->key == key) {
        return &entry->key;
      }
      entry = entry->right;
    }
    // didn't exist
    return nullptr;
  }

  T *insert(T key) {
    int hashVal = hash(key);
    Node<T> *entry = arr[hashVal];
    while (entry != nullptr) {
      if (entry->key == key) {
        entry->key = key;
        return &entry->key;
      }
      entry = entry->right;
    }

    Node<T> *new_node = new Node<T>(key);
    new_node->right = arr[hashVal];

    arr[hashVal] = new_node;
    size++;

    return &new_node->key;
  }
  void remove(T key) {
    int hashVal = hash(key);
    Node<T> *prev = nullptr;
    Node<T> *head = arr[hashVal];

    while (head != nullptr) {
      if (head->key == key) {
        if (prev == nullptr) {
          arr[hashVal] = head->right;
        } else {
          prev->right = head->right;
        }
        delete head;
        size--;
        return;
      }
      prev = head;
      head = head->right;
    }

    // if we didn't return, then we didn't find anything to delete
    throw std::runtime_error("No deletion occured");
  }
};

template <typename K, typename V> class HashMap {
private:
  // match on pairs
  // imp stands for implementation, for lack of a better word
  HashTable<pair<K, V>> imp;

public:
  HashMap() {}

  int size() { return imp.get_size(); }
  bool empty() { return imp.get_size() < 0; }

  // returns a pointer to the pair between K and V
  // this hack belongs in AVLMap. 0 value is ignored during find.
  // use V types default initialization (brace init for safe initialization)
  pair<K, V> *find(K k) { return imp.find({k, V{}}); }
  pair<K, V> *insert(K k, V v) { return imp.insert({k, v}); }
  // hack
  void remove(K k) { imp.remove({k, V{}}); }
};

class CharDistribution {
private:
  // The problem here is 32 is the ASCII for Space, so we cannot just subtract a
  // 97. We need to account for the space somehow
  std::array<double, LENGTH> occurences{};

public:
  CharDistribution() {}
  CharDistribution(std::string text) {
    for (char x : text) {

      // Subtract x - 96, the ascii for a - 1 (shift all the ascii characters
      // over by one, so a starts at 1 space is at 0). The ascii for space is 32
      // 32 - 97 is -65 map spaces to 0, else, map normally if it's a negative
      // number, then it's a space

      int y = (x - 96) < 0 ? 0 : x - 96;
      ++occurences[y];
    }
  }

  void addLetter(char letter) {
    int y = (letter - 96) < 0 ? 0 : letter - 96;
    ++occurences[y];
  }

  char getRandom() {
    double sum = 0;
    for (const auto &c : occurences) {
      sum += c;
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, sum);

    int num = dist(gen);

    for (int i = 0; i < occurences.size(); i++) {
      if (occurences[i] == 0)
        continue;
      // for each number of occurences
      num -= occurences[i];
      if (num <= 0) {
        if (i == 0)
          return ' ';
        return (char)(i + 96);
      }
    }
    // this will never happen, theoretically
    return '-';
  }

  /**
   * Getting the percentage occurences of each character
   */
  // NOTE: I DONT EVEN USE THIS
  // i spent time doing this so i kept it in :(
  std::array<double, LENGTH>
  normalprobdist(const std::array<double, LENGTH> arr) {
    std::array<double, LENGTH> res;

    double sum = 0;
    for (const auto &x : arr) {
      sum += x;
    }
    for (int i = 0; i < arr.size(); i++) {
      res[i] = arr[i] / sum;
    }
    return res;
  }

  std::array<double, LENGTH> getOccurences() { return occurences; }
};

} // namespace m

m::HashMap<std::string, m::CharDistribution> *read_input(std::ifstream &in,
                                                         int window_size) {
  m::HashMap<std::string, m::CharDistribution> *map =
      new m::HashMap<std::string, m::CharDistribution>();

  std::string str;
  getline(in, str);

  for (int i = window_size; i < str.length(); i += 1) {
    // go from i - window_size to i, and add the subsequent character to our
    // entry
    // create the string, and the char distribution
    auto f = map->find(str.substr(i - window_size, window_size));

    if (f == nullptr) {
      m::CharDistribution t;
      t.addLetter(str[i]);
      map->insert(str.substr(i - window_size, window_size), t);

    } else {
      f->second.addLetter(str[i]);
    }
  }
  return map;
}

void preprocess_input(std::ifstream &in) {
  std::ofstream out;
  out.open("preprocessed");

  std::string line;
  while (getline(in, line)) {
    // replace all new line with space
    out << line << ' ';
  }
}

std::string generate_output(std::ifstream &in,
                            m::HashMap<std::string, m::CharDistribution> *map,
                            int window_size, int output_size) {
  in.clear();
  in.seekg(0);

  std::string line;
  getline(in, line);
  std::string starting_substr = line.substr(0, window_size);

  // this is very inefficient, but no premade data structures so
  // no stringstream :(
  std::string ret = starting_substr;

  while (ret.size() <= output_size) {
    auto p = map->find(ret.substr(ret.size() - window_size, window_size));
    if (!p) {
      std::cerr << "EARLY EXIT, NO SUBSTR FOUND HERE" << std::endl;
      return ret;
    } else {
      ret += p->second.getRandom();
    }
  }
  return ret;
}

int main() {

  std::ifstream input;
  input.open("merchant.txt");

  if (input.is_open()) {
    preprocess_input(input);
    input.close();
  }
  input.open("preprocessed");

  std::cout << "Welcome to Anish's bootleg RNN!" << std::endl;
  std::cout << "Please enter a window size: " << std::endl;

  int window_size;
  std::cin >> window_size;

  std::cout << "Great! Now enter an output size for your novel: " << std::endl;

  int output_size;
  std::cin >> output_size;

  // this returns an AVLMap
  const auto ret = read_input(input, window_size);
  const std::string out = generate_output(input, ret, window_size, output_size);

  std::cout << out << std::endl;

  input.close();
  return 0;
}
