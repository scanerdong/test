#include "UnlockQueue.h"
#include <iostream>
#include <algorithm>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include "DFilter.h"
#include <boost/bind.hpp>

using namespace std;


class Call
{
public:
    void operator()(int i)
    {
        cout << "Call product id = " << i << endl;
    }
};

void call(int i)
{
    cout << "call product id = " << i << endl;
}

template <class C>
class Product
{
public:
    void operator()(C c, int i)
    {
        c(i);
    }
};

struct Node
{
  Node(){l=r=NULL;}
  Node(int value):v(value),l(NULL),r(NULL){}
  int v;
  Node* l;
  Node* r;
};

class BTree
{
  public:
    BTree();
    ~BTree();

    int insert(int v);
    void print_tree(Node* node);
    
    void print_tree(Node* node, int layer);
    int del(int v);
    
    Node* tree;
};

BTree::BTree(){
    tree = NULL;
}

BTree::~BTree(){
}

int BTree::insert(int v)
{
    Node* pNewNode = new Node(v);
    
    if(tree == NULL)
    {
        tree = pNewNode;
        return 0;
    }

    Node* node = tree;
    Node* parent = NULL;
    while(node)
    {
        if(node->v > v)
        {
            parent = node;
            node = node->l;
        }else if(node->v < v)
        {
            parent = node;
            node = node->r;
        }else{
            return -1;
        }
    }

    if(parent->v > v) parent->l = pNewNode;
    else parent->r = pNewNode;
    return 0;
}


void BTree::print_tree(Node* node)
{
    if(node->l)
    {
        print_tree(node->l);
    }

    cout << node->v << " ";
    
    if(node->r)
    {
        print_tree(node->r);
    }
}

void BTree::print_tree(Node* node, int layer)
{
    if(!node) return;
    
    print_tree(node->l, layer+1);

    for(int i=0;i<layer;i++)
       printf(" ");
    cout << node->v << endl;
    
    print_tree(node->r, layer+1);
}

int BTree::del(int v)
{
    Node* waitdel, *waitdelparent = NULL, *node;
    waitdel = tree;

    while(waitdel)
    {
        if(waitdel->v > v)
        {
            waitdelparent = waitdel;
            waitdel = waitdel->l;
        }else if(waitdel->v < v)
        {
            waitdelparent = waitdel;
            waitdel = waitdel->r;
        }else{
            break;
        }
    }
    if(waitdel == NULL) return -1;

    
    if(waitdel->l != NULL && waitdel->r != NULL){
        node = waitdel->l;
        while(node->r)
        {
            node = node->r;
        }

        node->r = waitdel->r;
    }else if(waitdel->r != NULL){
        waitdel->l = waitdel->r;
    }

    //waitdel is root node
    if(waitdelparent == NULL)
    {
        tree = waitdel->l;
    }else{
        if(waitdelparent->l == waitdel){
            waitdelparent->l = waitdel->l;
        }else if(waitdelparent->r == waitdel){
            waitdelparent->r = waitdel->l;
        }
    }

    
    delete waitdel;
    return 0;
}

int main(int argc, char** argv)
{
    int ai[] = {5,7,8,9,4,2,1,3,222,888,777,666,223};
    BTree btree;
    for(int i = 0; i < sizeof(ai)/4; i++){
        btree.insert(ai[i]);
    }

    btree.print_tree(btree.tree, 0);
    printf("\n");
    btree.del(5);
    btree.print_tree(btree.tree);
    printf("\n");

    btree.del(777);
    btree.print_tree(btree.tree);
    printf("\n");

    btree.del(888);
    btree.print_tree(btree.tree);
    printf("\n");
#if 0
//函数对象
Call c;
Product<Call> pdt;

pdt(c, 15);


//C风格函数
typedef void (*F)(int);

F f = call;

Product<F > pdt2;

pdt2(f,16);

cout << sizeof(c) << endl;
#endif


return 0;
}

