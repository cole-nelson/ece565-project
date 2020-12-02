#ifndef __PC_REPAIR__
#define __PC_REPAIR__

#include <deque>
#include <unordered_set>

#include "base/types.hh"

class PCRepair {

protected:
    int M; // Max number of PCs to repair
    std::deque<Addr> lru_queue; // Usual case of M PCs overriding TAGE
    std::unordered_set<Addr> repair_set;

    std::deque<Addr> update_history; // For LRUQ < M case
    std::unordered_set<Addr> history_set;

public:
    PCRepair(int M);
    virtual ~PCRepair();
    /* Updates PC in LRU queue, or adds if did not exist prior */
    void mark_pc(int pc);
    /* Updates the history queue */
    void add_history(int pc);

    void remove_lru_pc(int pc);

    void remove_history_pc(int pc);

    bool eligible(int pc);

};


#endif
