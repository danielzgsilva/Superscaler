#include "headers.h"

void ROB::insert(Instruction inst)
{
    this->list[this->tail % 1024] = inst;
    tail++;
}

void ROB::is_empty()
{

}