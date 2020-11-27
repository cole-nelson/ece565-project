#include <iostream>
#include <vector>

using namespace std;

class OBQ
{
        public:
                struct OBQ_entry
                {
                        int pc;
                        bool valid;
                        bool pattern;
                };
                std::vector<OBQ_entry> g_OBQ;
                int head = 0;
                int tail = 0;
                int size_of_OBQ = 32;

                /*Function to retire OBQ entry after cond branch committed*/
                void retire_branch(int tag);

                /*Function to add new branch instruction to OBQ*/
                int new_branch_inst(int pc, bool pattern, bool valid);

                /*Function to get size of obq*/
                int get_obq_size();
};
int OBQ::get_obq_size()
{
        return g_OBQ.size();

}

void OBQ::retire_branch(int tag)
{
        if (g_OBQ.empty())
        {
                return;
        }
        g_OBQ.erase(g_OBQ.begin()+(tag - head));
        head++;
        if (head > size_of_OBQ)
        {
                head = 0;
        }
}

int OBQ::new_branch_inst(int pc, bool pattern, bool valid)
{
        int tag = g_OBQ.size();
        if (((head % size_of_OBQ) ==
                ((tail) % size_of_OBQ)) && (g_OBQ.size() == 32))
        {
                return -1;
        }
        else
        {	//if hit in BHT
                printf("before push back \n");
                g_OBQ.push_back({pc, pattern, valid});
                tail++;
                if (tail > 31)
                {
                        tail = 0;
                }
                return tag;
        }
}
void print_OBQ(OBQ obq);
int main()
{
        int ret_val;
        OBQ obq;
        for (int i = 0; i < 32; i++)
        {
                ret_val = obq.new_branch_inst((i %4), false, true);
        }
        obq.retire_branch(0);
        obq.retire_branch(1);
        obq.retire_branch(2);
        obq.retire_branch(3);

        print_OBQ(obq);
        return 0;

}
void print_OBQ(OBQ obq)
{
        for (int i =0; i < obq.g_OBQ.size(); i++)
        {
                cout << "Index: " << i << " " << "PC: "
                << obq.g_OBQ[i].pc << " Pattern: "
                << obq.g_OBQ[i].pattern << endl;

        }
        cout << "HEAD: " << obq.head << " TAIL: " << obq.tail << endl;
        return;
}
