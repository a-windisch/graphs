#  Dijkstra's shortest path and Jarnik-Prim minimum spanning tree
### course project 'C++ for C Programmers Part A', University of California Santa Cruz, provided through coursera 
### Andreas Windisch, andreas.windisch@yahoo.com 

## 1. Files contained in this repository

- README.md - This file.
- Makefile - Makefile to produce the executables
- shortes_path.cpp - This program implements Dijkstra's shortes path and demonstrates the algorithm by applying it to an example. 
- minimum_spanning_tree.cpp - This program implements the Jarnik-Prim algorithm for computing the MST for a (connected, undirected) graph. 
- mst_data.dat - A datafile containing a graph. This file is used by the MST program to compute the MST on this graph.

## 2. Short description of the project

This is a naive implementation of Dijkstra's shortest path and the Jarnik-Prim minimum spanning tree. I also started implementing Kruskal's algorithm, but the implementation is incomplete. Feel free to use to code as you please, EXCEPT for getting course credit on coursera. If you have any questions regarding this problem, feel free to contact me using andreas.windisch (at) yahoo.com.

## 3. Building the program

On a linux machine, just open a console, cd into the directory where the files are strored, and run:
```bash
$ make
```
This should produce two executables, 'shortest_path' and 'minimum_spanning_tree'.
To run the two (independent) programs type:
```bash
$ ./shortest_path
```
and/or
```bash
$ ./minimum_spanning_tree
```



