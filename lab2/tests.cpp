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
        REQUIRE(tr.root == n);
        REQUIRE_THROWS(tr.synt("(m-n"));
        REQUIRE_NOTHROW(tr.synt("m-n-p+-r?-(<name>t-p-r)|k"));
        REQUIRE(tr.root->info == '|');
        REQUIRE(tr.groups["name"]->info == '-');
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
    SECTION("Capture groups")
    {
        STree tr;
        tr.synt("(<beb>n-p?-r-q+)?-(<ded>y-t)?-p|(<quq>a-h)");
        NFA n(tr);
        std::string name;
        std::string ch_str = "nprqqqyt";
        n.check_with_group(ch_str, "beb", name);
        REQUIRE(name == "nprqqq");
        n.check_with_group(ch_str, "ded", name);
        REQUIRE(name == "yt");
        ch_str = "ah";
        n.check_with_group(ch_str, "quq", name);
        REQUIRE(name == "ah");
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
    SECTION("minDFA")
    {
        STree s;
        s.synt("m-n+-p|r?|(y-r)+");
        NFA n(s);
        DFA d(n);
        d.minimize();
        REQUIRE(d.check("mnp"));
        REQUIRE(d.check("yryr"));
        REQUIRE(!d.check("mp"));
        s.synt("m{1,3}|r?|t-p");
        NFA n1(s);
        DFA d1(n1);
        d1.minimize();
        REQUIRE(d1.check(""));
        REQUIRE(!d1.check("mmmm"));
        REQUIRE(d1.check("tp"));
    }
    SECTION("K-path")
    {
        STree s;
        s.synt("a+");
        NFA n(s);
        DFA d(n);
        d.minimize();
        REQUIRE_NOTHROW(s.synt(d.k_path()));
    }
    SECTION("Intersection DFA")
    {
        STree s;
        s.synt("a|b-c");
        NFA n(s);
        DFA d1(n);
        d1.minimize();
        s.synt("a|b");
        NFA n2(s);
        DFA d2(n2);
        d2.minimize();
        DFA sum_df = d1 + d2;
        REQUIRE(sum_df.check("a"));
        REQUIRE(!sum_df.check("b"));
        REQUIRE(!sum_df.check("bc"));
    }
    SECTION("Inversion DFA")
    {
        STree s;
        s.synt("a-b");
        NFA n(s);
        DFA d(n);
        d.minimize();
        REQUIRE(d.check("ab"));
        d.inverse();
        REQUIRE(d.check("ba"));
        s.synt("a-b|c-d|e-f");
        NFA n1(s);
        DFA d1(n1);
        d1.minimize();
        REQUIRE(d1.check("ab"));
        REQUIRE(d1.check("cd"));
        REQUIRE(d1.check("ef"));
        d1.inverse();
        REQUIRE(d1.check("ba"));
        REQUIRE(d1.check("dc"));
        REQUIRE(d1.check("fe"));
    }
}
