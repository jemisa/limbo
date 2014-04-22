// vim:filetype=c:textwidth=80:shiftwidth=4:softtabstop=4:expandtab
/*
 * Reasoning about beliefs and belief revision.
 * At the core of our notion of belief there are belief conditionals which have
 * the form phi => psi and express that in the most plausible scenario where phi
 * holds, psi holds as well.
 *
 * In ESB, the belief conditional arrow `=>' is translated to material
 * implication plus some other conditions.
 * Since ESL allows only proper+ formulas, which are disjunctions of formulas,
 * we require phi in phi => psi to be a conjunction and psi to be a disjunction.
 * For technical reasons we do not explicitly introduce conjunctions but rather
 * use neg_phi which shall be a disjunction which is the negation of phi.
 *
 * Thus, we can construct a model for belief conditionals using the procedure
 * described in the proof of Theorem 7 from the ESB paper:
 * Let p := 0 and S be the set of belief conditionals not yet satisfied.
 * Compute the setup that satisfies, for all phi => psi in S, (neg_phi v psi),
 * which represents the material implication from rule 12(a).
 * If this setup satisfies neg_phi, rule 12(c) is violated for that belief
 * conditional, and therefore we leave phi => psi in S; otherwise, the belief
 * conditional is satisfied and we can remove phi => psi from S.
 * Repeat that loop until p > m where m is the total number of belief
 * conditionals.
 *
 * schwering@kbsg.rwth-aachen.de
 */
#ifndef _BELIEF_H_
#define _BELIEF_H_

#include "setup.h"
#include "query.h"

typedef struct belief_cond belief_cond_t;

VECTOR_DECL(belief_conds, belief_cond_t *);
VECTOR_DECL(ranked_setups, setup_t *);

typedef struct bcontext bcontext_t;
struct bcontext {
    int plausibility;
    kcontext_t ctx;
    bcontext_t *next;
};

ranked_setups_t setup_init_beliefs(
        const setup_t *static_bat_setup,
        const belief_conds_t *beliefs,
        const stdset_t *hplus,
        const int k);

#endif

