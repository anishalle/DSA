package org.example;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

class Node {
    int ridx, cidx, val;
    Node next, down;

    Node(int r, int c, int val){
        this.ridx = r;
        this.cidx = c;
        this.val = val;
        this.next = null;
        this.down = null;
    }
    Node(int r, int c,int val, Node next, Node down){
        this.ridx = r;
        this.cidx = c;
        this.val = val;
        this.next = next;
        this.down = down;
    }


    public String toString(){
        return String.valueOf(this.val);
    }
}

class SparseMatrix {

    //row length and col length
    int rn;
    int cn;
    Node[] rows;
    Node[] cols;

    Node head;

    public SparseMatrix(int n){
        rn = cn = n + 1;
        rows = new Node[rn];
        cols = new Node[cn];
        head = new Node(0,0,0);

        rows[0] = cols[0] = head;

        //build the rows
        Node prevRow = head;

        for (int i = 1; i <= n; i++) {
            Node rhead = new Node(i, 0, 0);
            rhead.next = rhead; // empty row ring, because rows point to themselves for now
            rhead.down = null; // will be linked later
            rows[i] = rhead;
            prevRow.down = rhead;
            prevRow = rhead;
        }

        // close the row chain
        prevRow.down = head;
        head.down = rows[1];


        //build cols
        Node prevCol = head;
        for (int j = 1; j <= n; j++) {
            Node chead = new Node(0, j, 0);
            chead.down = chead; // empty column ring
            chead.next = null; // will be linked below
            cols[j] = chead;

            prevCol.next = chead;
            prevCol = chead;
        }
        prevCol.next = head;
        head.next = cols[1];
    }

    //we assume actual content Nodes are ONE indexed
    //Why? because I said so
    //and also it's easier to have the entire array be zero indexed.
    public void addNode(Node node){
        int r = node.ridx, c = node.cidx, v = node.val;

        // —— ROW SEARCH (sorted by column via `next`) ——
        Node rHead = rows[r];
        Node left = rHead;
        Node cur = rHead.next;

        // find the column where we should insert
        while (cur != rHead && cur.cidx < c) {
            left = cur; cur = cur.next;
        }

        //if cur is not the head, and the column index is correct
        if (cur != rHead && cur.cidx == c) {
            // (r,c) already present
            if (v == 0) {
                //we don't like zeroes
                // unlink from row
                left.next = cur.next;
                // unlink from column
                unlinkFromColumn(cur);
                cur.next = cur.down = null;
            } else {
                cur.val = v;
            }
            return;
        }


        if (v == 0) return; // inserting zero = BIGGG nono


        //insert horizontally
        node.next = cur; // cur is the first node with col >= c or row sentinel
        left.next = node;


        //insert vertically
        Node cHead = cols[c];
        Node up = cHead;
        Node dn = cHead.down; // first data node in the column (or self if empty)
        while (dn != cHead && dn.ridx < r) { up = dn; dn = dn.down; }
        node.down = dn;
        up.down = node;
    }


    /**
     * Get the value at r, c
     * @param r the row
     * @param c the col
     * @return the value of row col
     */
    public int get(int r, int c) {
        Node rHead = rows[r];
        for (Node x = rHead.next; x != rHead; x = x.next) {
            if (x.cidx == c) return x.val;
            if (x.cidx > c) break;
        }
        return 0;
    }


    /** Remove entry at (r,c)
     *
     * @param r the row to remove
     * @param c the col to remove
     */
    public void remove(int r, int c) {
        Node rHead = rows[r];
        Node left = rHead;
        Node cur = rHead.next;

        while (cur != rHead && cur.cidx < c) {
            left = cur; cur = cur.next;
        }
        if (cur != rHead && cur.cidx == c) {
            left.next = cur.next; // unlink horizontally
            unlinkFromColumn(cur); // unlink vertically
            cur.next = cur.down = null;
        }
    }

    ////////////////
    //Calculations
    /// ////////////

    /**
     *
     * @param B the OTHER matrix
     * @return A sparsematrix that is the addition of both
     */
    public SparseMatrix add(SparseMatrix B) {
        int n = rn - 1;
        SparseMatrix C = new SparseMatrix(n);

        //copy our matrix from matrix A for addition.
        for (int r = 1; r <= n; r++) {
            //not ahead, but A Head.
            Node aHead = rows[r];
            for (Node a = aHead.next; a != aHead; a = a.next) {
                C.addNode(new Node(r, a.cidx, a.val));
            }
        }


        for (int r = 1; r <= n; r++) {
            Node bHead = B.rows[r];
            for (Node b = bHead.next; b != bHead; b = b.next) {
                int sum = C.get(r, b.cidx) + b.val;
                if (sum == 0) {
                    C.remove(r, b.cidx);
                } else {
                    //add our new song
                    C.addNode(new Node(r, b.cidx, sum));
                }
            }
        }
        return C;
    }

    public SparseMatrix scale(int k) {
        int n = rn - 1;
        SparseMatrix C = new SparseMatrix(n);

        if (k == 0)
            return C; // all zeros, so empty array

        for (int r = 1; r <= n; r++) {
            Node aHead = rows[r];
            for (Node a = aHead.next; a != aHead; a = a.next) {
                int v = a.val * k;

                if (v != 0)
                    //we at anish.inc hate zeroes
                    C.addNode(new Node(r, a.cidx, v));
            }
        }
        return C;
    }

    /** Transpose!
     *  r, c = c, r
     * @return return a transposed sparse matrix
     */
    public SparseMatrix transpose() {
        int n = rn - 1;
        SparseMatrix C = new SparseMatrix(n);

        for (int r = 1; r <= n; r++) {
            Node aHead = rows[r];
            for (Node a = aHead.next; a != aHead; a = a.next) {
                //flip the cidx and ridx
                C.addNode(new Node(a.cidx, r, a.val));
            }
        }
        return C;
    }

    /** Multiply two matrices.
     *
     * @param B The matrix to multply with
     * @return The sparsematrix returned
     */

    // C[i,j] = sum_{k=1..n} A[i,k] * B[k,j]
    public SparseMatrix multiply(SparseMatrix B) {
        //this is taking forever, my head hurts
        int n = rn - 1;
        SparseMatrix C = new SparseMatrix(n);


        for (int i = 1; i <= n; i++) {
            Node aRowHead = rows[i];
            if (aRowHead.next == aRowHead) continue; // empty row in A


            for (int j = 1; j <= n; j++) {
                Node bColHead = B.cols[j];
                if (bColHead.down == bColHead) continue; // empty column in B

                //Intellij is so good. auto extracted my method for me!
                int sum = getSum(aRowHead, bColHead);


                if (sum != 0) {
                    C.addNode(new Node(i, j, sum)); // keep sparse (skip zeros)
                }
            }
        }
        return C;
    }

    private static int getSum(Node aRowHead, Node bColHead) {
        int sum = 0;

        // Go across A's i-th row (by columns k)
        for (Node a = aRowHead.next; a != aRowHead; a = a.next) {
            int k = a.cidx; // column index in A equals row index in B's column

            // Find B[k,j] by going down column j to row k
            Node b = bColHead.down;
            while (b != bColHead && b.ridx < k) {
                b = b.down;
            }
            if (b != bColHead && b.ridx == k) {
                sum += a.val * b.val;
            }
        }
        return sum;
    }


    //private helpers
    private void unlinkFromColumn(Node target) {
        Node cHead = cols[target.cidx];
        Node up = cHead;
        Node dn = cHead.down;
        while (dn != cHead && dn != target) { up = dn; dn = dn.down; }
        if (dn == target) up.down = dn.down; // relink if found
    }



}

class CSVReader {
    private Scanner sc;
    final String delimiter = ",";
    File csv;

    public CSVReader(File file) throws FileNotFoundException {
        this.csv = file;
        this.sc = new Scanner(file);
    }


    public void readCSV(File outFile){
        //the header, A + n or M + n etc
        String[] header = sc.nextLine().split(delimiter);
        char op = header[0].trim().toUpperCase().charAt(0);
        int n = Integer.parseInt(header[1].trim());

        //skip the delim only line
        if (sc.hasNextLine()) sc.nextLine();

        //parse the right block
        SparseMatrix right = new SparseMatrix(n);
        while (sc.hasNextLine()) {
            String line = sc.nextLine();
            if (isDelimiterOnly(line)) break; // end of RIGHT block
            if (line.trim().isEmpty()) continue;
            String[] t = line.split(delimiter);
            int r = Integer.parseInt(t[0].trim());
            int c = Integer.parseInt(t[1].trim());
            int v = Integer.parseInt(t[2].trim());
            if (v == 0) continue;
            int cur = right.get(r, c);
            int nv = cur + v; // merge duplicates if any
            if (nv == 0) right.remove(r, c); else right.addNode(new Node(r, c, nv));
        }

        //Delim only line, already consumed by logic above

        SparseMatrix output;
        if (op == 'S') {
            //if Left is a scaler, we dont need to do the entire op
            String line = sc.hasNextLine() ? sc.nextLine() : "0";
            String[] t = line.split(delimiter);
            int k = Integer.parseInt(t[0].trim());
            output = right.scale(k);
        } else {
            //LEFT matrix logic
            SparseMatrix left = new SparseMatrix(n);
            while (sc.hasNextLine()) {
                String line = sc.nextLine();
                if (line.trim().isEmpty() || isDelimiterOnly(line)) break; // end/EOF
                String[] t = line.split(delimiter);
                int r = Integer.parseInt(t[0].trim());
                int c = Integer.parseInt(t[1].trim());
                int v = Integer.parseInt(t[2].trim());
                if (v == 0) continue;
                int cur = left.get(r, c);
                int nv = cur + v;
                if (nv == 0) left.remove(r, c); else left.addNode(new Node(r, c, nv));
            }
            switch (op) {
                case 'A': output = left.add(right); break;
                case 'M': output = right.multiply(left); break;
                case 'T': output = right.transpose(); break;
                default:  output = new SparseMatrix(n); // should not happen
            }
        }

        //print the answer
        System.out.println(op + "," + n);
        for (int r = 1; r <= n; r++) {
            Node h = output.rows[r];
            for (Node x = h.next; x != h; x = x.next) {
                System.out.println(r + "," + x.cidx + "," + x.val);
            }
        }

        //write to csv
        try (java.io.PrintWriter pw = new java.io.PrintWriter(new java.io.FileWriter(outFile))) {
            for (int r = 1; r <= n; r++) {
                Node h = output.rows[r];
                for (Node x = h.next; x != h; x = x.next) {
                    pw.println(r + "," + x.cidx + "," + x.val);
                }
            }
            pw.flush();
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }

    }


    /**
     * Check if line is only a delimiter
     * @param line is a line
     * @return returns true if only dlimiters and false if not
     */
    private boolean isDelimiterOnly(String line) {
        if (line == null) return false;
        for (int i = 0; i < line.length(); i++) {
            char ch = line.charAt(i);
            if (ch != ',' && !Character.isWhitespace(ch)) return false;
        }
        return true;
    }

}




public class Main {
   public static void main(String[] args) throws FileNotFoundException {
        File in = new File(args[0]);
        String name = in.getName();
        String outName = name + "_output.csv";

        File parent = in.getParentFile();
        File out = new File(parent == null ? new File(".") : parent, outName);

        CSVReader reader = new CSVReader(in);
        reader.readCSV(out);
    }

}

