#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <algorithm>
#include <stack>
#include <bitset>
#include <cmath>
#include <string.h>
#include <sys/time.h>

#include "FSM.h"
#include "ParCompUtilities.h"
#include "TRWSpecialUtilities.h"

using namespace std;

static bool verboseFlag;

bool compareFSM(FSM lhs, FSM rhs)
{
  return lhs.GetAllEvents().size() > rhs.GetAllEvents().size();
}

int main(int argc, char * argv[])
{
  //~~~~~~~~~~~~~~~~~~~~~~ Check inputs and set flags ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  if(argc <= 1) 
  {
    cout << "main: Format is: <executable> [-v] [-h] <file1.fsm> <file2.fsm> ... Exiting application." << endl;
    return 1;
  }
  
  //Set flags
  int nextArg = 1;
  for(; nextArg<argc; nextArg++)
  {
    if(argv[nextArg][0] != '-')
      break;
    else if(!strcmp("-v", argv[nextArg])) //Verbose output
      verboseFlag = true;
    else if(!strcmp("-h", argv[nextArg]))
    {
      cout << "Format is: <executable> [-v] [-h] <file1.fsm> <file2.fsm> ..." << endl;
      cout << "Flags:" << endl;
      cout << "\t-v: Verbose output. Print everything." << endl;
      cout << "\t-h: Print this help menu and exit." << endl;
      cout << "Exiting application." << endl;
      return 0;
    }
    else
    {
      cout << "main: " << argv[nextArg] << " is not a valid option" << endl;
      cout << "main: Set flag -h for command line information. Exiting application." << endl;
      return 1;
    }
  }   
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  vector<FSM> automata;

  //~~~~~~~~~~~~~~~~~~ Read in automata ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  cout << "Importing " << (argc - nextArg) << " .fsm files..."; cout.flush();
  for(; nextArg<argc; nextArg++)
  {
    string filepath(argv[nextArg]);
    automata.push_back(FSM(filepath));
  }
  cout << "Success" << endl;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  //~~~~~~~~~~~~~~~~~ Add alias transitions to automata ~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  cout << "Adding alias transitions to automata..."; cout.flush();
  for(int i=0; i<automata.size(); i++)
  {
    AddDcAliasTransitions(automata[i]);
  }
  cout << "Success" << endl;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


  //organize from smallest number of events to largest number of events
  sort(automata.begin(), automata.end(), compareFSM);
  timeval start, end;

  //~~~~~~~~~~~~~~~~~ Stage 1 Pairwaise State Space Reduction ~~~~~~~~~~~~~~~~~~~~~//
  //Hardcoding this for now...
  vector<FSM> subset;
  /*
  for(int i=0; i<automata.size(); i++)
  {
    if(
      !automata[i].Name.compare("G1_t") ||
      !automata[i].Name.compare("G2_t") ||
      !automata[i].Name.compare("G3_t") ||
      !automata[i].Name.compare("G5_t") ||
    !automata[i].Name.compare("deadlockFSM")
    !automata[i].Name.compare("bsp") ||
      !automata[i].Name.compare("cmd_t") ||
      !automata[i].Name.compare("da_v4") ||
      !automata[i].Name.compare("ddl_v6") ||
      !automata[i].Name.compare("ddwa") ||
      !automata[i].Name.compare("dsbs") ||
      !automata[i].Name.compare("fpr_ntfr") ||
      !automata[i].Name.compare("fs_v4") || 
      !automata[i].Name.compare("lp_v3") ||
      !automata[i].Name.compare("pdl_v6-notypo") ||
      !automata[i].Name.compare("pp_v3") ||
      !automata[i].Name.compare("igoff_v4") ||
      !automata[i].Name.compare("kiv_v1") ||
      !automata[i].Name.compare("kivl_v3") ||
      !automata[i].Name.compare("kw_v3") ||
      !automata[i].Name.compare("lkp_v3") ||
      !automata[i].Name.compare("lkps_v3") ||
      !automata[i].Name.compare("retry_v3") ||
      !automata[i].Name.compare("sbp") ||
      !automata[i].Name.compare("ula_v4") ||
      !automata[i].Name.compare("wlkfs_v3") ||
      !automata[i].Name.compare("wlkm_v4") ||
      !automata[i].Name.compare("wlkp_v4") ||
      !automata[i].Name.compare("wlks_v4"))

    {
      //subset.push_back(automata[i]);
    }
  }
  if(IsDDCSpecUseful(subset))
  {
    //subset.push_back(CreateDDCSpecialAutomaton(subset));
    //cout << "DDC spec is useful and has been added." << endl;
  }
  else
    //cout << "DDC spec is not useful and will not be used." << endl;
  if(IsFaultSpecUseful(subset))
  {
    //subset.push_back(CreateFaultSpecialAutomaton(subset));
    //cout << "Fault spec is useful and has been added." << endl;
  }
  else
    //cout << "Fault spec is not useful and will not be used." << endl;
  //Accessibility search
  uint64_t InitialStateSpace = GetCartesianStateSpace(subset);
  cout << "Composing " << subset.size() << " automata (Max " << InitialStateSpace << " states)" << endl;
  FSM Result1;
  bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits = new bitset<MAX_NUMBER_OF_STATES_ALLOWED>;
  gettimeofday(&start, NULL);
  uint32_t numStates = ParCompAcc(subset, bits, true, &Result1, true);
  gettimeofday(&end, NULL);
  float reduction = 100 - (numStates * 100 / InitialStateSpace);
  float rate = (float)numStates/((end.tv_sec + end.tv_usec/1000000) - (start.tv_sec + start.tv_usec/1000000));
  cout << "No special rules: " << numStates << " states (" << reduction << "% reduction) [" << rate <<" states/sec]" << endl;

  //Export result
  Result1.Export("Stage1_ParCompAcc.fsm");

  subset.clear();
  subset.push_back(Result1);
  */
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  //~~~~~~~~~~~~~~~~~ Stage 2 Pairwaise State Space Reduction ~~~~~~~~~~~~~~~~~~~~~~//
  //DDC Spec has been found to not be helpful here
  subset.push_back(automata[0]);
  uint64_t InitialStateSpace;
  bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits = new bitset<MAX_NUMBER_OF_STATES_ALLOWED>;
  uint32_t numStates;
  float reduction;
  float rate;
  for(int i=1; i<automata.size(); i++)
  {
    cout<<"Stage "<<i<<endl;
    
      subset.push_back(automata[i]);
    
  
  if(IsDDCSpecUseful(subset))
  {
    //subset.push_back(CreateDDCSpecialAutomaton(subset));
    //cout << "DDC spec is useful and has been added." << endl;
  }
  else
    cout << "DDC spec is not useful and will not be used." << endl;
  if(IsFaultSpecUseful(subset))
  {
    //subset.push_back(CreateFaultSpecialAutomaton(subset));
    //cout << "Fault spec is useful and has been added." << endl;
  }
  else
    cout << "Fault spec is not useful and will not be used." << endl;

  //Print update header
  InitialStateSpace = GetCartesianStateSpace(subset);
  cout << "Composing " << subset.size() << " automata (Max " << InitialStateSpace << " states)" << endl;

  gettimeofday(&start, NULL);
  FSM Result2;
  bits->reset();
  numStates = ParCompAcc(subset, bits, true, &Result2, true);
  gettimeofday(&end, NULL);
  //Print stats
  reduction = 100 - (numStates * 100 / InitialStateSpace);
  rate = (float)numStates/((end.tv_sec + end.tv_usec/1000000) - (start.tv_sec + start.tv_usec/1000000));
  cout << "No special rules: " << numStates << " states (" << reduction << "% reduction) [" << rate <<" states/sec]" << endl;

  //Export result
  //Result2.Export("Stage2_ParCompAcc.fsm");

  subset.clear();
  subset.push_back(Result2);
}
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  //~~~~~~~~~~~~~~~~~ On-the-fly ParComp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  for(int i=0; i<automata.size(); i++)
  {
    if(0 == 1)
    {
      subset.push_back(automata[i]);
    }
  }

  if(IsDDCSpecUseful(subset))
  {
    subset.push_back(CreateDDCSpecialAutomaton(subset));
    cout << "DDC spec is useful and has been added." << endl;
  }
  else
    cout << "DDC spec is not useful and will not be used." << endl;
  if(IsFaultSpecUseful(subset))
  {
    subset.push_back(CreateFaultSpecialAutomaton(subset));
    cout << "Fault spec is useful and has been added." << endl;
  }
  else
    cout << "Fault spec is not useful and will not be used." << endl;

  InitialStateSpace = GetCartesianStateSpace(subset);

  cout << "Composing " << subset.size() << " automata (Max " << InitialStateSpace << " states)" << endl;


  bits->reset();
  gettimeofday(&start, NULL);
  uint32_t numStatesAccessed = ParCompAcc(subset, bits, true, NULL, true);
  gettimeofday(&end, NULL);

  //Print stats
  float percent= ((float)numStates * 100 / (float)InitialStateSpace);
  rate = (float)numStatesAccessed/((end.tv_sec + end.tv_usec/1000000) - (start.tv_sec + start.tv_usec/1000000));
  cout << numStatesAccessed << " states accessed (" << percent << "%) [" << rate <<" states/sec]" << endl;

  //Print accessibility bitfile  
  cout << "Writing DFS Acc results to file..." << endl;
  string accfile("Stage3_AccBits.txt");
  BitSetToFile(*bits, InitialStateSpace, accfile);
  cout << "Success." << endl;

/*
  //Get bits from file
  bits->reset();
  string accfile("parCompAcc.txt");
  cout << "Reading in " << InitialStateSpace << " bits from " << accfile << "..."; cout.flush();
  FileToBitSet(*bits, InitialStateSpace, accfile);
  cout << "Success." << endl;
  cout << bits->count() << " states are accessible." << endl;

  //Get bits from file
  bits->set();
  string coaccfile("parCompCoAcc.txt");
  cout << "Reading in " << InitialStateSpace << " bits from " << coaccfile << "..."; cout.flush();
  FileToBitSet(*bits, InitialStateSpace, coaccfile);
  cout << "Success." << endl;
  cout << bits->count() << " states are not accessible or are coaccessible." << endl;
*/


  //Perform coaccessibility search
  bits->flip();
  gettimeofday(&start, NULL);
  uint32_t numStatesCoaccessed = ParCompCoAcc(subset, bits, true);
  gettimeofday(&end, NULL);

  //Print coaccessibility bitfile  
  cout << "Writing DFS CoAcc results to file..." << endl;
  string coaccfile("Stage3_CoAccBits.txt");
  BitSetToFile(*bits, InitialStateSpace, coaccfile);
  cout << "Success." << endl;

  //Count blocking states
  bits->flip();
  cout << bits->count() << " states are blocking." << endl;

  //Perform the accessibility search once again, printing the transitions to blocking states
  cout << "Performing search of non-blocking space..." << endl;
  ofstream blockout("blocking-events-full.txt");
  cout << FindBlockingTransitions(subset, bits, true, NULL, blockout) << " blocking transitions found" << endl;
  blockout.close(); 
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  return 0;
}
  
