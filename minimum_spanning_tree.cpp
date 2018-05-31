/////////////////////////////////////////////////
// Program minimum spanning tree               //
// by Andreas Windisch                         //
// andreas.windisch@yahoo.com                  //
// Implemented for coursera online course      //
// 'C++ for C Programmers, Part A'             //
/////////////////////////////////////////////////  

/////////////////////////////////////////////////////////////////////////////////////////
// Warning    : This program is my original work, so please do not sell it as yours    //
// for getting credit in the online coursera course 'C++ for C Programmers'.           //
// However, feel free to use this code for any other purpose. Andreas Windisch, 2018.  //
/////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>  //C++ IO
#include <vector>    //used for adjacency list and priority queue
#include <cstdlib>   //needed for random number generator
#include <ctime>     //used for seeding the random number generator
#include <algorithm> //used for sorting and finding elements in vectors
#include <fstream> //used for reading in data file
#include <iterator> //used for reading in data file

using namespace std;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> global aux function <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
inline double rnd( const double* range) //aux function for random number in range
{
   double t = (double)rand() / RAND_MAX;
   return range[0] + t * (range[1] - range[0]);
}
//-----------------------------------------------------------------------------------------
ostream& operator<<(ostream& out, vector<int> v) //overload to output a vector of ints
{
   for( vector<int>::size_type i=0; i != v.size(); i++  )
   {
      out << " " << v[i] << " ";
   }
}
//-----------------------------------------------------------------------------------------
struct path_solution //this struct contains the solution of a shortest path with its costs
{
   int start; //start vertex for path
   int end; //end vertex for path
   vector<int> path; //solution of shortest path
   double cost; //cost for path 
};
//-----------------------------------------------------------------------------------------
struct queue_element // this structure is used for the priority queue
{
   int start;
   int end;
   double weight;
};
//-----------------------------------------------------------------------------------------
struct by_weight //used for sorting within the priority queue
{
   bool operator()(queue_element const &a, queue_element const &b)
   {   
      return a.weight < b.weight;
   } 
};
//-----------------------------------------------------------------------------------------
bool match_element( queue_element qe1, queue_element qe2) //needed to compare two queue elements
{
	return qe1.start == qe2.start && qe1.end == qe2.end && qe1.weight == qe2.weight;
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> class  graph <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//NB: I did not define a default constructor for this class
class graph  //this class implements the generation of, as well as operations on, graphs
{
   public:
      graph(int n); //constructor for disconnected graph with n vertices
      graph(int n, double dnsty, const double* range); //constructor random graph with n vertices
      graph(ifstream &data); //constructor reads in graph from file
      int V; //number of vertices of a graph
      int E; //number of edges in a graph
      void print(); //print a graph
      bool adjacent(int i, int j); //return T if vertices i and j have an edge connecting them
      void add(int i, int j, double r); //add edge between vertices i and j (if it doesn't exist)
      void del(int i, int j); //remove edge between vertices i and j (if it exists)
      vector<int> neighbors(int i); //return list of all nodes directly connected to vertex i
      double get_node_value(int i); //return value associated with node i
      void set_node_value(int i, double val); //set value associated with node i
      double get_edge_value(int i, int j); //return value associated with edge from i to j
      void set_edge_value(int i, int j, double val); //set value associated with edge from i to j
   private:
      vector< vector<int> > adjacency_lst; //adjacency list representation of graph
      vector<double> vertex_values; //can assign weights to nodes
      vector< vector<double> > edge_values; //can assign weights to edges
};
//-----------------------------------------------------------------------------------------
ostream& operator<<(ostream& out, const graph& g) //overload the operator for outstream
{
  out << "number of vertices: " << g.V << ", number of edges: " << g.E;
}
//-----------------------------------------------------------------------------------------
graph::graph(int n)  //constructor for disconnected graph of n vertices
{
   V = n; E = 0;     //set vertices to n, but there are no edges
   vector<int> empty_ints; //empty int vector 
   vector<double> empty_dbls; //empty double vector 
   for( int i=0; i<n; ++i) //construct adjecency list
   {
      vertex_values.push_back(rand()); //randomly add a weight to each vertex
      adjacency_lst.push_back(empty_ints); //no edges in graph
      edge_values.push_back(empty_dbls); //no edges in graph
   }
}
//-----------------------------------------------------------------------------------------
graph::graph(ifstream &data)  //constructor for reading in graph from file
{
  istream_iterator<int> start(data),end; //iterator for data
  vector<int> datav(start,end);
  V = datav.front(); //read number of vertices 
  vector<int> empty_ints; //empty int vector 
  vector<double> empty_dbls; //empty double vector 
  for( int i=0; i<V; ++i) //construct adjecency list
  {
     vertex_values.push_back(0.0); //add zero weight to each vertex
     adjacency_lst.push_back(empty_ints); //no edges in graph yet
     edge_values.push_back(empty_dbls); //no edges in graph yet
  }
  cout << "Number of vertices: " << V << endl;
  for( vector<int>::iterator p=datav.begin()+1; p!=datav.end(); p+=3)
  {
     //cout << "Adding: " << *p << ", " << *(p+1) << ", " << double(*(p+2)) << endl;
     add(*p,*(p+1),double(*(p+2))); //add edge to graph
     if(!adjacent(*(p+1),*p)) E++; //increase edge count if edge doesn't exist 
  }
}
//-----------------------------------------------------------------------------------------
graph::graph(int n, double dnsty, const double* range) //constructor for random graph
{
   V = n; E = 0;     //set vertices to n, but there are no edges yet
   vector<int> temp_E; //temp vector, containing edges from i to other vertices
   vector<double> temp_E_val; //temp vector for weights associated with edges
  
   double r; //used to store random number
   for( int i=0; i<n; ++i) //construct adjecency list: loop over vertices
   {
      for( int j=0; j<n; ++j) //for each vertex i, loop over all vertices 
      {
         if( j < i ) //check previously generated edges and add if needed
         {
            //turn for loop iterator j into an appropriate iterator for a vector of int-vectors
            vector< vector<int> >::size_type jv = static_cast<vector< vector<int> >::size_type>(j);
            for( vector<int>::size_type k=0; k != adjacency_lst[jv].size(); k++  ) //go through existing edges 
            { 
              if( adjacency_lst[jv][k] == i ) //edge to vertex j exists
              {
                 //cout << "found vertex j = " << j << " to be connected with vertex i = " << i << endl;
                 temp_E.push_back(j); //graph is undirected, thus we have to add the vertex
                 temp_E_val.push_back(edge_values[jv][k]); //add associated weight 
              }
            }
         }
         
         if( j > i ) //generate new random connections
         {
            r = ((double) rand() / (RAND_MAX));
            //cout << "probability r: " << r << endl;
            if( r < dnsty )
            {
               //cout << "accepting value of j = " << j << " for vertex i = " << i << endl;
               temp_E.push_back(j); //add vertex j to adjacency list
               temp_E_val.push_back(rnd(range)); //add random weight to edge
               ++E; //increment edge count
            }
         }

      }

      r = ((double) rand() / (RAND_MAX)); //choose random weight for vertex , r e [0,1]
      adjacency_lst.push_back(temp_E); //add edges of vertex i to adjacency list
      edge_values.push_back(temp_E_val); //add weights of edges for vertex i
      vertex_values.push_back(r); //randomly add a weight to each vertex
      temp_E.clear();  //clear temporary edge list
      temp_E_val.clear();  //clear temporary edge weight list
   }

   //print(); //print the generated adjacency list
 
}
//-----------------------------------------------------------------------------------------
void graph::print() //This method prints the adjacency list of a graph
{
   cout << endl;
   for( vector< vector<int> >::size_type i=0; i != adjacency_lst.size(); i++  )
   {
      cout << "vertex " << i << ": ";
      for( vector<int>::size_type j=0; j != adjacency_lst[i].size(); j++  )
      {
         cout << "\t" << adjacency_lst[i][j] << "(" << edge_values[i][j] << ")";
      }
      cout << endl;
   }
   cout << endl; 
}
//-----------------------------------------------------------------------------------------
bool graph::adjacent(int i, int j) //return T if there is an edge btwn. vertices i and j
{
   //turn i into an appropriate iterator for a vector of int-vectorsi
   vector< vector<int> >::size_type iv = static_cast<vector< vector<int> >::size_type>(i);  
   if( !adjacency_lst.empty() )
   {  
      if( !adjacency_lst[iv].empty() ) //ensure that vector list is not empty
      {
         for( vector<int>::size_type jv=0; jv != adjacency_lst[iv].size(); jv++  )
         {
            if( adjacency_lst[iv][jv] == j )  //check if vertex j is in list of vertex i
            {
               return true;
            }
         }
      }
   }
   return false;
}
//-----------------------------------------------------------------------------------------
void graph::add(int i, int j, double r) //add edge between vertices i and j (if it doesn't exist)
{
   if ( !adjacent(i,j) ) //if edge doesn't exist
   {
      //turn i and j into an appropriate iterator for a vector of int-vectors
      vector< vector<int> >::size_type iv = static_cast<vector< vector<int> >::size_type>(i);
      vector< vector<int> >::size_type jv = static_cast<vector< vector<int> >::size_type>(j);
      adjacency_lst[iv].push_back(j);  //add edge i to j to adjacency list
      adjacency_lst[jv].push_back(i);  //add edge j to i to adjacency list
      edge_values[iv].push_back(r);    //add edge value to list
      edge_values[jv].push_back(r);    //add edge value to list
      ++E; 
   }
}
//-----------------------------------------------------------------------------------------
void graph::del(int i, int j) //remove edge between vertices i and j (if it exists)
{
   if ( adjacent(i,j) ) //if edge exists
   {
      //turn i and j into an appropriate iterator for a vector of int-vectors
      vector< vector<int> >::size_type iv = static_cast<vector< vector<int> >::size_type>(i);
      vector< vector<int> >::size_type jv = static_cast<vector< vector<int> >::size_type>(j);
      vector<int>::iterator val; //vertex j to be removed from list for vertex i
      int pos; //position in the vector
      val = find( adjacency_lst[iv].begin(), adjacency_lst[iv].end(),j); //find the vertex value in vector
      pos = distance (adjacency_lst[iv].begin(), val); //find the position of the vertex
      adjacency_lst[iv].erase(adjacency_lst[iv].begin() + pos); //remove the vertex j from vertex list i 
      edge_values[iv].erase(edge_values[iv].begin() + pos); //also remove the edge value  
      val = find( adjacency_lst[jv].begin(), adjacency_lst[jv].end(),i); //same as above, but with roles i, j swapped
      pos = distance (adjacency_lst[jv].begin(), val);
      adjacency_lst[jv].erase(adjacency_lst[jv].begin() + pos);  
      edge_values[jv].erase(edge_values[jv].begin() + pos);  
      --E; //decrement number of edges
   }
}
//-----------------------------------------------------------------------------------------
vector<int> graph::neighbors(int i) //return list of all nodes directly connected to vertex i
{
   //turn i into an appropriate iterator for a vector of int-vectors
   vector< vector<int> >::size_type iv = static_cast<vector< vector<int> >::size_type>(i);
   return adjacency_lst[iv]; //return the entries of adjacency list for vertex i  
}
//-----------------------------------------------------------------------------------------
double graph::get_node_value(int i) //return value associated with node i
{
   //turn i into an appropriate iterator for a vector
   vector<double>::size_type iv = static_cast<vector<double>::size_type>(i);
   return vertex_values[iv]; //return value associated with vertex i
}
//-----------------------------------------------------------------------------------------
void graph::set_node_value(int i, double val) //set value associated with node i
{
   //turn i into an appropriate iterator for a vector
   vector<double>::size_type iv = static_cast<vector<double>::size_type>(i);
   vertex_values[iv] = val; //return value associated with vertex i
}
//-----------------------------------------------------------------------------------------
double graph::get_edge_value(int i, int j) //return value associated with edge from i to j
{
   if ( adjacent(i,j) ) //if edge exists
   {
      //turn i and j into an appropriate iterator for a vector of int-vectors
      vector< vector<int> >::size_type iv = static_cast<vector< vector<int> >::size_type>(i);
      vector< vector<int> >::size_type jv = static_cast<vector< vector<int> >::size_type>(j);
      vector<int>::iterator val; //vertex number for search
      int pos; //position in the vector
      val = find( adjacency_lst[iv].begin(), adjacency_lst[iv].end(),j); //find the vertex value in vector
      pos = distance (adjacency_lst[iv].begin(), val); //find the position of the vertex
      vector< vector<int> >::size_type jd = static_cast<vector< vector<int> >::size_type>(pos);
      return edge_values[iv][jd];  
   }
   else
   {
      cout << endl << "Edge i=" << i << " --> j=" << j << " does not exist." << endl; 
      return 0.0;
   }
}
//-----------------------------------------------------------------------------------------
void graph::set_edge_value(int i, int j, double eval) //set value associated with edge from i to j
{
   if ( adjacent(i,j) ) //if edge exists
   {
      //turn i and j into an appropriate iterator for a vector of int-vectors
      vector< vector<int> >::size_type iv = static_cast<vector< vector<int> >::size_type>(i);
      vector< vector<int> >::size_type jv = static_cast<vector< vector<int> >::size_type>(j);
      vector<int>::iterator val; //vertex number for search
      int pos; //position in the vector
      val = find( adjacency_lst[iv].begin(), adjacency_lst[iv].end(),j); //find the vertex value in vector
      pos = distance (adjacency_lst[iv].begin(), val); //find the position of the vertex
      vector< vector<int> >::size_type vpos = static_cast<vector< vector<int> >::size_type>(pos);
      edge_values[iv][vpos] = eval; //set new edge value in list for vertex i
      val = find( adjacency_lst[jv].begin(), adjacency_lst[jv].end(),i); //find the vertex value in vector
      pos = distance (adjacency_lst[jv].begin(), val); //find the position of the vertex
      vpos = static_cast<vector< vector<int> >::size_type>(pos);
      edge_values[jv][vpos] = eval; //set new edge value in list for vertex j
   }
   else
   {
      cout << endl << "Edge i=" << i << " --> j=" << j << " does not exist." << endl; 
   }
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> class prq <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
class prq //class priority queue
{
   public:
      void insert(queue_element a); //insert a queue element into queue
      int queue_size(); //number of queue elements in queue
      queue_element top(); //returns the top element of the queue
      bool contains(queue_element a); //returns T if queue_element is an element of queue
      bool contains_end_pt(int a); //returns T if queue contains vertex a as an end point
      bool contains_start_pt(int a); //returns T if queue contains vertex a as a start point
      void rmv_top(); //removes top element from queue
      void queue_clear(); //clear all elements in queue
      void change_priority(queue_element a, double priority); //change the priority of a queue_element
      int get_previous(int a); //retrieve priviously visited node
      int get_next(int a); //retrieve next node
      double get_weight_end_node(int a); //retrieve costs for an end node
      void remove_element(queue_element a); //remove a queue_element

   private:
      vector<queue_element> queue;
};
//-----------------------------------------------------------------------------------------
void prq::insert( queue_element a)
{
   queue.push_back(a); //add element to queue
   sort(queue.begin(), queue.end(), by_weight()); //re-arrange queue
}
//-----------------------------------------------------------------------------------------
int prq::queue_size( )
{
   return queue.size();
}
//-----------------------------------------------------------------------------------------
queue_element prq::top()
{
   return queue.front();
}
//-----------------------------------------------------------------------------------------
bool prq::contains( queue_element a)
{
   vector<queue_element> lst;
   lst.push_back(a);
   vector<queue_element>::iterator pos = search(queue.begin(),queue.end(),lst.begin(),lst.end(),match_element);
   if(pos != queue.end()) //check if element is contained
   {
      return true;
   }
   else
   {
      return false;
   }
}
//-----------------------------------------------------------------------------------------
bool prq::contains_end_pt( int a)
{
   for( vector<queue_element>::size_type iv=0; iv != queue.size(); iv++  )
   {
      if( queue[iv].end == a ) //check if end point is contained in queue
      {
         return true;
      } 
   }
   return false;
}
//-----------------------------------------------------------------------------------------
bool prq::contains_start_pt( int a)
{
   for( vector<queue_element>::size_type iv=0; iv != queue.size(); iv++  )
   {
      if( queue[iv].start == a ) //check if start point is contained in queue
      {
         return true;
      } 
   }
   return false;
}
//-----------------------------------------------------------------------------------------
int prq::get_previous( int a ) //get previous node for a given vertex
{
   for( vector<queue_element>::size_type iv=0; iv != queue.size(); iv++  )
   {
      if( queue[iv].end == a ) //check if end point is contained in queue
      {
         return queue[iv].start; 
      }
   }
   cout << " Requested node is not contained in this queue. " << endl;
   return 0;
}
//-----------------------------------------------------------------------------------------
int prq::get_next( int a ) //get next node for a given vertex
{
   for( vector<queue_element>::size_type iv=0; iv != queue.size(); iv++  )
   {
      if( queue[iv].start == a ) //check if end point is contained in queue
      {
         return queue[iv].end; 
      }
   }
   cout << " Requested node is not contained in this queue. " << endl;
   return 0;
}
//-----------------------------------------------------------------------------------------
double prq::get_weight_end_node( int a ) //get costs of end node
{
   for( vector<queue_element>::size_type iv=0; iv != queue.size(); iv++  )
   {
      if( queue[iv].end == a ) //check if end point is contained in queue
      {
         return queue[iv].weight; 
      }
   }
   cout << " Requested node is not contained in this queue. " << endl;
   return 0;
}
//-----------------------------------------------------------------------------------------
void prq::rmv_top()
{
   queue.erase(queue.begin());
}
//-----------------------------------------------------------------------------------------
void prq::queue_clear()
{
   queue.clear();
}
//-----------------------------------------------------------------------------------------
void prq::change_priority(queue_element a, double  priority) //change the priority of a queue_element
{
   if( contains(a) )
   {
      vector<queue_element> lst; //find element in queue
      lst.push_back(a);
      queue_element b;
      b.start = a.start;
      b.end = a.end;
      b.weight = priority;
      vector<queue_element>::iterator iv = search(queue.begin(),queue.end(),lst.begin(),lst.end(),match_element);
      queue.erase(iv);
      queue.push_back(b);    
      sort(queue.begin(), queue.end(), by_weight()); //re-arrange queue
   }
   else
   {
      //cout << "The element is not contained in this queue." << endl; 
   }   
}
//-----------------------------------------------------------------------------------------
void prq::remove_element(queue_element a) //remove a queue_element
{
   if( contains(a) )
   {
      vector<queue_element> lst; //find element in queue
      lst.push_back(a);
      vector<queue_element>::iterator iv = search(queue.begin(),queue.end(),lst.begin(),lst.end(),match_element);
      queue.erase(iv);
      sort(queue.begin(), queue.end(), by_weight()); //re-arrange queue
   }
   else
   {
      //cout << "The element is not contained in this queue." << endl;
   }
}
//-----------------------------------------------------------------------------------------
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> class shortest path <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
class shortest_path //class path for computing the shortest path
{
   public: 
      shortest_path(){}; //default constructor
      path_solution path(int s, int t, graph G); //find the shortest path between s and t of graph g
           
   private: 
            
};
//-----------------------------------------------------------------------------------------
path_solution shortest_path::path(int s, int t, graph G)
{
   prq open_set; //create an open set queue using the class priority queue
   prq closed_set; //create a closed set queue using the class priority queue
   vector<int> neighbors; //neighbors of a given vertex
   vector<int> final_path; //list of vertices for shortest path
   queue_element qe; //queue element
   queue_element en; //expansion node
   path_solution psol; //final solution 
   
   //Dijkstra's Shortest Path
   qe.start = s; //starting point
   qe.end = s; //end point
   qe.weight = 0.0; //no weight for vertex
   en = qe; //set expansion node
   closed_set.insert(qe); //put starting point in closed set
   neighbors = G.neighbors(s); //find all naeighbors of starting point s

   for( vector<int>::size_type iv=0; iv != neighbors.size(); iv++)  //add to open set
   {
     qe.start = s; //set expansion node
     qe.end = neighbors[iv]; //current neigbor is end point
     qe.weight = G.get_edge_value(s,qe.end);//load weight of edge from s to current neighbor
     open_set.insert(qe); //add element to open set
   }

   while(1) //loop till termination
   {  
      if ( open_set.queue_size() == 0 ) //no more successors -> no path
      {
         final_path.clear(); //return empty vector, since there is no path
         cout << "No path found from s = " <<  s << " to t = " << t << "." << endl;
         break;
      }
 
      qe = open_set.top(); //get top element from queue 
      en = qe; //set expansion node
      open_set.rmv_top(); //remove top element from queue
      closed_set.insert(qe); //add former top element to closed set

      if( qe.end == t  ) //stop, once target vertex has been moved to closed set
      {
         //cout << "found path from s = " << s << " to t = " << t << " with costs " << qe.weight << "." << endl; 
         break;
      }

      neighbors = G.neighbors(en.end); //find all neighbors of expansion node
      if( neighbors.size() > 0 )
      { 
         qe.start = en.end; //set expansion node as starting point for new queue element
         for( vector<int>::size_type iv=0; iv != neighbors.size(); iv++)  //add to open set
         {
            qe.end = neighbors[iv]; //load vertex into queue element
            qe.weight = en.weight + G.get_edge_value(qe.start,neighbors[iv]);//load weight of edge
            if( !closed_set.contains_end_pt(qe.end) )
            {
               open_set.insert(qe); //add element to open set
            }
         }
      }
   }

   //while(closed_set.queue_size()>0) //this while loop prints the content of the closed set and destroys it
   //{
   //   qe = closed_set.top();
   //   closed_set.rmv_top();
   //   cout << "prev: " << qe.start <<  " node: " << qe.end << " weight from s: " << qe.weight << endl;
   //}

   int node = t;
   final_path.push_back(node); 
   while( closed_set.get_previous(node) != s )
   {
      node=closed_set.get_previous(node);
      final_path.push_back(node); 
   }
   final_path.push_back(s); 
   reverse(final_path.begin(), final_path.end());
   psol.start = s;
   psol.end = t;
   psol.path = final_path;
   psol.cost = closed_set.get_weight_end_node(t); 

   return psol;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> class minimal spanning tree <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
class minimal_spanning_tree
{
   public:
      minimal_spanning_tree(){}; //default constructor 
      double mst_kruskal(graph G); //compute minimal spanning tree (Kruskal)
      double mst_jarnik_prim(graph G); //compute minimal spanning tree (Jarnik-Prim)

   private:
};
//-----------------------------------------------------------------------------------------
double minimal_spanning_tree::mst_kruskal(graph G) //!!!routine is incomplete!!!under construction!!!
{
   prq open_set; //create an open set queue using the class priority queue
   prq closed_set; //create a closed set queue using the class priority queue
   queue_element qe; //queue element
   queue_element qer; //reversed queue element (inverse link)
   vector<int> neighbors; //neighbors of a given vertex
   double result; //result of mst calculation
   
   //Kruskal's Minimal Spanning Tree
   for(int i=0; i<G.V; ++i) //loop over all vertices of the graph
   {
      neighbors = G.neighbors(i); //find all naeighbors for vertex i
      for( vector<int>::size_type iv=0; iv != neighbors.size(); iv++)  //add to open set
      {
         qe.start = i; //set start vertex
         qe.end = neighbors[iv]; //current neigbor is end point
         qe.weight = G.get_edge_value(qe.start,qe.end);//load weight of edge from s to current neighbor
         qer.start = qe.end;//reversed edge
         qer.end = qe.start;
         qer.weight = qe.weight;
         if( !open_set.contains(qe) && !open_set.contains(qer)) //if element or reversed element is not in queue 
         {
            open_set.insert(qe); //add element to open set
         }
      }
   }
   //up to this point we have created a priority queue with all edges in the graph
   //now we have to collect the top elements from the list until  all vertices are
   //accounted for
   
   result = 0.0; //reset value for MST
   while( open_set.queue_size() > 0 ) //loop over all prioritized edges
   {
      qe = open_set.top(); //take top element
      //if( !closed_set.forms_loop(qe) ) //add element if there is no closed loop. this part is incomplete
      {
         closed_set.insert(qe);
         result += qe.weight;
         cout << "qe: st = " << qe.start << ", ed = " << qe.end << ", weight = " << qe.weight << endl; 
      }
      open_set.rmv_top();
   }
   return result;      
}
//-----------------------------------------------------------------------------------------
double minimal_spanning_tree::mst_jarnik_prim(graph G)
{
   prq open_set; //create an open set queue using the class priority queue
   prq closed_set; //create a closed set queue using the class priority queue
   vector<int> neighbors; //neighbors of a given vertex
   vector<int> final_tree; //list of vertices for shortest path
   queue_element qe; //queue element
   queue_element qr; //reversed element
   int s = 0; //starting point
   double result = 0.0; //return value for mst costs

   neighbors = G.neighbors(s); //find all neighbors of starting point s

   for( vector<int>::size_type iv=0; iv != neighbors.size(); iv++)  //add to open set
   {
     qe.start = s; //set expansion node
     qe.end = neighbors[iv]; //current neigbor is end point
     qe.weight = G.get_edge_value(s,qe.end);//load weight of edge from s to current neighbor
     open_set.insert(qe); //add element to open set
   }
   closed_set.insert(open_set.top()); //take edge with least costs
   result += open_set.top().weight; //sum up weight for mst
   cout <<"MST: added edge " << open_set.top().start << "->" << open_set.top().end << ", weight=" << open_set.top().weight << endl; 
   open_set.queue_clear(); //remove this edge from the open set 
 
   //up to this point, the first edge of the minimal spanning tree has been added
   //from now on we will proceed in a similar fashion, but the neighbors to be considered
   //in the open set are all the neighbors that connect to the tree, of which we will choose 
   //the cheapest

   while( closed_set.queue_size() != G.V-1) //assume that graphs are connected
   {
      for( int i=1; i<G.V; i++) //loop over all vertices
      {
         if( closed_set.contains_end_pt(i) || closed_set.contains_start_pt(i)) //check if vertex is part of tree
         {
            qe.start = i; //prepare queue element for open set
            neighbors = G.neighbors(i); //find all neighbors of point i
            for( vector<int>::size_type iv=0; iv != neighbors.size(); iv++)  //add to open set
            {
               qe.end = neighbors[iv]; //current neigbor is end point
               qe.weight = G.get_edge_value(i,qe.end);//load weight of edge from i to current neighbor
               qr = qe; //reverse element
               qr.start = qe.end;
               qr.end = qe.start;
               if( !closed_set.contains(qe) && !closed_set.contains(qr) \
                && !closed_set.contains_end_pt(qe.end) && !closed_set.contains_start_pt(qe.end))
               {
                  open_set.insert(qe); //add element to open set
               }
            }
         }
      }
      
      closed_set.insert(open_set.top()); //take edge with least costs
      result += open_set.top().weight; //sum up weight for mst
      cout <<"MST: added edge " << open_set.top().start << "->" << open_set.top().end << ", weight=" << open_set.top().weight << endl; 
      open_set.queue_clear(); //remove this edge from the open set 
       
   }
   return result; 
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> begin main <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int main()
{
   const double range[] = {1.0,10.0}; //set the range for the edges
   const double dnsty   = 0.2; //set the density for the graph
   minimal_spanning_tree mst; //minimal spanning tree
   
   srand(clock()); //seed random number generator
   
   cout << "=================================" << endl;
   cout << "=    Minimum Spanning Tree      =" << endl;
   cout << "=================================" << endl;
   cout << "       by Andreas Windisch "       << endl;
   cout << "   andreas.windisch@yahoo.com"     << endl;
   cout << "=================================" << endl << endl;
   cout << "Homework 03 for coursera online course 'C++ for C Programmers' " << endl; 
   cout << "************************************************************** " << endl << endl; 
   cout << "I will use one of the graphs discussed in class for this homework using the " << endl;
   cout << "Jarnik-Prim algorithm. I will make use of the classes I implemented in the" << endl; 
   cout << "previous assignment for the shortest path calculation. " << endl;
   cout << "Here is the graph (vertices re-labeled to ints, weights in parantheses)."<< endl << endl;

   cout << "                                           Dictionary labels used in class" << endl;
   cout << "                                           ===============================" << endl;
   cout << "     1--(8)--2--(7)--3                        class      |      here      " << endl;
   cout << "    /|      / \\      |\\                         a        |       0        " << endl;
   cout << "  (4)|    (2)  \\     |(9)                       b        |       1        " << endl;
   cout << "  /  |    /     \\    |  \\                       c        |       2        " << endl;
   cout << " 0  (11) 8      (4) (14) 4                      d        |       3        " << endl;
   cout << "  \\  |  / \\       \\  |  /                       e        |       4        " << endl;
   cout << "  (8)|(7) (6)      \\ |(10)                      f        |       5        " << endl;
   cout << "    \\|/     \\       \\|/                         g        |       6        " << endl;
   cout << "     7--(1)--6--(2)--5                          h        |       7        " << endl;
   cout << "                                                i        |       8        " << endl;
   cout << "                                                                        " << endl << endl;
   cout << "Computing the minimal spanning tree as discussed in class. The result  " << endl;
   cout << "should be 37.0." << endl << endl;

   cout << "Creating empty an graph with 9 vertices. " << endl;
   graph a(9); //create empty graph with 9 vertices

   cout << "Building graph as shown above. " << endl;
   a.add(0,1,4.0);a.add(0,7,8.0);   
   a.add(1,2,8.0);a.add(1,7,11.0);   
   a.add(2,3,7.0);a.add(2,8,2.0);a.add(2,5,4.0);   
   a.add(3,4,9.0);a.add(3,5,14.0);   
   a.add(4,5,10.0);   
   a.add(5,6,2.0);   
   a.add(6,7,1.0);a.add(6,8,6.0);   
   a.add(7,8,7.0);   
  
   cout << "Printing  adjacency list for this graph: " << endl;
   a.print();
   cout << a << endl;

   cout << "Computing minimal spanning tree (Jarnik-Prim)" << endl;
   cout << "---------------------------------------------" << endl;
   cout << "Minimal Spanning Tree = " << mst.mst_jarnik_prim(a) << endl;   
   cout << "---------------------------------------------" << endl;

   cout << "Reading in graph from file provided on coursera" << endl;  
   ifstream data_file("mst_data.txt"); 
   graph b(data_file);
   cout << "Computing minimal spanning tree (Jarnik-Prim)" << endl;
   cout << "---------------------------------------------" << endl;
   cout << "Minimal Spanning Tree = " << mst.mst_jarnik_prim(b) << endl;   
   cout << "---------------------------------------------" << endl;

   return 0;
}
