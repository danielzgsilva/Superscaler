#include "headers.h"

// Init global variables
int PC = 0;
int Cycle = 0;

// Init global objects
ROB rob = ROB();
std::list<Instruction> dispatch_list, issue_list, execute_list;
std::map<int, Reg> reg_file;


int main(int argc, char** argv)
{
    // Get arguments
    unsigned long int S = strtoul(argv[1], NULL, 10);
    unsigned long int N = strtoul(argv[2], NULL, 10);
    std::string trace_file = argv[3];

    // Initialize register file 
    for (int i = 0; i < REGS; i++)
    {
        reg_file.insert({i, Reg()});
    }

    // Create trace file stream
    std::ifstream input(trace_file);

    //std::cout << S << " " << N << " " << trace_file << std::endl;

    // Process input file
    if (input.is_open())
    {
        do
        {
            FakeRetire();
            Execute();
            Issue(N);
            Dispatch(S);
            Fetch(input, N);

        } while (Advance_Cycle(input));

        input.close();
    }

    // print statistics
    std::cout << "number of instructions = " << PC << std::endl;
    std::cout << "number of cycles       = " << Cycle << std::endl;
    std::cout << "IPC                    = " << std::fixed << std::setprecision(5) << PC / (float)Cycle << std::endl;

    return 0;
}