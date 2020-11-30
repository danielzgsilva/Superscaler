#include <iostream>
#include <iomanip>  
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>

#define IF 0
#define ID 1
#define IS 2
#define EX 3
#define WB 3

struct Instruction
{
    int stage, type, tag, latency, dest, src1, src2;
};

extern int PC, N, S;
extern std::list<Instruction> dispatch_list, issue_list, execute_list;

bool Advance_Cycle(std::ifstream& input);
void FakeRetire();
void Execute();
void Issue();
void Dispatch();
void Fetch(std::ifstream& input);

class ROB
{   
    public: 
        unsigned long head = 0, tail = 0;

        std::array<Instruction, 1024> list;

        ROB() {};
        void insert(Instruction);
        void is_empty();
};