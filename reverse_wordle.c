#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordle.h"
#include "constraints.h"

/* Read the wordle grid and solution from fp. 
 * Return a pointer to a wordle struct.
 * See sample files for the format. Assume the input file has the correct
 * format.  In other words, the word on each is the correct length, the 
 * words are lower-case letters, and the line ending is either '\n' (Linux,
 * Mac, WSL) or '\r\n' (Windows)
 */
struct wordle *create_wordle(FILE *fp)
{
    struct wordle *w = malloc(sizeof(struct wordle));
    char line[MAXLINE];
    w->num_rows = 0;

    while (fgets(line, MAXLINE, fp) != NULL)
    {
        // remove the newline character(s)
        char *ptr;
        if (((ptr = strchr(line, '\r')) != NULL) ||
            ((ptr = strchr(line, '\n')) != NULL))
        {
            *ptr = '\0';
        }

        strncpy(w->grid[w->num_rows], line, SIZE);
        w->grid[w->num_rows][SIZE - 1] = '\0';
        w->num_rows++;
    }
    return w;
}

/* Create a solver_node and return it.
 * If con is not NULL, copy con into dynamically allocated space in the struct
 * If con is NULL set the new solver_node con field to NULL.
 * Tip: struct assignment makes copying con a one-line statements
 */
struct solver_node *create_solver_node(struct constraints *con, char *word)
{

    struct solver_node *solver = malloc(sizeof(struct solver_node));

    //copy the value of con
    if (con != NULL)
    {
        struct constraints *solver_con = malloc(sizeof(struct constraints));
        *solver_con = *con;
        solver->con = solver_con;
    }
    else
    {
        solver->con = NULL;
    }

    strncpy(solver->word, word, SIZE);
    solver->word[SIZE - 1] = '\0';
    solver->next_sibling = NULL;
    solver->child_list = NULL;

    return solver;
}

/* remove "letter" from "word"
 * "word" remains the same if "letter" is not in "word"
 */
void remove_char(char *word, char letter)
{
    char *ptr = strchr(word, letter);
    if (ptr != NULL)
    {
        *ptr = word[strlen(word) - 1];
        word[strlen(word) - 1] = '\0';
    }
}

/* Return 1 if "word" matches the constraints in "con" for the wordle "w".
 * Return 0 if it does not match
 */
int match_constraints(char *word, struct constraints *con,
                      struct wordle *w, int row)
{

    char *solution = malloc(sizeof(SIZE));
    strncpy(solution, w->grid[0], SIZE);
    solution[SIZE - 1] = '\0';

    for (int i = 0; i < WORDLEN; i++)
    {
        //check if word[i] is grey and in cannot_be. If yes then doesn't match
        if (w->grid[row][i] == '-' && con->cannot_be[word[i] - 'a'] == '1')
            return 0;

        //check if current yellow is the same as solution at index i. If same, doesn't match
        if (w->grid[row][i] == 'y' && solution[i] == word[i])
            return 0;

        //check if must_be[i] is empty
        if (strlen(con->must_be[i]) == 0)
        {
            //If must_be[i] is the empty string and word[index] is in the cannot_be set, doesn't match
            if (con->cannot_be[word[i] - 'a'] == '1')
            {
                return 0;
            }
        }
        else
        {
            if (strchr(con->must_be[i], word[i]) == NULL)
            {
                //if must_be[i] is not empty, check if word[i] is in must_be[i]. If not, does not match
                return 0;
            }
            else
            {
                if (strchr(solution, word[i]) == NULL)
                {
                    //if word[i] is in must_be[i] but has letter removed from char solution, which means there is duplicate
                    return 0;
                }
                else
                {
                    //if not duplicate, remove letter from solution
                    remove_char(solution, word[i]);
                }
            }
        }
    }

    free(solution);
    return 1;
}

/* This function is used to help remove node in the linked list to prevent duplicate.
Traverse through the linked list of dict's starting from dict_first_node. Returns the 
first node of the modified linked list
If first node is to be deleted, return second node right away
*/

/*this function will be used to set constraints for nodes in solver_subtree, according to 
their current tiles, next tiles, word, and constraint to update
*/
void set_constraint(int row, struct wordle *w, struct constraints *con_to_update, char *parent_word)
{

    for (int i = 0; i < SIZE; i++)
    {

        if (w->grid[row][i] == 'g')
        {
            //if the grid is green, simply call set_green
            set_green(parent_word[i], i, con_to_update);
        }
        else if (w->grid[row][i] == '-')
        {
            //if the grid is grey, clearn the must_be
            con_to_update->must_be[i][0] = '\0';
        }
        //if the grid is green or yellow, call set_green and set_yellow
        else if (w->grid[row][i] == 'y')
        {
            //if this is the root node, the next tile is the solution so 'ggggg'; Otherwise just call set_yellow
            if (row == 1)
            {
                char *all_green = "ggggg";
                set_yellow(i, w->grid[row], all_green, parent_word, con_to_update);
            }
            else
            {
                set_yellow(i, w->grid[row], w->grid[row - 1], parent_word, con_to_update);
            }
        }
    }
    add_to_cannot_be(parent_word, con_to_update);
}

/* Build a tree starting at "row" in the wordle "w". 
 * Use the "parent" constraints to set up the constraints for this node
 * of the tree
 * For each word in "dict", 
 *    - if a word matches the constraints, then 
 *        - create a copy of the constraints for the child node and update
 *          the constraints with the new information.
 *        - add the word to the child_list of the current solver node
 *        - call solve_subtree on newly created subtree
 */

void solve_subtree(int row, struct wordle *w, struct node *dict,
                   struct solver_node *parent)
{
    if (verbose)
    {
        printf("Running solve_subtree: %d, %s\n", row, parent->word);
    }

    //since this function is being called recursively, check if it reached max depth
    if (row == w->num_rows)
    {
        return;
    }

    //first node of dict which will be passed in recursively
    struct node *dict_first_node = dict;

    //check if this is the root node. If it is, set the parent node's constraints.
    if (row == 1)
    {
        set_constraint(row, w, parent->con, w->grid[0]);
    }

    //iterate through each node in dict and check if matches constraints
    while (dict != NULL)
    {
        //first check if current node matches constraint
        if (match_constraints(dict->word, parent->con, w, row))
        {
            //if matches, create a child
            struct constraints *child_con = malloc(sizeof(struct constraints));
            *child_con = *parent->con;

            set_constraint(row + 1, w, child_con, dict->word);
            struct solver_node *child_node = create_solver_node(child_con, dict->word);
            free(child_con);

            //if this is the first child, add the child node. Else add it to the beginning of the linked list child_list
            if (parent->child_list != NULL)
            {
                child_node->next_sibling = parent->child_list;
                parent->child_list = child_node;
                solve_subtree(row + 1, w, dict_first_node, child_node);
            }
            else
            {
                parent->child_list = child_node;
                solve_subtree(row + 1, w, dict_first_node, child_node);
            }
        }
        dict = dict->next;
    }
}

/* Print to standard output all paths that are num_rows in length.
 * - node is the current node for processing
 * - path is used to hold the words on the path while traversing the tree.
 * - level is the current length of the path so far.
 * - num_rows is the full length of the paths to print
 */

void print_paths(struct solver_node *node, char **path,
                 int level, int num_rows)
{

    //add the word at this path to **path, to be printed at the end of recursive calls

    char *this_path = malloc(sizeof(SIZE));
    strncpy(this_path, node->word, SIZE);
    this_path[SIZE - 1] = '\0';

    path[level - 1] = this_path;

    /*it has reached the final row, so print path and/or iterate to next sibling
    */
    if (num_rows == 1)
    {
        for (int i = 0; i < level; i++)
        {
            printf("%s ", path[i]);
        }
        printf("\n");

        //check if there's a sibling. If there is, call print_path on sibling
        if (node->next_sibling != NULL)
        {
            char **sibling_path;
            sibling_path = path;
            print_paths(node->next_sibling, sibling_path, level, num_rows);
        }
    }
    else if (node->child_list == NULL && node->next_sibling == NULL)
    {
        //no sibling and no child, reached the final node
        free(this_path);
        return;
    }

    //word of this node to path, and recursively call it for its child_list and siblings
    else
    {
        if (node->child_list != NULL)
            print_paths(node->child_list, path, level + 1, num_rows - 1);

        if (node->next_sibling != NULL)
        {
            char **sibling_path;
            sibling_path = path;

            print_paths(node->next_sibling, sibling_path, level, num_rows);
        }
    }

    free(this_path);
}

/* Free all dynamically allocated memory pointed to from w.
 */
void free_wordle(struct wordle *w)
{
    free(w);
}

/* Free all dynamically allocated pointed to from node
 */
void free_tree(struct solver_node *node)
{
    free(node->con);

    if (node->next_sibling != NULL)
        free_tree(node->next_sibling);
    if (node->child_list != NULL)
        free_tree(node->child_list);

    free(node);
}
