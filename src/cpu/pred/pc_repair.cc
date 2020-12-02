#include <deque>
#include <unordered_set>

#include "base/types.hh"

#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/random.hh"
#include "base/trace.hh"
#include "cpu/pred/pc_repair.hh"
#include "debug/LTage.hh"
#include "debug/PCRepair.hh"

PCRepair::PCRepair(int M) : M(M) {
    DPRINTF(PCRepair, "Created PC Repair of size %d\n", M);
}

PCRepair::~PCRepair() {}

bool PCRepair::eligible(int pc) {
   // DPRINTF(PCRepair, "PCRepair: Eligible (pc = %d)\n", pc);
    if(repair_set.count(pc)) {
        //DPRINTF(PCRepair, "PCRepair: Exiting False (pc = %d)\n", pc);
        return true;
    } else if(repair_set.size() < M) {
        //DPRINTF(PCRepair, "PCRepair: Before Loop (pc = %d)\n", pc);
        // See if PC was recently updated in BHT
        auto diff = M - repair_set.size();
        for(auto i = 0; i < diff && i < update_history.size(); i++) {
            if(update_history.at(i) == pc) {
                return true;
            }
        }
    }
    
    //DPRINTF(PCRepair, "PCRepair: Exiting False (pc = %d)\n", pc);

    return false;
}

void PCRepair::mark_pc(int pc) {
    DPRINTF(PCRepair, "Marking PC %d in LRU queue\n", pc);
    if(repair_set.count(pc)) {
        // PC Already in queue, move to front
        for(auto it = lru_queue.begin(); it != lru_queue.end(); it++) {
            if(*(it) == pc) {
                //auto temp = *(it);
                lru_queue.erase(it);
                lru_queue.insert(lru_queue.begin(), pc);
                break;
            }
        }
        
        // LRU Queue & Repair set should be synced
        assert(lru_queue.front() == pc);
    } else if(repair_set.size() == M) {
        // Already tracking M PCs, remove LRU & Add PC
        auto temp = lru_queue.back();
        lru_queue.pop_back();
        lru_queue.push_front(pc);
        auto temp_idx = repair_set.find(temp);
        
        // LRU Queue & Repair set should be synced
        assert(temp_idx != repair_set.end());
        // Replace PC in repair set
        repair_set.erase(temp_idx);
        repair_set.insert(pc);
    } else {
        // Warm-up case: Not tracking M PCs yet
        lru_queue.push_front(pc);
        repair_set.insert(pc);
    }

    assert(repair_set.size() == lru_queue.size() && repair_set.size() <= M);
}

void PCRepair::remove_lru_pc(int pc) {
    DPRINTF(PCRepair, "Killing PC %d in LRU queue\n", pc);
    if(repair_set.count(pc)) {
        for(auto it = lru_queue.begin(); it != lru_queue.end(); it++) {
            if(*(it) == pc) {
                lru_queue.erase(it);
                break;
            }
        }

        repair_set.erase(repair_set.find(pc));
    }
}

// Note: Usage should dictate that the history set is disjoint from the LRU set,
// as PCs added to LRU set should not be added to the history set. Converse is not true
// however, as a PC could be promoted from history -> LRU set. Need to remove from one
// before adding to other.
void PCRepair::add_history(int pc) {
    DPRINTF(PCRepair, "Marking PC %d in history queue\n", pc);
    if(history_set.count(pc)) {
        for(auto it = update_history.begin(); it != update_history.end(); it++) {
            if(*(it) == pc) {
                update_history.erase(it);
                update_history.push_front(pc);
                break;
            }
        }

        assert(update_history.front() == pc);
    } else if(history_set.size() == M) {
        auto temp = update_history.back();
        update_history.pop_back();
        auto temp_idx = history_set.find(temp);
        assert(temp_idx != history_set.end());

        history_set.erase(temp_idx);

        // Add new PC
        history_set.insert(pc);
        update_history.push_front(pc);
    } else {
        update_history.push_front(pc);
        repair_set.insert(pc);
    }

    assert(update_history.size() == history_set.size() && history_set.size() <= M);
}

void PCRepair::remove_history_pc(int pc) {
    DPRINTF(PCRepair, "Killing PC %d in history queue\n", pc);
    if(history_set.count(pc)) {
        for(auto it = update_history.begin(); it != update_history.end(); it++) {
            if(*(it) == pc) {
                update_history.erase(it);
                break;
            }
        }

        history_set.erase(repair_set.find(pc));
    }
}
