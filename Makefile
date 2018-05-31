#=========================================
#=   Makefile for shortest path and MST  =
#=---------------------------------------= 
#=         by Andreas Windisch           =
#=              May 2018                 =
#=========================================

CPP       = g++
CPPFLAGS  = -std=c++11 -Wall -O3 

default:	all 

shortest_path: shortest_path.cpp
	$(CPP) $(CPPFLAGS) $^ -o $@ 

minimum_spanning_tree: minimum_spanning_tree.cpp
	$(CPP) $(CPPFLAGS) $^ -o $@

all: shortest_path minimum_spanning_tree
   

clean: 
	rm shortest_path
	rm minimum_spanning_tree
