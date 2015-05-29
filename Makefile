# Makefile for ParallelGt

all: countBlockingStates.cpp FSM.cpp ParCompUtilities.cpp TRWSpecialUtilities.cpp SmartStack.cpp SmartHeap.cpp DjikstraNode.cpp
	g++ countBlockingStates.cpp FSM.cpp ParCompUtilities.cpp TRWSpecialUtilities.cpp SmartStack.cpp SmartHeap.cpp DjikstraNode.cpp -I. -std=c++11 -o CountBlock
original: countBlockingStates_original.cpp FSM.cpp ParCompUtilities.cpp TRWSpecialUtilities.cpp SmartStack.cpp SmartHeap.cpp DjikstraNode.cpp
	g++ countBlockingStates_original.cpp FSM.cpp ParCompUtilities.cpp TRWSpecialUtilities.cpp SmartStack.cpp SmartHeap.cpp DjikstraNode.cpp -I. -std=c++11 -o CountBlock_original