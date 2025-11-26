#include <array>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <stdexcept>

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
  void insert(K k, V v) { imp.insert({k, v}); }
  // hack
  void remove(K k) { imp.remove({k, V{}}); }
};

class CharDistribution {
private:
  // The problem here is 32 is the ASCII for Space, so we cannot just subtract a
  // 97. We need to account for the space somehow
  std::array<double, LENGTH> occurences{};
  int window;

public:
  CharDistribution(std::string text, int window) : window(window) {
    for (char x : text) {

      // Subtract x - 96, the ascii for a - 1 (shift all the ascii characters
      // over by one, so a starts at 1 space is at 0). The ascii for space is 32
      // 32 - 97 is -65 map spaces to 0, else, map normally if it's a negative
      // number, then it's a space

      int y = (x - 96) < 0 ? 0 : x - 96;
      ++occurences[y];
    }
  }

  /**
   * Getting the percentage occurences of each character
   */
  std::array<double, LENGTH>
  normalprobdist(const std::array<double, LENGTH> arr) {
    std::array<double, LENGTH> res;

    double sum = std::reduce(arr.begin(), arr.end());
    for (int i = 0; i < arr.size(); i++) {
      res[i] = arr[i] / sum;
    }
    return res;
  }

  std::array<double, LENGTH> getOccurences() { return occurences; }
};

} // namespace m

int main() {

  // m::CharDistribution x(
  //     "lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
  //     eiusmod " "tempor incididunt ut labore et dolore magna aliqua. Ut enim
  //     ad minim " "veniam, quis nostrud exercitation ullamco laboris nisi ut
  //     aliquip ex ea " "commodo consequat. Duis aute irure dolor in
  //     reprehenderit in voluptate " "velit esse cillum dolore eu fugiat nulla
  //     pariatur. Excepteur sint " "occaecat cupidatat non proident, sunt in
  //     culpa qui officia deserunt " "mollit anim id est laborum", 12);
  // auto arr = x.getOccurences();
  // auto res = x.normalprobdist(arr);
  //
  // for (auto x : res) {
  //   std::cout << x << std::endl;
  // }
  //
  // auto sum = std::reduce(res.begin(), res.end());
  // std::cout << "accum: " << sum << std::endl;

  m::AVLMap<int, int> map;
  map.insert(5, 3);
  map.insert(2, 3);
  map.insert(1, 3);
  map.insert(3, 3);
  map.insert(4, 3);
  map.insert(5, 4);
  map.insert(7, 3);

  auto x = map.find(5);

  std::cout << x->second << std::endl;

  map.remove(20);

  return 0;
}
