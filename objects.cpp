#include "headers.h"

void ROB::insert(Instruction inst)
{
    this->list[this->tail % 1024] = inst;
    tail++;
}

bool ROB::is_empty()
{
    return this->head == this->tail;
}

void ROB::update_entry(Instruction inst)
{
    this->list[inst.tag % 1024] = inst;
}

void remove_from_list(Instruction instruction, std::list<Instruction>& list)
{
    // removes given instruction from the given list 

    std::list<Instruction>::iterator it = list.begin();
    while (it != list.end()) 
	{
        // match tags 
        if (it->tag == instruction.tag)
        {
            it = list.erase(it);
            break;
        }
        it++;
    }
}

void print_inst(Instruction inst)
{
    if (inst.valid)
    {
        std::cout << "Tag: " << inst.tag;
        std::cout << "Type: " << inst.type;
        std::cout << "Tag: " << inst.stage;
        std::cout << "Dest: " << inst.dest;
        std::cout << "Src1: " << inst.src1;
        std::cout << "Src2: " << inst.src2;
        std::cout << "Latency: " << inst.latency;
        std::cout << std::endl;
    }
}

void debug()
{
    std::cout << "ROB" << std::endl;
    for (int i = 0; i < 1024; i++)
    {
        std::cout << "Entry: " << i;
        print_inst(rob.list[i]);
        std::cout << std::endl;
    }
}