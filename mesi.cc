/************************************************************
                        Course          :       CSC506
                        Source          :       mesi.cc
                        Instructor      :       Ed Gehringer
                        Email Id        :       efg@ncsu.edu
------------------------------------------------------------
        © Please do not replicate this code without consulting
                the owner & instructor! Thanks!
*************************************************************/

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
using namespace std;
#include "main.h"
#include "mesi.h"
#include "cache.h"

void MESI::PrRd(ulong addr, int processor_number)
{
    cache_state state;
    current_cycle++;
    reads++;
    cache_line * line = find_line(addr);
       if (line == NULL) {
             read_misses++;
             cache_line *newline = allocate_line(addr);
         if(sharers_exclude(addr, processor_number) == 0){
                memory_transactions++;
                I2E++;
                newline->set_state(E);
         }
         else{
                I2S++;
                cache2cache++;
                newline->set_state(S);

         }
             bus_reads++;
             sendBusRd(addr, processor_number);
       }

      else {
             state=line->get_state();
             if (state == I)
                {
                read_misses++;
                cache_line *newline = allocate_line(addr);
                     if(sharers_exclude(addr, processor_number) == 0) //shared line
                     {
                        memory_transactions++;
                        I2E++;
                        newline->set_state(E);
                     }
                     else
                     {
                        cache2cache++;
                        interventions++;
                        I2S++;
                        newline->set_state(S);

                     }
                        bus_reads++;
                        sendBusRd(addr, processor_number);
                }
                else if (state == M || state == S || state == E)
                {
                        update_LRU(line);
                }
           }

}

void MESI::PrRdDir(ulong addr, int processor_number) {

}

void MESI::PrWrDir(ulong addr, int processor_number) {
}


void MESI::PrWr(ulong addr, int processor_number)
{
    cache_state state;
    current_cycle++;
    writes++;
    cache_line * line = find_line(addr);
    if (line == NULL || line->get_state() == I)
    {
                write_misses++;
                cache_line *newline = allocate_line(addr);
                I2M++;
                newline->set_state(M);
                if (sharers_exclude(addr,processor_number) == 0){
                        memory_transactions++;
                }
                else{
                        cache2cache++;
                }
                bus_readxs++;
                sendBusRdX(addr, processor_number);
     }
    else
        {
                state=line->get_state();
                if (state == M)
                {
                        update_LRU(line);
                }
                else if (state == S)
                {
                        S2M++;
                        line->set_state(M);
                        update_LRU(line);
                        bus_upgrades++;
                        sendBusUpgr(addr, processor_number);
                }
                else if (state == E)
                {
                        E2M++;
                        line->set_state(M);
                        update_LRU(line);
                }
         }
 }


void MESI::BusRd(ulong addr)
{
    cache_state state;
    cache_line * line=find_line(addr);
    if (line != NULL)
       {
                state = line->get_state();
                if (state == M)
                    {
                      flushes++;
                      write_backs++;
                      memory_transactions++;
                      interventions++;
                      line->set_state(S);
                      M2S++;
                    }
                   else if (state == E)
                   {
                      interventions++;
                      line->set_state(S);
                      E2S++;
                    }

        }
}

void MESI::BusRdX(ulong addr)
{
        cache_line * line=find_line(addr);
        cache_state state;
        if (line != NULL)
        {
                //cache_state state;
                state=line->get_state();
                if (state == S )
                {
                        invalidations++;
                        line->set_state(I);
                        //S2I++;
                }
                else if (state == M )
                {
                        flushes++; //ONLY FLUSH here
                        invalidations++;
                        write_backs++;
                        memory_transactions++;
                        line->set_state(I);
                        M2I++;
                }
                else if (state == E )
                {
                        invalidations++;
                        line->set_state(I);
                }
                else if (state == I)
                {
                        invalidations++;
                }

        }

}


void MESI::BusUpgr(ulong addr)
{
    cache_state state;
    cache_line* line = find_line(addr);

    if (line != NULL)
    {
	     state = line->get_state();
	     if(state == S)
       {
            line->set_state(I);
            invalidations++;
       }
	     else if(state == I)
	     {   invalidations++;}
    }
}



bool MESI::writeback_needed(cache_state state)
{
    if (state == M)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MESI::signalRd(ulong addr, int processor_number){


}


void MESI::signalRdX(ulong addr, int processor_number){

}

void MESI::signalUpgr(ulong addr, int processor_number){

}

void MESI::Int(ulong addr) {

}


void MESI::Inv(ulong addr) {

}
