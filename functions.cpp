#include "headers.h"

bool Advance_Cycle(std::ifstream& input)
{
	if (pipeLine_empty == true && input.eof())
		return false;
	else
        PC++;
		return true;
}

void FakeRetire();
void Execute();
void Issue();
void Dispatch();

void Fetch(std::ifstream& input)
{
    std::string line, hex_address;
    int type, dest, src1, src2;
	Instruction inst;
	int ctr = 0;

	if (input.eof() || DE.empty == false)
		return;
	
	if (DE.empty)
	{
		while (getline(input, line))
		{
            // ignore empty line
            if (line.empty()) 
            {
                continue;
            }

            // read in address and outcome
            std::istringstream ss_line(line);
            if (!(ss_line >> hex_address >> type >> dest >> src1 >> src2)) { break; }
            
			inst.tag = PC;
			inst.type = type;
			calc_src_dst(&instruction, line);
			instruction.rs1_rdy = instruction.rs2_rdy = instruction.rs1_ROB = instruction.rs2_ROB = false;
			instruction.FE_start = Cycle;
			instruction.FE_cycles = 1;
			instruction.DE_start = Cycle + 1;
			
			if (inst.type == 0)
				inst.latency = 1;
			else if (inst.type == 1)
				inst.latency = 2;
			else if (inst.type == 2)
				inst.latency = 5;
            else
                std::cout << 'UNKNOWN INSTRUCTION TYPE !!') std::endl;;

			DE.reg.push_back(instruction);
			PC++;
			ctr++;
			
			if (DE.reg.size() == ((unsigned)Width))
				break;
		}
		if (ctr != 0)
			DE.empty = false;
	}
}