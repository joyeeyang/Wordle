#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordlist.h"

/* Read the words from a filename and return a linked list of the words.
 *   - The newline character at the end of the line must be removed from
 *     the word stored in the node.
 *   - You an assume you are working with Linux line endings ("\n").  You are
 *     welcome to also handle Window line endings ("\r\n"), but you are not
 *     required to, and we will test your code on files with Linux line endings.
 *   - The time complexity of adding one word to this list must be O(1)
 *     which means the linked list will have the words in reverse order
 *     compared to the order of the words in the file.
 *   - Do proper error checking of fopen, fclose, fgets
 */

// helper function to help create a new node
struct node *create_node_helper(char *str, struct node *next)
{
    struct node *new = malloc(sizeof(struct node));
    strncpy(new->word, str, SIZE);
    new->word[SIZE - 1] = '\0';
    new->next = next;

    return new;
}

struct node *read_list(char *filename)
{

    struct node *word_list = malloc(sizeof(struct node));
    char read_line[SIZE + 1];
    FILE *file;
    int error;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("fopen");
        exit(1);
    }

    //reads the first word and this will be the last node of the linked list
    if (fgets(read_line, SIZE + 1, file) != NULL)
    {
        //remove trailing new line in read_line
        char *index;
        index = strchr(read_line, '\n');
        if (index != NULL)
        {
            read_line[index - read_line] = '\0';
        }

        strncpy(word_list->word, read_line, SIZE);
        word_list->word[SIZE - 1] = '\0';
    }

    //read the lines in file and create a linked list with next node pointing to current node, and current node replaced by the new node
    while (fgets(read_line, SIZE + 1, file) != NULL)
    {

        //remove trailing new line if there are any
        char *index;
        index = strchr(read_line, '\n');
        if (index != NULL)
        {
            read_line[index - read_line] = '\0';
        }
        word_list = create_node_helper(read_line, word_list);
    }

    error = fclose(file);
    if (error != 0)
    {
        fprintf(stderr, "fclose failed\n");
        exit(1);
    }

    return word_list;
}

/* Print the words in the linked-list list one per line
 */
void print_dictionary(struct node *list)
{

    while (list != NULL)
    {
        printf("%s", list->word);
        list = list->next;
    }
}

// Free all of the dynamically allocated memory in the dictionary list

void free_dictionary(struct node *list)
{

    struct node *temp;

    while (list != NULL)
    {
        temp = list;
        list = list->next;
        free(temp);
    }
}
