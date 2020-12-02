#include "headers.h"

void ROB::insert(Instruction inst)
{
    // insert instruction into ROB at tail pointer and then advance tail 
    this->list.push_back(inst);
    tail++;
}

bool ROB::is_empty()
{
    // ROB is empty if head and tail pointer point to same entry
    return this->head == this->tail;
}

void ROB::update_entry(Instruction inst)
{
    this->list[inst.tag] = inst;
}

void remove_from_list(Instruction instruction, std::list<Instruction>& list)
{
    // removes given instruction from a given list 

    std::list<Instruction>::iterator it = list.begin();
    while (it != list.end()) 
	{
        // ensure matching tags prior to removal
        if (it->tag == instruction.tag)
        {
            it = list.erase(it);
            break;
        }
        it++;
    }
}

// ---------------------------------------------------------------------------------
// the remaining functions were used for debugging                                 |
// ---------------------------------------------------------------------------------

void print_inst(Instruction inst)
{
    std::map<int, std::string> stages;
    stages[0] = "IF";
    stages[1] = "ID";
    stages[2] = "IS";
    stages[3] = "EX";
    stages[4] = "WB";

    if (inst.valid)
    {
        assert(inst.type >= 0 || inst.type <= 2);
        std::cout << " Tag: " << inst.tag;
        std::cout << " Type: " << inst.type;
        std::cout << " Stage: " << stages[inst.stage];
        std::cout << " Dest: " << inst.dest;
        std::cout << " Src1: " << inst.src1;
        std::cout << " Src1 tag: " << inst.src1_tag;
        std::cout << " Src1 rdy: " << inst.src1_rdy;
        std::cout << " Src2: " << inst.src2;
        std::cout << " Src2 tag: " << inst.src2_tag;
        std::cout << " Src2 rdy: " << inst.src2_rdy;

        /* std::cout << " IF beg: " << inst.if_start;
        std::cout << " IF dur: " << inst.if_duration;
        std::cout << " ID beg: " << inst.id_start;
        std::cout << " ID dur: " << inst.id_duration;
        std::cout << " IS beg: " << inst.is_start;
        std::cout << " IS dur: " << inst.is_duration;
        std::cout << " EX beg: " << inst.ex_start;
        std::cout << " EX dur: " << inst.ex_duration;
        std::cout << " WB beg: " << inst.wb_start;
        std::cout << " WB dur: " << inst.wb_duration; */
        std::cout << std::endl;
    }
    else
    {
         std::cout << " Tag: " << inst.tag;
        std::cout << " invalid instruction" << std::endl;
    }
    
}

void print_list(std::list<Instruction> &list)
{
    for (auto i: list) {
        print_inst(i);
        fflush(stdout);
    }
}

void print_rob()
{
    std::cout << "ROB" << std::endl;
    //for (unsigned long int i = 0; i < ROB_SIZE; i++)
    for (unsigned long int i = 0; i < rob.list.size(); i++)
    {
        if (i == rob.head)
            std::cout << "HEAD ";
        if (i == rob.tail)
            std::cout << " TAIL ";
        
        std::cout << "entry: " << i;
        print_inst(rob.list[i]);
    }
    std::cout << std::endl;
}

void print_reg()
{
    std::cout << "REG FILE - size:  " << reg_file.size() <<  std::endl;
    for (int i = 0; i < REGS; i++)
    {
        std::cout << i << ": " << reg_file.at(i).in_reg_file << " " << reg_file.at(i).tag <<  std::endl;
    }

    std::cout << std::endl;
}

void print_lists()
{
    std::cout << "dispatch list - size: " << dispatch_list.size() <<  std::endl;
    print_list(dispatch_list);

    std::cout << "issue list - size: " << issue_list.size() <<  std::endl;
    print_list(issue_list);

    std::cout << "execute list - size: " << execute_list.size() <<  std::endl;
    print_list(execute_list);
}

void debug()
{
    print_reg();
    print_rob();
    print_lists();
}