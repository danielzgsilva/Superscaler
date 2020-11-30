#include "headers.h"

int PC = 0;


int main(int argc, char** argv)
{
    // Get arguments
    int S = atoi(argv[1]);
    int N = atoi(argv[2]);
    std::string trace_file = argv[3];

    // Create trace file stream
    std::ifstream input(trace_file);

    // Process input file
    if (input.is_open())
    {
        do
        {
            FakeRetire();
            Execute();
            Issue();
            Dispatch();
            Fetch(input);

        } while (Advance_Cycle(input));

        input.close();
    }

    return 0;
}