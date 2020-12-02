#include "headers.h"

bool Advance_Cycle(std::ifstream& input)
{
	// do not advance cycle if ROB is empty and trace file is complete
	if (rob.is_empty() && input.eof())
		return false;
	else
        Cycle++;
		return true;
} 

void FakeRetire()
{
	if (rob.list.size() == 0)
		return;

	Instruction inst;

	// Remove instructions from head of ROB
	// so long as they're in WB state
	while(rob.list[rob.head].valid && rob.list[rob.head].stage == WB)
	{
		// print instruction timing information
		inst = rob.list[rob.head];
		std::cout << inst.tag << " ";
		std::cout << "fu{" << inst.type << "} ";
		std::cout << "src{" << inst.src1 << "," << inst.src2 << "} ";
		std::cout << "dst{" << inst.dest << "} ";
		std::cout << "IF{" << inst.if_start << "," << inst.if_duration << "} ";
		std::cout << "ID{" << inst.id_start << "," << inst.id_duration << "} ";
		std::cout << "IS{" << inst.is_start << "," << inst.is_duration << "} ";
		std::cout << "EX{" << inst.ex_start << "," << inst.ex_duration << "} ";
		std::cout << "WB{" << inst.wb_start << "," << 1 << "}";
		std::cout << std::endl;

		// invalidate entry in rob and advance head pointer
		rob.list[rob.head].valid = false;
		rob.head++;
	}
}

void Execute()
{	
	std::list<Instruction>::iterator inst = execute_list.begin();
    while (inst != execute_list.end()) 
	{
		// Move instructions that are finishing execution to writeback stage
		if (inst->latency <= 1)
		{	
			// advance to WB stage and update ROB
			inst->stage++;
			inst->wb_start = Cycle;
			inst->ex_duration = Cycle - inst->ex_start;
			rob.update_entry(*inst);
			
			// update register file
			if (inst->dest != -1 && reg_file.at(inst->dest).tag == inst->tag)
			{
				reg_file.at(inst->dest).tag = -1;
				reg_file.at(inst->dest).in_reg_file = true;
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
			// if not finshing execution, simply execute one execution cycle
			inst->latency--;
			inst++;
		}
	}
}

void Issue(unsigned long N)
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
		// remove instruction from issue list
		remove_from_list(*rdy_inst, issue_list);

		// move instruction from issue stage to execute stage
		rdy_inst->stage++;
		rdy_inst->ex_start = Cycle;
		rdy_inst->is_duration = Cycle - rdy_inst->is_start;
		rob.update_entry(*rdy_inst);

		// push ready instruction to execute list 
		execute_list.push_back(*rdy_inst);

		rdy_inst++;
		issued++;
	}
}

void Dispatch(unsigned long S)
{
	// Scan dispatch list in ascending order of tags
	// move instructions in decode stage to the scheduling queue

	std::list<Instruction>::iterator inst = dispatch_list.begin();
    while (inst != dispatch_list.end()) 
	{
        if (inst->stage == ID && issue_list.size() < S) 
		{
			// move instruction to issue stage
			inst->stage++;
			inst->is_start = Cycle;
			inst->id_duration = Cycle - inst->id_start;

			// rename source operands using tomasulo'ss
			if (inst->src1 == -1)
			{
				inst->src1_tag = -1;
				inst->src1_rdy = true;
			}
			else
			{
				// getting register status from register file
				inst->src1_tag = reg_file.at(inst->src1).tag;
				inst->src1_rdy = reg_file.at(inst->src1).in_reg_file;
			}
			if (inst->src2 == -1)
			{
				inst->src2_tag = -1;
				inst->src2_rdy = true;
			}
			else
			{
				inst->src2_tag = reg_file.at(inst->src2).tag;
				inst->src2_rdy = reg_file.at(inst->src2).in_reg_file;
			}

			// update instruction information in rob
			rob.update_entry(*inst);

			// update destination register state in register file
			if (inst->dest != -1)
			{
				reg_file.at(inst->dest).tag = inst->tag;
				reg_file.at(inst->dest).in_reg_file = false;
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
				inst->stage++;
				inst->id_start = Cycle;
				inst->if_duration = Cycle - inst->if_start;
				rob.update_entry(*inst);
			}
			inst++;
		}
		
    }
}

void Fetch(std::ifstream& input, unsigned long N)
{
	int type, dest, src1, src2;
	long unsigned int num_fetched = 0;
    std::string line, hex_address;

	if (input.eof()) {return;}

	// Fetch instructions while the following conditions are true:
		// not yet at the end of trace file
		// Dispatch queue is not full
		// and fetch bandwidth not yet exceeded
	while (dispatch_list.size() < 2*N && num_fetched < N && getline(input, line))
	{
		// ignore empty line just in case
		if (line.empty()) {continue;}

		// read instruction
		std::istringstream ss_line(line);
		if (!(ss_line >> hex_address >> type >> dest >> src1 >> src2)) { break; }
		
		// Create instruction object
		Instruction inst;
		inst.valid = true;
		inst.stage = 0;
		inst.tag = PC;
		inst.type = type;
		inst.dest = dest;
		inst.src1 = src1;
		inst.src2 = src2;
		inst.if_start = Cycle;
		
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
}