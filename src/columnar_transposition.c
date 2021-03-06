/*
    Cifer: Automating classical cipher cracking in C
    Copyright (C) 2008  Daniel Richman & Simrun Basuita

    Cifer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cifer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cifer.  If not, see <http://www.gnu.org/licenses/>.
*/

/* TODO: Look at r2c c2r reoordering is happening on rows not columns */

#include "stdinc.h"

/* KEY FORMAT: int array of size "key size", no recurring numbers
 * to use, the columns are shifted. ZEBRAS: 521304. The key allways
 * encodes; to decode you have to flip the key first =) (there's a 
 * function below to do that). The key must be starting at 0. */

void columnar_transposition_bruteforce(char *intext, int intext_size, 
         char *outtext, int key_min, int key_max, 
         columnar_transposition_function routine)
{
  int key_size, i, j, k, h, factorial, np, npi, score, best_score, best_size;
  int temp_text_size;
  int *key, *key_best;
  score_text_pro_state pro_state;

  /* Allocate to the maximum size, that's means we only have to alloc once */
  key      = malloc_good(sizeof(int) * key_max);
  key_best = malloc_good(sizeof(int) * key_max);

  /* Clip the size of the temporary text to make it quicker */
  temp_text_size = min(600, intext_size);

  /* We'll use the outtext as temporary space as well, so lets shove a 
   * null in the right place to be sure */
  *(outtext + temp_text_size) = 0;

  /* Prepare */
  best_score = -1;
  best_size = 0;

  /* Go Pro! */
  score_text_pro_start(temp_text_size, &pro_state);

  /* Print a header */
  printf("Columnar Transposition Bruteforce: %i => %i, using %i chars\n", 
                                             key_min, key_max, temp_text_size);
  printf(" ->  key_size - factorial | loops/bar: progress...\n");

  /* Start teh cracking! */
  for (key_size = key_min; key_size <= key_max; key_size++)
  {
    /* Calcualte the factorial */
    factorial = 1;
    for (i = 2; i <= key_size; i++)  factorial *= i;

    /* Sub-progress indicator setup */
    npi = max(factorial / 30, 1);  /* How many loops equals a new bar? */
    np  = npi;             /* The running "next loop for bar" precache */

    /* Progress indicator */
    printf(" -> %3i - %8i | %7i: ", key_size, factorial, npi); 
    fflush(stdout);

    for (i = 1; i <= factorial; i++)
    {
      /* Initialise the key */
      for (j = 0; j < key_size; j++)  key[j] = j;

      /* Prepare */
      k = i;

      /* Generate the permutation */
      for (j = 2; j <= key_size; j++)
      {
        k = k / (j - 1);
        h = key[modp(k, j)];
        key[modp(k, j)] = key[j - 1];
        key[j - 1] = h;
      }

      /* Try it */
      (*routine)(intext, temp_text_size, outtext, key, key_size);

      /* Score it */
      score = score_text_pro(outtext, &pro_state);

      if (score > best_score)
      {
        best_score = score;
        best_size = key_size;
        for (j = 0; j < key_size; j++) key_best[j] = key[j];
      }

      /* Subprocess Indicator */
      if (i == np)
      {
        np += npi;
        printf("|");
        fflush(stdout);
      }
    }

    printf("\n -> %3i - %8i: best score %4i, from length %i; key: ", 
                  key_size, factorial, best_score, best_size); 
    printf("%i", key_best[0]);
    for (i = 1; i < best_size; i++) printf("|%i", key_best[i]);
    printf("\n");
  }

  /* Print _pro stats */
  score_text_pro_print_stats("columnar-transposition", &pro_state);

  /* Do it for real and save the result (doing for real WITH numbers) */
  (*routine)(intext, intext_size, outtext, key_best, best_size);

  /* Be sure about null pointers */
  *(outtext + intext_size) = 0;
  *(intext + intext_size) = 0;

  /* Results */
  printf("Columnar Transposition Bruteforce: best_score %i; key size %i\n",
                                      best_score, best_size);

  columnar_transposition_keyinfo(key_best, best_size);

  printf("%s\n\n", outtext);

  /* Free up */
  free(key);
  free(key_best);
  score_text_pro_cleanup(&pro_state);
}

void columnar_transposition_keyinfo(int *key, int key_size)
{
  int i;

  printf("key[%i]: %i", key_size, key[0]);
  for (i = 1; i < key_size; i++) printf(" %i", key[i]);
  printf("\n");
}

/* Reads off into columns KEY_SIZE, reorders as per key, reads off
 * in columns - the key is array[source] => target */
/* THE INPUT TEXT AND THE OUTPUT TEXT MUST BE SEPARATE STRINGS */
void columnar_transposition_col2col(char *intext, int text_size,
                           char *outtext, int *key, int key_size)
{
  int i, chunk_start, key_item, target;

  chunk_start = 0;
  for (i = 0; i < text_size; i++)
  {
    key_item = modp(i, key_size);
    if (key_item == 0)  chunk_start = i;
    if (chunk_start + key_size >= text_size) break;
    target = chunk_start + key[key_item];

    *(outtext + target) = *(intext + i);
  }

  /* Any remainter should just be copied from a => b */
  while (i < text_size)
  {
    *(outtext + i) = *(intext + i);
    i++;
  }
}

/* Essentially writes out the text in cols (top to bottom then 
 * reads off in rows (left to right). This  is a DECODER,
 * however it requires a flipped key. */
void columnar_transposition_col2row(char *intext, int text_size,
                           char *outtext, int *key, int key_size)
{
  int i, col_start, col_end, col_num, col_std_length, cols_incomplete, target;

  col_start = 0; col_end = 0; col_num = -1;
  cols_incomplete = modp(text_size, key_size);
  col_std_length = (text_size - cols_incomplete) / key_size;

  for (i = 0; i < text_size; i++)
  {
    if (i == col_end)
    {
      /* New col calculation is needed */
      col_num++;
      col_start = i;
      col_end = i + col_std_length + (key[col_num] < cols_incomplete ? 1 : 0);
    }

    target = key[col_num] + ((i - col_start) * key_size);
    *(outtext + target) = *(intext + i);
  }
}

/* encodes, writes out in rows (l => r) and reads off in columns. */
void columnar_transposition_row2col(char *intext, int text_size,
                           char *outtext, int *key, int key_size)
{
  int i, cols_incomplete, col_std_length, col_num, col_start, col_pos;

  cols_incomplete = modp(text_size, key_size);
  col_std_length = (text_size - cols_incomplete) / key_size;

  for (col_num = 0; col_num < key_size; col_num++)
  {
    col_start = col_std_length * key[col_num];
    col_pos = 0;

    for (i = 0; i < key_size; i++)
      if (i < cols_incomplete && key[i] < key[col_num]) col_start++;

    for (i = col_num; i < text_size; i += key_size, col_pos++)
    {
      /* Source = i, target = col_start + col_pos */
      *(outtext + col_start + col_pos) = *(intext + i);
    } 
  }
}

void columnar_transposition_flip_key(int *key, int key_size)
{
  int i, *temp;  /* Dynamic size */

  temp = malloc_good( sizeof(int) * key_size );

  /* copy into temp, then flip */
  for (i = 0; i < key_size; i++) temp[i] = key[i];
  for (i = 0; i < key_size; i++) key[temp[i]] = i;

  /* Free up */
  free(temp);
}

void columnar_transposition_text2key(char *text, int text_size, 
                                     int **key, int *new_key_size)
{
  int i, j, c;
  int used[26];

  /* Used variable cries out for initialisation */
  for (i = 0; i < 26; i++) used[i] = -1;

  /* Pass 1, count letters (into variable j) */
  for (i = 0, j = 0; i < text_size; i++)
  {
    c = CHARNUM( *(text + i) );

    if (c == -1)
    {
      /* Fail */
      *key = NULL;
      *new_key_size = 0;
      return;
    }

    if (used[c] == -1)
    {
      used[c] = j;
      j++;
    }
  }

  /* Now allocate */
  *key = malloc_good( sizeof(int) * j );
  *new_key_size = j;

  #define targkey  (*key)

  /* Pass 2: Using the used array, we can work out each items 
   * position in the alphabet relative to this key... */
  for (i = 0, j = 0; i < 26; i++)
  {
    if (used[i] != -1)
    {
      /* because we're now going through by used[i] insted of text[i] we're 
       * going in alphabetical order.
       *  used[i] contains a reference to the character in which it occured.
       * We want to filter unused characters
       * And so J goes up each time we get a match, so the first alphabetic
       * character order has 0.
       * And we used the reference in used[i] to just save it into the key! */

      targkey[used[i]] = j;
      j++;

      /* Hope that makes sense */
    }
  }
}

int columnar_transposition_verify_key(int *key, int key_size)
{
  int i, *used;

  /* First checks */
  if (key_size < 2)  return -1;

  /* Allocate */
  used = malloc_good( sizeof(int) * key_size );

  /* Initialise */
  for (i = 0; i < key_size; i++) used[i] = 0;

  /* For each key value, check that it fits and then
   *  increment the value's use count */
  for (i = 0; i < key_size; i++) 
    if (key[i] >= 0 && key[i] < key_size) 
      used[key[i]]++;

  /* For anything that hasn't been used once, 
   * only once (ie. not too much, not too little) fail */
  for (i = 0; i < key_size; i++) if (used[i] != 1) return -1;

  /* Otherwise success */
  return 0;
}

