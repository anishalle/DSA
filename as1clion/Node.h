#pragma once // Node.h

class Node {
public:
  int rowidx, colidx, val;
  Node *rowptr, *colptr;

  Node();
  Node(int row, int col, int value)
      : rowidx(row), colidx(col),val(value), rowptr(nullptr), colptr(nullptr) {}
  Node(int row, int col, int value, Node *rp, Node *colp)
      : rowidx(row), colidx(col), val(value), rowptr(rp), colptr(colp) {}
};
