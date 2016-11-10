// vim:filetype=cpp:textwidth=80:shiftwidth=2:softtabstop=2:expandtab
// Copyright 2014--2016 Christoph Schwering

#include <gtest/gtest.h>
#include "./clause.h"
#include "./maybe.h"
#include "./pretty.h"

namespace lela {

using namespace output;

struct EqSubstitute {
  EqSubstitute(Term pre, Term post) : pre_(pre), post_(post) {}
  Maybe<Term> operator()(Term t) const { if (t == pre_) return Just(post_); else return Nothing; }

 private:
  const Term pre_;
  const Term post_;
};

TEST(Clause, general) {
  {
    Symbol::Factory sf;
    Term::Factory tf;
    const Symbol::Sort s1 = sf.CreateSort();
    const Symbol::Sort s2 = sf.CreateSort();
    const Term n1 = tf.CreateTerm(sf.CreateName(s1));
    const Term n2 = tf.CreateTerm(sf.CreateName(s1));
    //const Term x1 = tf.CreateTerm(sf.CreateVariable(s1));
    const Term x2 = tf.CreateTerm(sf.CreateVariable(s1));
    const Symbol f = sf.CreateFunction(s1, 1);
    //const Symbol g = sf.CreateFunction(s2, 1);
    const Symbol h = sf.CreateFunction(s2, 2);
    const Term f1 = tf.CreateTerm(f, {n1});
    const Term f2 = tf.CreateTerm(h, {n1,x2});
    //const Term f3 = tf.CreateTerm(g, {f1});
    const Term f4 = tf.CreateTerm(h, {n1,f1});

    EXPECT_TRUE(Clause({Literal::Eq(n1,n1)}).valid());
    EXPECT_TRUE(!Clause({Literal::Neq(n1,n1)}).valid());
    EXPECT_TRUE(Clause({Literal::Eq(f1,f1)}).valid());
    EXPECT_TRUE(!Clause({Literal::Neq(f1,f1)}).valid());
    EXPECT_TRUE(!Clause({Literal::Neq(f1,n1)}).valid());
    EXPECT_TRUE(!Clause({Literal::Neq(f1,f2)}).valid());
    EXPECT_TRUE(Clause({Literal::Eq(n1,n1), Literal::Eq(n2,n2)}).valid());
    EXPECT_TRUE(Clause({Literal::Eq(n1,n1), Literal::Neq(n2,n2)}).valid());
    EXPECT_TRUE(!Clause({Literal::Neq(n1,n1), Literal::Neq(n2,n2)}).valid());

    EXPECT_TRUE(!Clause({Literal::Eq(n1,n1)}).invalid());
    EXPECT_TRUE(Clause({Literal::Neq(n1,n1)}).invalid());
    EXPECT_TRUE(!Clause({Literal::Eq(f1,f1)}).invalid());
    EXPECT_TRUE(Clause({Literal::Neq(f1,f1)}).invalid());
    EXPECT_TRUE(!Clause({Literal::Neq(f1,n1)}).invalid());
    EXPECT_TRUE(!Clause({Literal::Neq(f1,f2)}).invalid());
    EXPECT_TRUE(!Clause({Literal::Eq(n1,n1), Literal::Eq(n2,n2)}).invalid());
    EXPECT_TRUE(!Clause({Literal::Eq(n1,n1), Literal::Neq(n2,n2)}).invalid());
    EXPECT_TRUE(Clause({Literal::Neq(n1,n1), Literal::Neq(n2,n2)}).invalid());

    {
      Clause c1({Literal::Eq(f1,n1)});
      Clause c2({});
      std::cout << 1 << std::endl;
      EXPECT_FALSE(c1.Subsumes(c2));
      std::cout << 2 << std::endl;
      EXPECT_TRUE(c2.Subsumes(c1));
    }

    {
      Clause c1({Literal::Eq(f1,n1)});
      Clause c2({Literal::Neq(f1,n2)});
      EXPECT_TRUE(c1.Subsumes(c2));
      EXPECT_FALSE(c2.Subsumes(c1));
    }

    {
      Clause c1({Literal::Eq(f1,n1)});
      Clause c2({Literal::Eq(f1,n2)});
      EXPECT_FALSE(c1.Subsumes(c2));
      EXPECT_FALSE(c2.Subsumes(c1));
    }
    {
      Clause c1({Literal::Eq(f1,n1)});
      Clause c2({Literal::Eq(f1,n1)});
      EXPECT_TRUE(c1.Subsumes(c2));
      EXPECT_TRUE(c2.Subsumes(c1));
    }

    {
      Clause c1({Literal::Eq(f1,n1), Literal::Neq(n1,n1)});
      Clause c2({Literal::Eq(f1,n1)});
      EXPECT_TRUE(c1.Subsumes(c2));
      EXPECT_TRUE(c2.Subsumes(c1));
      EXPECT_TRUE(c1 == c2); // because of minimization, n1 != n1 is removed
    }

    {
      Clause c1({Literal::Eq(f1,n1), Literal::Neq(n1,n1)});
      Maybe<Clause> c2;
      c2 = c1.PropagateUnit(Literal::Neq(f1,n1));
      EXPECT_TRUE(c2 && c2.val.empty());
      EXPECT_TRUE(c2.val.Subsumes(c1));
      c2 = c1.PropagateUnit(Literal::Eq(f1,n2));
      EXPECT_TRUE(c2 && c2.val.empty());
      EXPECT_TRUE(c2.val.Subsumes(c1));
      c2 = c1.PropagateUnit(Literal::Eq(f1,n1));
      EXPECT_FALSE(c2);
    }

    {
      Clause c1({Literal::Eq(f4,n1), Literal::Eq(f2,n1)});
      EXPECT_TRUE(c1.size() == 2);
      c1 = c1.Substitute(EqSubstitute(f1, n2), &tf);
      EXPECT_TRUE(c1.size() == 2);
      EXPECT_TRUE(!c1.ground());
      c1 = c1.Substitute(EqSubstitute(x2, n2), &tf);
      EXPECT_TRUE(c1.size() == 1);
      EXPECT_TRUE(c1.unit());
    }
  }
  {
    Symbol::Factory sf;
    Term::Factory tf;
    const Symbol::Sort s1 = sf.CreateSort();
    //const Symbol::Sort s2 = sf.CreateSort();
    const Term n = tf.CreateTerm(Symbol::Factory::CreateName(1, s1));
    const Term m = tf.CreateTerm(Symbol::Factory::CreateName(2, s1));
    const Term a = tf.CreateTerm(Symbol::Factory::CreateFunction(1, s1, 0), {});
    //const Term b = tf.CreateTerm(Symbol::Factory::CreateFunction(2, s1, 0), {});
    //const Term fn = tf.CreateTerm(Symbol::Factory::CreateFunction(3, s1, 1), {n});
    //const Term fm = tf.CreateTerm(Symbol::Factory::CreateFunction(3, s1, 1), {m});
    //const Term gn = tf.CreateTerm(Symbol::Factory::CreateFunction(4, s1, 1), {n});
    //const Term gm = tf.CreateTerm(Symbol::Factory::CreateFunction(4, s1, 1), {m});

    Clause c1({Literal::Eq(a,m), Literal::Eq(a,n)});
    Clause c2({Literal::Neq(a,m)});
    EXPECT_FALSE(c1.Subsumes(c2));
    EXPECT_FALSE(c2.Subsumes(c1));
  }
}

}  // namespace lela
 