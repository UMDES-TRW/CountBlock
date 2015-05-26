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

//copied from RawCountBlockingStates.cpp
float GetCommonalityOfEventSets(const vector<FSM> & automata)
{
  //Return 0 on empty set
  if(automata.size() == 0)
    return 0;
  else if(automata.size() == 1)
    return 1;

  //Get the union
  vector<string> event_union = GetEventUnionOfAllAutomata(automata);

  //Get the intersection
  vector<string> intersection = automata[0].GetAllEvents();
  for(int i=1; i<automata.size(); i++)
  {
    vector<string> events = automata[i].GetAllEvents();
    vector<string> result(intersection.size());
    set_intersection(intersection.begin(), intersection.end(), events.begin(), events.end(), result.begin());
    intersection = result;
  }

  //If the union is zero, return 0
  if(!event_union.size())
    return 0;

  //Return the ratio
  return (float)intersection.size() / (float) event_union.size();
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
/*
  //~~~~~~~~~~~~~~~~~ Add alias transitions to automata ~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  cout << "Adding alias transitions to automata..."; cout.flush();
  for(int i=0; i<automata.size(); i++)
  {
    AddDcAliasTransitions(automata[i]);
  }
  cout << "Success" << endl;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
*/
  timeval start, end;

  //~~~~~~~~~~~~~~~~~ Stage 1 Pairwaise State Space Reduction ~~~~~~~~~~~~~~~~~~~~~//
  //Hardcoding this for now...
  vector<FSM> subset;/*
  for(int i=0; i<automata.size(); i++)
  {
    if(!automata[i].Name.compare("da_v4") ||
       !automata[i].Name.compare("kivl_v3") ||
       !automata[i].Name.compare("lkp_v3") ||
       !automata[i].Name.compare("lp_v3") ||
       !automata[i].Name.compare("retry_v3") ||
       !automata[i].Name.compare("subsys3min") ||
       !automata[i].Name.compare("temp5min") ||
       !automata[i].Name.compare("ula_v4"))
    {
      subset.push_back(automata[i]);
    }
  }
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
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  //~~~~~~~~~~~~~~~~~ Stage 2 Pairwaise State Space Reduction ~~~~~~~~~~~~~~~~~~~~~~//
  //DDC Spec has been found to not be helpful here
  for(int i=0; i<automata.size(); i++)
  {
    if( !automata[i].Name.compare("temp2tmin") )
    {
      subset.push_back(automata[i]);
    }
  }

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
  Result2.Export("Stage2_ParCompAcc.fsm");

  subset.clear();
  subset.push_back(Result2);
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

  //~~~~~~~~~~~~~~~~~ On-the-fly ParComp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  for(int i=0; i<automata.size(); i++)
  {
    if( !automata[i].Name.compare("temp1trevC") )
    {
      subset.push_back(automata[i]);
    }
  }
  
*/
while(GetEventUnionOfAllAutomata(automata).size() > 4294967296) // 2^32
{
	int size = automata.size();	//tracks if any automata have been composed
	vector<float> common;	//keeps track of % difference between fsm pairs
	vector<FSM> comp;		//vector to compare a pair of automata
	for(int i = 0; i < automata.size(); i++)
	{
		for(int j = 0; j < automata.size(); j++)
		{
			if(i != j)
			{
				comp.push_back(automata.at(i));
				comp.push_back(automata.at(j));
				common.push_back(GetCommonalityOfEventSets(comp));
				comp.clear();
			}else{
				common.push_back(0.0);	//push back 0 when i = j to prevent automata from being composed with itself
			}
		}
		//find which automata have greater than 50% similarity to the ith 'automata'
		int offset = 0;	//keeps track of how many automata have been selected and deleted from 'automata'
		for(int k = 0; k < common.size(); k++)
		{
			if(common.at(k) > .5)	//adjust this value to change sensitivity
			{
				subset.push_back(automata.at(k - offset));
				//delete selected automata from 'automata'
				automata.erase(automata.begin() + k - offset);
				offset++;
			}
		}
		FSM result;
		ParCompAcc(subset, NULL, true, &result, true);
		automata.push_back(result);
		
		common.clear();
	}
	if(size == automata.size())
	{
		cout<<"Unable to sufficiently reduce state space!"<<endl<<"Exiting..."<<endl;
		return 1; 
	}
}

subset = automata;

/*
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
*/
  uint64_t InitialStateSpace = GetCartesianStateSpace(subset); //MODIFIED-add utin64_t

  cout << "Composing " << subset.size() << " automata (Max " << InitialStateSpace << " states)" << endl;
	
	bitset<MAX_NUMBER_OF_STATES_ALLOWED> * bits = new bitset<MAX_NUMBER_OF_STATES_ALLOWED>;	//ADDED

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
  
