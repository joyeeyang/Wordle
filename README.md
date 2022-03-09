# Wordle CSC209 Assignment

- Makefile is provided
- To run: ./solver samples/input1 > output1.txt


This program is designed to solve the Wordle game reversely, with modefied rules

Wordle rules:

  - A green tile means that the letter is in the solution in the same position.
  - A yellow tile means that the letter is in the solution in a different position.  
  - A grey tile means that the letter is not in the solution.
  
Furthermore:
  - If a letter was marked as grey, it cannot be used again.\n
  - If a letter was marked as yellow, that letter must be used in the next guess in a different position that the previous guess.
  - If a letter was marked as green, that letter must be used in the same position in the next guess.
  
Steps to solution:
  1. Read the dictionary file into a linked list. The dictionary file contains a list of 5-letter words, one per line.  Read the file and construct a linked list of struct nodes.  This is a task in the extra lab, so you can just copy your code from that lab.
  2. Read the wordle input and initialize the wordle struct. This function is given to you.
  3. Initialize the root node of the tree.  The constraints struct for the root node initially is empty.
  4. Build the solver tree - the solver tree is a general tree where a node can have an arbitrary number of children.  A parent node has a pointer to a linked list of child nodes.  The linked list of child nodes is connected through a next_sibling pointer.  See the solver tree   Download solver tree diagram.
    a. Construct the constraints that will be applied to generate the list of children for this node.  Use the functions set_green, set_yellow, and add_to_cannot_be that you wrote in the extra lab.
    b. Iterate over the words in the dictionary, calling match_constraints on each word to see if it fits.  If a word matches,  create a new struct solver_node, add it to the list of children of this node, and call solve_subtree on it.  The base case of the recursion is when we reach the end of the rows in the wordle input.
  
  
