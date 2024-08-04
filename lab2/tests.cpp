#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <random>
#include "lib/dfa/dfa.h"

std::random_device r;
std::mt19937 rng(r());
std::uniform_int_distribution<std::mt19937::result_type> info(32,127);

TEST_CASE("Tree testing")
{
    SECTION("Node testing")
    {
        char c = static_cast<char>(info(rng));
        Node n(c);
        REQUIRE(n.info == c);
        c = static_cast<char>(info(rng));
        Node n1(c);
        n.left = &n1;
        REQUIRE(n.left->info == c);
    }
    SECTION("STree testing")
    {
        Node *n = new Node(static_cast<char>(info(rng)));
        STree tr(n);
        REQUIRE(tr.get_root() == n);
        REQUIRE_THROWS(tr.synt("(m-n"));
        REQUIRE_NOTHROW(tr.synt("m-n-p+-r?-(<name>t-p-r)|k"));
        REQUIRE(tr.get_root()->info == '|');
        REQUIRE(tr.get_groups()["name"]->info == '-');
        tr.synt("m-n-p|r+|t-(r|p)+");
        std::stringstream ss;
        tr.lcp(ss);
        REQUIRE(ss.str() == "| | - - m n p + r - t + | r p ");
    }
}

TEST_CASE("NFA testing")
{
    SECTION("StateNFA testing")
    {
        char c = static_cast<char>(info(rng));
        StateNFA *s1 = new StateNFA;
        StateNFA *s2 = new StateNFA;
        s1->edge[c] = s2;
        StateNFA *s3 = s1;
        REQUIRE(s3->edge[c] == s2);
    }
    SECTION("NFA default constructors")
    {
        char c1 = static_cast<char>(info(rng)), c2 = static_cast<char>(info(rng));
        NFA n1(c1), n2(c2);
        NFA n3(std::move(n1));
        REQUIRE(n1.get_start() == nullptr);
        REQUIRE(n1.get_end() == nullptr);
        REQUIRE(n3.get_start()->edge[c1] == n3.get_end());
    }
}

TEST_CASE("DFA testing")
{
    SECTION("DFA construction + check")
    {
        STree s;
        s.synt("m-n+-p|r?|(y-r)+");
        NFA n(s);
        DFA d(n);
        REQUIRE(d.check("mnp"));
        REQUIRE(d.check("yryr"));
        REQUIRE(!d.check("mp"));
        s.synt("m{1,3}|r?|t-p");
        NFA n1(s);
        DFA d1(n1);
        REQUIRE(d1.check(""));
        REQUIRE(!d1.check("mmmm"));
        REQUIRE(d1.check("tp"));
    }
}
