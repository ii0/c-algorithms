/*

Copyright (c) 2005-2008, Simon Howard

Permission to use, copy, modify, and/or distribute this software 
for any purpose with or without fee is hereby granted, provided 
that the above copyright notice and this permission notice appear 
in all copies. 

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE 
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN      
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "alloc-testing.h"
#include "framework.h"

#include "trie.h"

int test_array[100000];
char test_strings[100000][10];

Trie *generate_trie(void)
{
	Trie *trie;
	int i;
	int entries;

	/* Create a trie and fill it with a large number of values */

	trie = trie_new();
	entries = 0;

	for (i=0; i<100000; ++i) {

		/* Create a string containing a text version of i, and use
		 * it as a key for the value */
		
		test_array[i] = i;
		sprintf(test_strings[i], "%i", i);
		
		trie_insert(trie, test_strings[i], &test_array[i]);

		++entries;

		assert(trie_num_entries(trie) == entries);
	}

	return trie;
}

void test_trie_new_free(void)
{
	Trie *trie;
	
	/* Allocate and free an empty trie */

	trie = trie_new();

	assert(trie != NULL);

	trie_free(trie);

	/* Add some values before freeing */

	trie = trie_new();

	trie_insert(trie, "hello", "there");
	trie_insert(trie, "hell", "testing");
	trie_insert(trie, "testing", "testing");
	trie_insert(trie, "", "asfasf");

	trie_free(trie);

	/* Add a value, remove it and then free */

	trie = trie_new();

	trie_insert(trie, "hello", "there");
	assert(trie_remove(trie, "hello") == 1);
	
	trie_free(trie);
}

void test_lookup(void)
{
	Trie *trie;
	char buf[10];
	int *val;
	int i;

	trie = generate_trie();

	/* Test lookup for non-existent values */

	assert(trie_lookup(trie, "000000000000000") == TRIE_NULL);
	assert(trie_lookup(trie, "") == TRIE_NULL);

	/* Look up all values */

	for (i=0; i<100000; ++i) {

		sprintf(buf, "%i", i);

		val = (int *) trie_lookup(trie, buf);
		
		assert(*val == i);
	}

	trie_free(trie);
}

void test_remove(void)
{
	Trie *trie;
	char buf[10];
	int i;
	int entries;

	trie = generate_trie();

	/* Test remove on non-existent values. */

	assert(trie_remove(trie, "000000000000000") == 0);
	assert(trie_remove(trie, "") == 0);

	entries = trie_num_entries(trie);

	assert(entries == 100000);

	/* Remove all values */

	for (i=0; i<100000; ++i) {

		sprintf(buf, "%i", i);

		/* Remove value and check counter */

		assert(trie_remove(trie, buf) != 0);
		--entries;
		assert(trie_num_entries(trie) == entries);
	}

	trie_free(trie);
}

void test_replace(void)
{
	Trie *trie;
	int *val;

	trie = generate_trie();

	/* Test replacing values */

	val = malloc(sizeof(int));
	*val = 999;
	trie_insert(trie, "999", val);
	assert(trie_num_entries(trie) == 100000);

	assert(trie_lookup(trie, "999") == val);
	free(val);
	trie_free(trie);
}

void test_insert_empty(void)
{
	Trie *trie;
	char buf[10];

	trie = trie_new();

	/* Test insert on empty string */

	trie_insert(trie, "", buf);
	assert(trie_num_entries(trie) == 1);
	assert(trie_lookup(trie, "") == buf);
	assert(trie_remove(trie, "") != 0);

	assert(trie_num_entries(trie) == 0);

	trie_free(trie);
}

static UnitTestFunction tests[] = {
	test_trie_new_free,
	test_lookup,
	test_remove,
	test_replace,
	test_insert_empty,
	NULL
};

int main(int argc, char *argv[])
{
	run_tests(tests);

	return 0;
}

