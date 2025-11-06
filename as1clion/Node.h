#pragma once // Node.h

class Node {
public:
  int rowidx, colidx;
  Node *rowptr, *colptr;

  Node();
  Node(int row, int col)
      : rowidx(row), colidx(col), rowptr(nullptr), colptr(nullptr) {}
  Node(int row, int col, Node *rp, Node *colp)
      : rowidx(row), colidx(col), rowptr(rp), colptr(colp) {}
};
