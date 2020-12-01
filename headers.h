#include <iostream>
#include <iomanip>  
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <algorithm> 

#define IF 0
#define ID 1
#define IS 2
#define EX 3
#define WB 3

struct Instruction
{
    bool valid, src1_rdy, src2_rdy;
    int stage, type, tag, latency, dest, src1, src2;
    int src1_tag, src2_tag;

    Instruction() : valid(false), src1_rdy(false), src2_rdy(false) {}

    bool operator < (const Instruction& inst) const
    {
        return (tag < inst.tag);
    }
};

struct Reg
{
    int tag;
    bool in_reg_file;

    Reg() : tag(-1), in_reg_file(true) {}
};

class ROB
{   
    public: 
        unsigned long head = 0, tail = 0;

        std::array<Instruction, 1024> list;

        ROB() {};
        void insert(Instruction inst);
        void update_entry(Instruction inst);
        bool is_empty();
};

bool Advance_Cycle(std::ifstream& input);
void FakeRetire();
void Execute();
void Issue();
void Dispatch();
void Fetch(std::ifstream& input);

void remove_from_list(Instruction inst, std::list<Instruction>& list);

extern int PC, Cycle;
extern long unsigned int N, S;
extern std::map<int, Reg> reg_file;
extern std::list<Instruction> dispatch_list, issue_list, execute_list;
extern ROB rob;

void debug();
