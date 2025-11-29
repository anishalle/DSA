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
#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
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

  bool operator<(const Node &other) const { return this->key < other.key; };
  bool operator>(const Node &other) const { return this->key > other.key; };
  bool operator==(const Node &other) const { return this->key == other.key; };
};

template <typename T> class AVL {
private:
  Node<T> *root;
  int size;

  Node<T> *minValueNode(Node<T> *node) {
    Node<T> *current = node;
    while (current->left != nullptr) {
      current = current->left;
    }
    return current;
  }

  int getHeight(Node<T> *n) {
    if (n == nullptr) {
      return 0;
    }
    return n->height;
  }
  int getBalance(Node<T> *N) {
    if (N == nullptr)
      return 0;
    return getHeight(N->left) - getHeight(N->right);
  }

  Node<T> *rightRotate(Node<T> *y) {
    Node<T> *x = y->left;
    Node<T> *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));
    x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));

    return x;
  }

  Node<T> *leftRotate(Node<T> *x) {
    Node<T> *y = x->right;
    Node<T> *T2 = y->left;

    // perform rotation
    y->left = x;
    x->right = T2;

    // update heights
    x->height = 1 + std::max(getHeight(x->left), getHeight(x->right));
    y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));

    // return new root
    return y;
  }

  // reference return the inserted item because its a recursive call
  Node<T> *insert_at(Node<T> *node, T key, T *&inserted_item) {
    if (node == nullptr) {
      Node<T> *new_node = new Node(key);
      inserted_item = &new_node->key;
      return new_node;
    }

    if (key < node->key) {
      node->left = insert_at(node->left, key, inserted_item);
    } else if (key > node->key) {
      node->right = insert_at(node->right, key, inserted_item);
    } else {
      // upon duplicate, change key to latest key
      node->key = key;
      inserted_item = &node->key;
      return node;
    }

    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    // If this node becomes unbalanced,
    // then there are 4 cases

    // Left Left Case
    if (balance > 1 && key < node->left->key)
      return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
      return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->key) {
      node->left = leftRotate(node->left);
      return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key) {
      node->right = rightRotate(node->right);
      return leftRotate(node);
    }

    // Return the (unchanged) node pointer
    return node;
  }
  Node<T> *delete_at(Node<T> *root, T key, bool &deleted) {
    if (root == nullptr)
      return root;

    // If the key to be deleted is smaller
    // than the root's key, then it lies in
    // left subtree
    if (key < root->key)
      root->left = delete_at(root->left, key, deleted);

    // If the key to be deleted is greater
    // than the root's key, then it lies in
    // right subtree
    else if (key > root->key)
      root->right = delete_at(root->right, key, deleted);

    // if key is same as root's key, then
    // this is the node to be deleted
    else {
      deleted = true;
      if ((root->left == nullptr) || (root->right == nullptr)) {
        Node<T> *temp = root->left ? root->left : root->right;

        if (temp == nullptr) {
          temp = root;
          root = nullptr;
        } else
          *root = *temp;

        delete temp;
      } else {
        Node<T> *temp = minValueNode(root->right);

        root->key = temp->key;

        root->right = delete_at(root->right, temp->key, deleted);
      }
    }
    // past all the recursions.
    if (!deleted) {
      throw std::runtime_error("No deletion occured");
    }

    if (root == nullptr)
      return root;

    root->height = 1 + std::max(getHeight(root->left), getHeight(root->right));

    int balance = getBalance(root);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
      return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) {
      root->left = leftRotate(root->left);
      return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
      return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) {
      root->right = rightRotate(root->right);
      return leftRotate(root);
    }

    return root;
  }
  T *find_at(Node<T> *root, T key) {
    // nullptr
    if (!root) {
      return nullptr;
    }

    Node<T> *iter = root;
    while (iter != nullptr) {
      if (iter->key == key) {
        // get the address of the key value, not the actual value
        // because we need to return a reference to the obj instead...
        return &iter->key;
      }
      if (iter->key < key) {
        iter = iter->right;
      } else {
        iter = iter->left;
      }
    }
    // not found
    return nullptr;
  }

public:
  AVL() {
    root = nullptr;
    size = 0;
  }

  int get_size() { return size; }
  T *insert(T val) {
    T *inserted_at = nullptr;
    root = insert_at(root, val, inserted_at);
    return inserted_at;
  }
  void remove(T val) {
    bool deleted = false;
    root = delete_at(root, val, deleted);
  }
  T *find(T val) { return find_at(root, val); }
};

template <typename K, typename V> class AVLMap {
private:
  // match on pairs
  // imp stands for implementation, for lack of a better word
  AVL<pair<K, V>> imp;

public:
  AVLMap() {}

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

m::AVLMap<std::string, m::CharDistribution> *read_input(std::ifstream &in,
                                                        int window_size) {
  m::AVLMap<std::string, m::CharDistribution> *map =
      new m::AVLMap<std::string, m::CharDistribution>();

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
                            m::AVLMap<std::string, m::CharDistribution> *map,
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
