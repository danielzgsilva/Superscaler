#include <iostream>
#include <iomanip>  
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <cassert>
#include <algorithm> 

// possible instruction stages
#define IF 0
#define ID 1
#define IS 2
#define EX 3
#define WB 4

// number of registers
#define REGS 128

// structure to represent an instruction and all its information
struct Instruction
{
    bool valid = false, src1_rdy = false, src2_rdy = false;
    int stage = 0, type = 0, tag = 0, latency = 0, dest = 0, src1 = 0, src2 = 0;
    int src1_tag = 0, src2_tag = 0;
    int if_start = 0, id_start = 0, is_start = 0, ex_start = 0, wb_start = 0;
    int if_duration = 0, id_duration = 0, is_duration = 0, ex_duration = 0, wb_duration = 0;

    bool operator < (const Instruction& inst) const
    {
        return (tag < inst.tag);
    }
};

// Structure to represent a single register (contains tag and in register info)
struct Reg
{
    int tag;
    bool in_reg_file;

    Reg() : tag(-1), in_reg_file(true) {}
};

// Circular FIFO object to represent a reorder buffer
class ROB
{   
    public: 
        unsigned long head = 0, tail = 0;

        std::vector<Instruction> list;

        ROB() {};
        void insert(Instruction inst);
        void update_entry(Instruction inst);
        bool is_empty();
};

// pipeline functions
bool Advance_Cycle(std::ifstream& input);
void FakeRetire();
void Execute();
void Issue(unsigned long N);
void Dispatch(unsigned long S);
void Fetch(std::ifstream& input, unsigned long N);

// helper and debugging functions
void remove_from_list(Instruction inst, std::list<Instruction>& list);
void debug();
void print_inst(Instruction inst);
void print_rob();
void print_reg();
void print_list(std::list<Instruction> &list);
void print_lists();

// global variables
extern int PC, Cycle;

// global objects
extern std::map<int, Reg> reg_file;
extern std::list<Instruction> dispatch_list, issue_list, execute_list;
extern ROB rob;

