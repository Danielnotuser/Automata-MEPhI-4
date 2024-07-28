#include <iostream>
#include <vector>
#include <format>
#include <sstream>

#include "tree.h"

void STree::synt(std::string &pat_inp)
{
    std::string spec = "<>{,}()+-|";
    std::string pat = "(" + pat_inp + ")";
    std::vector<Node*> nodes(pat.size());
    for (auto & node : nodes) node = new Node;
    size_t fir, sec;
    while ((sec = pat.find(")")) != pat.npos)
    {
        fir = pat.rfind("(", sec);
        if (fir == pat.npos) throw std::invalid_argument("One or more parentheses are not paired!");
        std::string group_name;
        size_t i = fir + 1;
        while (i < sec)
        {
            if (pat[i] == '<' && !nodes[i]->info)
            {
                size_t j = i;
                while (j < pat.size() && pat[j] != '>') j++;
                if (pat[j] != '>') throw std::invalid_argument("One of the <>-braces are not paired.");
                group_name = pat.substr(i + 1, j - i - 1);
                nodes.erase(nodes.begin() + i, nodes.begin() + j + 1);
                pat.erase(pat.begin() + i, pat.begin() + j + 1);
                sec = sec - (j - i + 1);
                i--;
            }
            i++;
        }
        i = fir + 1;
        while (i < sec)
        {
            if (spec.find(pat[i]) == spec.npos || pat[i] == '.' || pat[i - 1] == '&')
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
            if (pat[i] == '{' && !nodes[i]->info)
            {
                size_t j = i;
                BraceNode *b = new BraceNode(pat[i]);
                while (j < pat.size() && pat[j] != '}') j++;
                if (pat[j] != '}') throw std::invalid_argument("One of the {}-braces are not paired.");
                std::string brace = pat.substr(i + 1, j - i - 1);
                std::istringstream iss(brace);
                int s1 = -1, s2 = -1, s = -1;
                std::string str;
                while (std::getline(iss, str, ',')) {
                    s = std::atoi(str.c_str());
                    if (s1 == -1) s1 = s;
                    else s2 = s;
                }
                if (s == -1);
                else if (s1 != -1 && s2 != -1) {b->start = s1; b->end = s2;}
                else if (pat[i + 1] == ',') b->end = s1;
                else b->start = s1;
                nodes[i] = dynamic_cast<Node*>(b);
                nodes[i]->left = nodes[i-1];
                nodes.erase(nodes.begin() + i + 1, nodes.begin() + j + 1);
                nodes.erase(nodes.begin() + i - 1);
                pat.erase(pat.begin() + i + 1, pat.begin() + j + 1);
                pat.erase(pat.begin() + i - 1);
                sec = sec - (j - i + 1);
                i--;
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
                i--;
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
                i--;
            }
            i++;
        }
        if (!group_name.empty())
            groups[group_name] = nodes[fir + 1];
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
