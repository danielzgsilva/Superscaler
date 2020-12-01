#include "headers.h"

bool Advance_Cycle(std::ifstream& input)
{
	if (rob.is_empty() && input.eof())
		return false;
	else
        Cycle++;
		return true;
}

void FakeRetire()
{
	std::cout << "attempt retire " << rob.head << " " << rob.head % 1024 << std::endl;

	// Remove instructions from head of ROB
	// so long as they're in WB state
	while(rob.list[rob.head % 1024].valid && rob.list[rob.head % 1024].stage == WB)
	{
		rob.list[rob.head].valid = false;
		rob.head++;
		std::cout << "attempt retire " << rob.head << " " << rob.head % 1024 << std::endl;
	}

	std::cout << "done retiring " << std::endl;
}

void Execute()
{	
	std::list<Instruction>::iterator inst = execute_list.begin();
    while (inst != execute_list.end()) 
	{
		// Move instructions that are finishing execution to writeback stage
		if (inst->latency <= 1)
		{	
			// update ROB info
			inst->stage = WB;
			rob.update_entry(*inst);
			
			// update register file
			if (inst->dest != -1)
			{
				reg_file[inst->dest].tag = -1;
				reg_file[inst->dest].in_reg_file = true;
			}

			// Wake up dependent instructions by broadcasting to scheduling queue
			std::list<Instruction>::iterator issue_inst = issue_list.begin();
			while (issue_inst != issue_list.end()) 
			{
				if (issue_inst->src1_tag == inst->tag)
				{
					issue_inst->src1_rdy = true;
					issue_inst->src1_tag = -1;
				}
				if (issue_inst->src2_tag == inst->tag)
				{
					issue_inst->src2_rdy = true;
					issue_inst->src2_tag = -1;
				}
				rob.update_entry(*issue_inst);
				issue_inst++;
			}

			// remove from execution list
			inst = execute_list.erase(inst);
		}
		else
		{
			inst->latency--;
			inst++;
		}
	}

	std::cout << "done execute" << std::endl;
}

void Issue()
{
	// Create list of instructions with ready operands 
	std::list<Instruction>::iterator inst = issue_list.begin();
	std::vector<Instruction> ready_list;
    while (inst != issue_list.end()) 
	{
		if (inst->src1_rdy && inst->src2_rdy)
		{
			ready_list.push_back(*inst);
		}
		inst++;
	}

	// Sort ready instructions in ascending tag order
	std::sort(ready_list.begin(), ready_list.end());

	// Move up to N+1 instructions from scheduling queue into functional units
	long unsigned int issued = 0;
	std::vector<Instruction>::iterator rdy_inst = ready_list.begin();
	while (issued <= N && rdy_inst != ready_list.end())
	{
		// remove instruction from issue_list
		remove_from_list(*rdy_inst, issue_list);

		// move instruction from issue stage to execute stages
		rdy_inst->stage = EX;
		rob.update_entry(*rdy_inst);

		// push instruction to execute list 
		execute_list.push_back(*rdy_inst);

		rdy_inst++;
		issued++;
	}
	std::cout << "done issue" << std::endl;
}

void Dispatch()
{
	// Scan dispatch list in ascending order of tags
	// move instructions in decode stage to the scheduling queue

	std::cout << "start dispatch" << std::endl;

	std::list<Instruction>::iterator inst = dispatch_list.begin();
    while (inst != dispatch_list.end()) 
	{
		std::cout << "TEST" << std::endl;
        if (inst->stage == ID && issue_list.size() < S) 
		{
			std::cout << "move inst to IS: " << inst->tag << std::endl;
			fflush(stdout);
			// move instruction to issue stage
			inst->stage = IS;
			std::cout << "done move inst to IS: " << inst->tag << std::endl;
			// rename source operands using tomasulo's
			if (inst->src1 == -1)
			{
				inst->src1_tag = -1;
				inst->src1_rdy = true;
			}
			else
			{
				inst->src1_tag = reg_file[inst->src1].tag;
				inst->src1_rdy = reg_file[inst->src1].in_reg_file;
			}
			if (inst->src2 == -1)
			{
				inst->src2_tag = -1;
				inst->src2_rdy = true;
			}
			else
			{
				inst->src2_tag = reg_file[inst->src2].tag;
				inst->src2_rdy = reg_file[inst->src2].in_reg_file;
			}
			// update instruction information in rob
			rob.update_entry(*inst);

			// update destination register state in register file
			if (inst->dest != -1)
			{
				reg_file[inst->dest].tag = inst->tag;
				reg_file[inst->dest].in_reg_file = false;
			}

			// add instruction to scheduling queue
			issue_list.push_back(*inst);
			
			// remove instruction from dispatch list 
            inst = dispatch_list.erase(inst);
        } 
		else
		{
			// move instructions in fetch stage to decode stage
			if (inst->stage == IF)
			{
				inst->stage = ID;
			}
			inst++;
		}
    }
	std::cout << "done dispatch" << std::endl;
}

void Fetch(std::ifstream& input)
{
	int type, dest, src1, src2;
	long unsigned int num_fetched = 0;
    std::string line, hex_address;

	if (input.eof())
		return;

	// Fetch instructions while not at the end of trace file
	// Dispath queue is not full
	// and fetch bandwidth not yet exceeded
	while (getline(input, line) && dispatch_list.size() <= 2*N && num_fetched <= N)
	{
		// ignore empty line
		if (line.empty()) 
		{
			continue;
		}

		// read instruction
		std::istringstream ss_line(line);
		if (!(ss_line >> hex_address >> type >> dest >> src1 >> src2)) { break; }
		
		// Create instruction object
		Instruction inst;
		inst.valid = true;
		inst.stage = IF;
		inst.tag = PC;
		inst.type = type;
		inst.dest = dest;
		inst.src1 = src1;
		inst.src2 = src2;
		
		// Set latency based on instruction type
		switch(inst.type) {
			case 0:
				inst.latency = 1;
				break; 
			case 1:
				inst.latency = 2;
				break;
			case 2:
				inst.latency = 5;
				break;
		}

		// Add instruction to ROB and dispatch list
		rob.insert(inst);
		dispatch_list.push_back(inst);

		PC++;		
		num_fetched++;
	}

	std::cout << "done fetching" << std::endl;
}