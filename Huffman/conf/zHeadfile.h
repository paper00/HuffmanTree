#ifndef HUFFMAN_H_INCLUDED
#define HUFFMAN_H_INCLUDED
#include <bits/stdc++.h>
#define MAXCOUNT 100
#define NUM 256
#define BIT 8
using namespace std;

template <typename T>
struct Node {
    T weight;
    Node<T>* lchild, *rchild;
    bool operator > (const Node<T>& node) const {
        return weight > node.weight;
    }
};

struct _char {
    unsigned char _ch;
    int _count;
    string _code;
    bool operator > (const _char& c) const {
        return _count > c._count;
    }
};

template <typename T>
struct _greater {
    bool operator () (Node<T> const *x, Node<T> const *y) {
        return x->weight > y->weight;
    }
};

template <typename T>
class HuffmanTree {
public:
    HuffmanTree() {
        root = new Node<T>;
    }
    ~HuffmanTree() {
        //destroyTree(root);
    }
    bool create(const T* ch) {
        priority_queue<Node<T>*, vector<Node<T>* >, _greater<T> > q;
        // Create single tree
        for (int i=0; i<NUM; i++) {
            if (!ch[i]._count) continue;
            Node<T>* temp = new Node<T>;
            temp->weight = ch[i];
            temp->lchild = temp->rchild = NULL;
            q.push(temp);
        }
        // Create Huffman tree
        while(!q.empty()) {
            Node<T>* l = new Node<T>;
            l = q.top();
            q.pop();
            if (q.empty()) {
                root = l; break;
            }
            Node<T>* r = new Node<T>;
            r = q.top();
            q.pop();
            Node<T>* p = new Node<T>;
            p->lchild = l;
            p->rchild = r;
            p->weight._count = l->weight._count + r->weight._count;
            q.push(p);
        }
        return true;
    }
    Node<T>* getRoot() const {
        return root;
    }
    void destroyTree(Node<T>* root) {
        if (!root) return;
        destroyTree(root->lchild);
        destroyTree(root->rchild);
        delete root;
    }
private:
    Node<T>* root;
};

class zip {
public:
    zip () {
        for (int i=0; i<NUM; i++) {
            ch[i]._ch = i;
            ch[i]._count = 0;
        }
    }
    void huffmanCoding(Node<_char>* root, string& code) {
        if (!root) return;
        string temp1 = code + '0';
        string temp2 = code + '1';
        huffmanCoding(root->lchild, temp1);
        huffmanCoding(root->rchild, temp2);
        root->weight._code = code;
        // At last
        if (!root->lchild && !root->rchild)
            ch[root->weight._ch]._code = code;
    }
    bool writeConf(string fileName) {
        fstream output(fileName.c_str(), ios::out | ios::binary);
        for (int i=0; i<NUM; i++) {
            if (!ch[i]._count) continue;
            output.put(ch[i]._ch);
            output.write(reinterpret_cast<char*>(&ch[i]._count), sizeof(int));
            output.write(reinterpret_cast<char*>(&ch[i]._code), sizeof(string));
        }
        output.close();
        return true;
    }
    bool readConf(string fileName) {
        ifstream input(fileName.c_str(), ios::binary);
        if (input.fail()) {
            cerr << "open error!" << endl; exit(1);
        }
        unsigned char c;
        while (!input.eof()) {
            c = input.get();
            if (input.eof()) break;
            ch[c]._ch = c;
            input.read(reinterpret_cast<char*>(&ch[c]._count), sizeof(int));
            input.read(reinterpret_cast<char*>(&ch[c]._code), sizeof(string));
        }
    }
    bool compress() {
        cout << "Enter the file name to compress: ";
        string fileName, s;
        getline(cin, fileName);
        ifstream input(fileName.c_str(), ios::binary);
        if (input.fail()) {
            cerr << "open error!" << endl; exit(1);
        }
        // Count the times of each characters appearing
        unsigned char c;
        while (!input.eof()) {
            c = input.get();
            if (input.eof()) break;
            ch[c]._count++;
        }
        HuffmanTree<_char> hf;
        hf.create(ch);
        huffmanCoding(hf.getRoot(), s);
        input.close();
        input.open(fileName.c_str(), ios::binary);
        // Create a new compressed file
        string fileName1 = fileName + ".compress";
        ofstream output(fileName1.c_str(), ios::binary);
        unsigned char new_ch = 0;
        int bits = 0;
        while (!input.eof()) {
            c = input.get();
            if (input.eof()) break;
            string code = ch[c]._code;
            for (int i=0; i<code.size(); i++) {
                new_ch <<= 1;
                if (code[i]=='1')
                    new_ch |= 1;
                if ((++bits) == BIT) {
                    output.put(new_ch);
                    new_ch = 0;
                    bits = 0;
                }
            }
        }
        // If less than 8 bits at last, add 0 in the back
        if (new_ch) {
            new_ch <<= BIT - bits;
            output.put(new_ch);
        }
        // Write the configuration file
        string fileName2 = fileName + ".conf";
        writeConf(fileName2);
        output.close();
        input.close();
        cout << "Compress Succeed" << endl;
        return true;
    }
    bool decompress() {
        cout << "Enter the file name to decompress: ";
        string fileName, newFileName;
        getline(cin, fileName);
        cout << "Enter the new file name: ";
        getline(cin, newFileName);
        string fileName1 = fileName + ".conf";
        readConf(fileName1);
        HuffmanTree<_char> ht;
        ht.create(ch);
        string fileName2 = fileName + ".compress";
        ifstream input(fileName2.c_str(), ios::binary);
        ofstream output(newFileName.c_str(), ios::binary);
        char temp1[BIT];
        Node<_char>* cur = ht.getRoot();
        while (!input.eof()) {
            unsigned char c = input.get();
            if (input.eof()) break;
            itoa((int)c, temp1, 2);
            string temp2(temp1);
            while (temp2.size()!=BIT)
                temp2 = '0' + temp2;
            for (int i=0; i<BIT; i++) {
                if (cur->lchild==NULL && cur->rchild==NULL) {
                    output.put(cur->weight._ch);
                    cur = ht.getRoot();
                }
                if (temp2[i] == '0')
                    cur = cur->lchild;
                else
                    cur = cur->rchild;
            }
        }
        output.close();
        input.close();
        cout << "Decompress Succeed" << endl;
        return true;
    }
public:
    _char ch[NUM];
};

#endif // HUFFMAN_H_INCLUDED
