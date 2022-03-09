#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "constraints.h"

/* Create and initialize a constraints struct. 
 * Sets the fields to 0 or empty string.
 * Return a pointer to the newly created constraints struct.
 */
struct constraints *init_constraints()
{

    struct constraints *c = malloc(sizeof(struct constraints));

    //set fieds of must_be to empty
    for (int i = 0; i < WORDLEN; i++)
    {
        c->must_be[i][0] = '\0';
    }

    //set fields of cannot_be to 0
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        c->cannot_be[i] = '0';
    }

    return c;
}

/* Update the "must_be" field at "index" to be a string 
 * containing "letter"
 * The tile at this index is green, therefore the letter at "index"
 * must be "letter"
 */
void set_green(char letter, int index, struct constraints *con)
{
    assert(islower(letter));
    assert(index >= 0 && index < SIZE);

    assert(islower(letter));
    assert(index >= 0 && index < SIZE);

    //strcpy(con->must_be[index][0], letter);
    con->must_be[index][0] = letter;
    con->must_be[index][1] = '\0';
}

/* Update "con" by adding the possible letters to the string at the must_be 
 * field for "index".
 * - index is the index of the yellow tile in the current row to be updated
 * - cur_tiles is the tiles of this row
 * - next_tiles is the tiles of the row that is one closer to the solution row
 * - word is the word in the next row (assume word is all lower case letters)
 * Assume cur_tiles and next_tiles contain valid characters ('-', 'y', 'g')
 * 
 * Add to the must_be list for this index the letters that are green in the
 * next_tiles, but not green in the cur_tiles or green or yellow in the 
 * next_tiles at index.
 * Also add letters in yellow tiles in next_tiles.
 */
void set_yellow(int index, char *cur_tiles, char *next_tiles,
                char *word, struct constraints *con)
{

    assert(index >= 0 && index < SIZE);
    assert(strlen(cur_tiles) == WORDLEN);
    assert(strlen(next_tiles) == WORDLEN);
    assert(strlen(word) == WORDLEN);

    //reset con to be empty
    for (int i = 0; i < WORDLEN; i++)
    {
        con->must_be[index][i] = '\0';
    }

    for (int i = 0; i < WORDLEN; i++)
    {
        // check for yellow tiles in next_tile and add it to must_be
        if (next_tiles[i] == 'y' && (i != index))
        {
            strncat(con->must_be[index], &word[i], 1);
        }

        // check the letters that are green in next line, and if they are not green in cur or green or yellow in next_tiles at index, add to must_be
        if (next_tiles[i] == 'g' && cur_tiles[i] != 'g' && i != index)
        {
            //if cur_tiles is not green and next is, then must_be solution at index i
            strncat(con->must_be[index], &word[i], 1);
        }
    }
}

/* Add the letters from cur_word to the cannot_be field.
 * See the comments in constraints.h for how cannot_be is structured.
 */
void add_to_cannot_be(char *cur_word, struct constraints *con)
{
    assert(strlen(cur_word) <= WORDLEN);

    int len = strlen(cur_word);
    for (int i = 0; i < len; i++)
    {
        con->cannot_be[cur_word[i] - 'a'] = '1';
    }
}

void print_constraints(struct constraints *c)
{
    printf("cannot_be: ");

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (c->cannot_be[i] == '1')
        {
            printf("%c ", 'a' + i);
        }
    }

    printf("\nmust_be\n");
    for (int i = 0; i < WORDLEN; i++)
    {
        printf("[%d] ", i);
        for (int j = 0; j < strlen(c->must_be[i]); j++)
        {
            printf("%c ", c->must_be[i][j]);
        }
        printf("\n");
    }
}
