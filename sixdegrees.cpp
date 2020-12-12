#include <string>
#include <cstring>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
#include <queue>

using namespace std;

// this class will be used as values in an unordered_map
// will store an actor and the movie they have in common with another actor
class Pair {
  
  public:
    string actor;
    string movie;
  
};

// this class helps me define all of the current paths
// holds the string of the current actor in the step of the path, as well as the previously visited actors in a string called path
class Path {
  
  public:
    string curr_actor;
    string path;
};

int main(int argc, char** argv) {
  
  if (argc != 3) {
    throw std::invalid_argument("Usage: ./sixdegrees <INPUT FILE> <OUTPUT FILE>");
  }
  
  ifstream input;
  ofstream output;
  
  unordered_map<string, vector<Pair*>> all_actors;
  set<Pair*> garbage_truck; // for holding all of the pointers so we can delete them later
  
  // first we process the movie file
  string nextLine, movie, next_act;
  char *line, *token;
  
  input.open("cleaned_movielist.txt");
  
  // get each line of the movielist
  while (getline(input, nextLine)) {
    
    if (nextLine.length() == 0) {
      continue;
    }
    
    line = strdup(nextLine.c_str());
    
    token = strtok(line, " ");
    
    if (token == NULL) {
      continue;
    }
    
    movie = token;
    
    // initialize a vector of updates that will store all the connections in a given movie
    vector<Pair*> updates;
    
    token = strtok(NULL, " ");
    
    // the line has been tokenized, and we get each actor with each token, and create a pair out of it
    while (token != NULL) {
      next_act = token;
      
      Pair* to_add = new Pair;
      to_add->actor = next_act;
      to_add->movie = movie;
      
      updates.push_back(to_add);
      token = strtok(NULL, " ");
    }
    
    // once all of the pairs of a movie have been created, we update the hash table of each actor to hash to all of the pairs
    for (auto pair = updates.begin(); pair != updates.end(); pair++) {
      string curr = (*pair)->actor;
      // if the actor isnt a key yet, make it a key
      if (all_actors.find(curr) == all_actors.end())
        all_actors[curr] = updates;
      else // otherwise update the vector at that key
        all_actors[curr].insert(all_actors[curr].end(), updates.begin(), updates.end());
      
      garbage_truck.insert((*pair));
      
    }
    
    free(line);
  }
  
  // we've mapped all of the actors to their costars, time to handle the queries
  
  input.close();
  
  input.open(argv[1]);
  output.open(argv[2]);
  
  string actor1, actor2;
  
  
  queue<Path*> all_paths;
  
  // take each line of the input file, one line at a time
  while (getline(input, nextLine)) {
    if (nextLine.length() == 0) { // should never happen but just in case
      continue;
    }
    
    set<string> visited_acts;
    
    line = strdup(nextLine.c_str());
    
    token = strtok(line, " ");
    
    if (token == NULL) { // should also never happen but just in case
      continue;
    }
    
    actor1 = token; // set the starting actor to actor1
    
    token = strtok(NULL, " ");
    
    if (token == NULL) { // should also never happen but just in case
      continue;
    }
    
    actor2 = token; // set the ending actor to actor2
    
    free(line);
    
    // if either of the actors don't exist, write not present
    if (all_actors.find(actor1) == all_actors.end() || all_actors.find(actor2) == all_actors.end()) {
      output << "Not present\n";
      continue;
    }
    
    // initialize a new path, and set its contents equal to the starting actor
    Path* new_path = new Path;
    new_path->curr_actor = actor1;
    new_path->path = actor1;
    
    // insert into queue, and visited actors
    visited_acts.insert(actor1);
    
    all_paths.push(new_path);
    bool found = false;
    
    // while the queue isnt empty
    while (!all_paths.empty()) {
      Path* front = all_paths.front(); // pop the first value in the queue
      all_paths.pop();
      
      // check if equal to the ending actor, if so, write the path and break
      if (front->curr_actor.compare(actor2) == 0) {
        output << front->path + "\n";
        found = true;
        delete(front);
        break;
      }
      
      // otherwise, find all of the connections of the actor
      vector<Pair*> network = all_actors[front->curr_actor];
      
      // for every connection that has not already been visited, make a new path out of it and push to the queue
      for (auto pair = network.begin(); pair != network.end(); pair++) {
        if (visited_acts.find((*pair)->actor) == visited_acts.end()) {
          Path* add_path = new Path;
          add_path->curr_actor = (*pair)->actor;
          add_path->path = front->path + " -(" + (*pair)->movie + ")- " + (*pair)->actor;
          
          // check to see if a new path leads to the ending actor
          // if so, break
          if (add_path->curr_actor.compare(actor2) == 0) {
            output << add_path->path + "\n";
            found = true;
            delete(add_path);
            break;
          }
          
          all_paths.push(add_path);
          
          visited_acts.insert((*pair)->actor);
        }
        
      }
      
      // free memory we popped from stack
      delete(front);
      // if we found a path, break
      if (found)
          break;
      
    }
    
    // if we didn't find a path, output not present
    if (!found) {
      output << "Not present\n";
      continue;
    } else { // otherwise, make sure to empty the queue
      while (!all_paths.empty()) {
        Path* trash = all_paths.front();
        all_paths.pop();
        delete(trash);
      }
    }
    
  }
  
  // once we are done handling inputs, we free the memory of the unordered map
  for (auto i = garbage_truck.begin(); i != garbage_truck.end(); i++) {
    delete((*i));
  }
  
  input.close();
  output.close();
  
  return 0;
}
