/*OBQ does not update all PCs that are matching within the entries
 *only updates the first by going to the BHT to change the local prediction
 *
 *
 */

#include <iostream>
#include <vector>
using namespace std;

struct OBQ_entry
{
	int pc;									//pc for vector
	bool pattern;								//pattern from BHT for given PC
	bool valid;								//valid bit from BHT
	
};

vector<OBQ_entry> g_OBQ;
int head = 0;
int tail = 0;
int size_of_OBQ = 32;								//32 entry OBQ
int new_branch_inst(int, bool, bool);
void retire_branch_inst(int);
void repair_OBQ(int);
void print_OBQ();
int main()
{
	int ret_val;								//test return value;
	for(int i = 0; i < 33; i++)
	{
		ret_val = new_branch_inst((i % 4), false, true);
		//cout << ret_val << " " << head << " " << tail << endl;	
	}
	retire_branch_inst(0);
	retire_branch_inst(1);
	retire_branch_inst(2);
	retire_branch_inst(3);
	retire_branch_inst(4);
	retire_branch_inst(5);
	retire_branch_inst(6);
	retire_branch_inst(7);
	//test overflow of buffer, ensure no current branches get evicted
	//new_branch_inst(11, false, true);
	//new_branch_inst(11, false, true);
	//new_branch_inst(11, false, true);
	//new_branch_inst(11, false, true);
	//new_branch_inst(11, false, true);
	//new_branch_inst(11, false, true);
	new_branch_inst(11, false, true);
	new_branch_inst(11, false, true);
	//new_branch_inst(11, false, true);
	//new_branch_inst(11, false, true);
	//new_branch_inst(11, false, true);
	print_OBQ();
	repair_OBQ(8);
	print_OBQ();
	return 0;
}
//function to add entry to OBQ wehn instruction is a conditional branch - return value is the tag that is sent down the pipeline
//****IF NO HIT IN BHT MAKE ENTRY, BUT PUT BEFORE TAIL POINTER******
int new_branch_inst(int pc, bool pattern, bool valid)
{
	int tag = g_OBQ.size();
	if(((head % size_of_OBQ) == ((tail) % size_of_OBQ)) && (g_OBQ.size() == 32))
	{
		return -1;							//buffer is full
	}
	else
	{	//if hit in BHT
		g_OBQ.push_back({pc, pattern, valid});
		//else no hit in BHT
		//g_OBQ.push_back({pc, pattern, valid});
		//g_OBQ[tail - 1] = temp
		//g_OBQ[tail - 1] = g_OBQ[tail]
		//g_OBQ[tail] = temp	
		tail++;
		if(tail > 31)
		{
			tail = 0;
		}
		return tag;
	}
}
//Function to remove entry from OBQ when instruction retires
void retire_branch_inst(int tag)
{										//what if there is a squashed instruction, do I need to clear the OBQ -- definitley yes but how??	
	if(g_OBQ.empty())
	{
		return;
	}
	g_OBQ.erase(g_OBQ.begin()+(tag - head));				//assuming just retires the head instruction
	head++;
	if(head > size_of_OBQ)
	{
		head = 0;
	}	
}
//Function to update on a misprediction
void repair_OBQ(int tag)
{
	int repair_pc = g_OBQ[tag].pc;
	for(int i = tag - head; i < g_OBQ.size(); i++)
	{
		if(repair_pc == g_OBQ[i].pc)
		{
			g_OBQ[i].pattern = ~(g_OBQ[i].pattern);
		}
	}	

}
//Function to print out the contents of the OBQ
void print_OBQ()
{
	for(int i = 0; i < g_OBQ.size(); i++)
	{
		cout << "Index: " << i << " " << "PC: " << g_OBQ[i].pc << " Pattern: " << g_OBQ[i].pattern << endl;  
	}
	cout << "HEAD: " << head << " TAIL: " << tail << endl;
	return;
}
