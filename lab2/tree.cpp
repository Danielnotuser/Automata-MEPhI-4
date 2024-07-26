#include <iostream>
#include <vector>
#include <map>
#include <format>

#include "tree.h"


void STree::synt(std::string &pat_inp)
{
    std::string spec = "()+-|";
    for (int i = 0; i < pat_inp.size(); i++) if (spec.find(pat_inp[i]) == spec.npos && (pat_inp[i] < 'a' || pat_inp[i] > 'z')) throw std::invalid_argument("Symbol is not from the alphabet!");
    std::string pat = "(" + pat_inp + ")";
    std::vector<Node*> nodes(pat.size());
    for (int i = 0; i < nodes.size(); i++) nodes[i] = new Node;
    int fir, sec;
    while ((sec = pat.find(")")) != pat.npos)
    {
        fir = pat.rfind("(", sec);
        if (fir == pat.npos) throw std::invalid_argument("One or more parentheses are not paired!");
        int i = fir + 1;
        while (i < sec)
        {
            if (pat[i] >= 'a' && pat[i] <= 'z' || pat[i - 1] == '&')
            {
                nodes[i]->info = pat[i];
                if (pat[i - 1] == '&')
                {
                    delete nodes[i - 1];
                    nodes.erase(nodes.begin() + i - 1);
                    pat.erase(pat.begin() + i - 1);
                    i--;
                    sec--;
                }
            }
            i++;

        }
        i = fir + 1;
        while (i < sec)
        {
            if ((pat[i] == '+' || pat[i] == '?') && !nodes[i]->info)
            {
                nodes[i]->info = pat[i];
                nodes[i]->left = nodes[i - 1];
                nodes.erase(nodes.begin() + i - 1);
                pat.erase(pat.begin() + i - 1);
                sec--;
                i--;
            }
            i++;
        }
        i = fir + 1;
        while (i < sec)
        {
            if (pat[i] == '-' && !nodes[i]->info)
            {
                nodes[i]->info = pat[i];
                nodes[i]->left = nodes[i - 1];
                nodes[i]->right = nodes[i + 1];
                nodes.erase(nodes.begin() + i - 1);
                nodes.erase(nodes.begin() + i);
                pat.erase(pat.begin() + i - 1);
                pat.erase(pat.begin() + i);
                sec -= 2;
                i -= 2;
            }
            i++;
        }
        i = fir + 1;
        while (i < sec)
        {
            if (pat[i] == '|' && !nodes[i]->info)
            {
                nodes[i]->info = pat[i];
                nodes[i]->left = nodes[i - 1];
                nodes[i]->right = nodes[i + 1];
                nodes.erase(nodes.begin() + i - 1);
                nodes.erase(nodes.begin() + i);
                pat.erase(pat.begin() + i - 1);
                pat.erase(pat.begin() + i);
                sec -= 2;
                i -= 2;
            }
            i++;
        }
        delete nodes[fir];
        delete nodes[sec];
        nodes.erase(nodes.begin() + fir);
        nodes.erase(nodes.begin() + sec - 1);
        pat.erase(pat.begin() + fir);
        pat.erase(pat.begin() + sec - 1);

    }
    if (nodes.size() != 1) throw std::invalid_argument("One or more parentheses are not paired!");
    root = nodes[0];
    nodes.clear();
    pat.clear();
}


void recur(Node *rt)
{
    std::cout << rt->info << " ";
    if (rt->left) recur(rt->left);

    if (rt->right) recur(rt->right);
}

void STree::lcp()
{
    Node *rt = root;
    recur(rt);
}

void del(Node *rt)
{
    if (rt->left) del(rt->left);
    if (rt->right) del (rt->right);
    delete rt;
}

STree::~STree()
{
    del(root);
}
