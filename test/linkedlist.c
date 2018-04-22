#include "linkedlist.h"
#include "linkedlist_funcs.h"

#include <assert.h>
#include <errno.h> /* for errno, EINVAL, ERANGE */
#include <log.h> /* for info(), verbose() */
#include <stdio.h> /* for printf() */
#include <stdlib.h> /* for NULL */



extern int errno;



static LinkedList *llist;


static int VALUES[] = {42, 3, 7, 13, 6};
static const unsigned int INT_LINKED_LIST_SIZE = 5;

static bool eq_as_int(const data *const e1, const data *const e2) {
	assert(e1 != NULL && e2 != NULL);
	return *(int*)e1 == *(int*)e2;
}
static const char eq_as_int_repr[] = "(data *e1, data *e2) -> (*(int*)e1 == *(int*)e2)";



static void init(void) {
	info("llist = ll_new()");
	llist = ll_new();
	assert(llist != NULL);
	info("OK\n");
}

static void cleanup(void) {
	info("ll_free(llist)");
	ll_free(llist);
	info("OK\n");
}


static void test_ll_len__empty(void) {
	unsigned int got;
	info("test ll_len -- empty list");
	info("ll_len(llist)");
	verbose("expected: 0");
	got = ll_len(llist);
	verbose("got     : %u", got);
	assert(got == 0);
	info("OK\n");
}

static void test_ll_append(void) {
	int expected, got;
	data *param;
	info("tests ll_append");
	for(unsigned int n = 0; n < INT_LINKED_LIST_SIZE; ++n) {
		param = VALUES + n;
		info("ll_append(llist, %p)", param);
		expected = n;
		verbose("expected: %d", expected);
		got = ll_append(llist, param);
		verbose("got     : %d", got);
		assert(got == expected);
		assert(errno == 0);
	}
	info("OK\n");
}

static void test_ll_len__full(void) {
	unsigned int got;
	info("test ll_len -- list full");
	info("ll_len(llist)");
	verbose("expected: %u", INT_LINKED_LIST_SIZE);
	got = ll_len(llist);
	verbose("got     : %u", got);
	assert(got == INT_LINKED_LIST_SIZE);
	info("OK\n");
}

static void test_ll_get__valid(void) {
	data *expected, *got;
	info("test ll_get -- valid indices");
	for(unsigned int index = 0; index < INT_LINKED_LIST_SIZE; ++index) {
		info("ll_get(llist, %u)", index);
		expected = VALUES + index;
		verbose("expected: %p", expected);
		got = ll_get(llist, index);
		verbose("got     : %p", got);
		assert(got == expected);
		assert(errno == 0);
	}
	info("OK\n");
}

static void test_ll_get__invalid(void) {
	const unsigned int invalid_indices[3] = {
		ll_len(llist),
		ll_len(llist) + 1,
		42
	};
	data *got;
	unsigned int index;
	info("test ll_get -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("ll_get(llist, %u)", index);
		verbose("expected: (nil)");
		got = ll_get(llist, index);
		verbose("got     : %p", got);
		assert(got == NULL);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_ll_set__valid(void) {
	static int extra = 64;
	data *const param = &extra;
	const unsigned int index = 4;
	info("test ll_set -- valid index");
	info("ll_set(llist, %u, %p)", index, param);
	verbose("expected errno: 0");
	ll_set(llist, index, param);
	verbose("actual errno  : %d", errno);
	assert(errno == 0);
	info("OK\n");
}

static void test_ll_set__invalid(void) {
	static int extra = 23;
	data *const param = &extra;
	const unsigned int invalid_indices[3] = {
		ll_len(llist),
		ll_len(llist) + 1,
		73 /* big "arbitrary" value */
	};
	unsigned int index;
	info("tests ll_set -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("ll_set(llist, %u, %p)", index, param);
		verbose("expected errno: %d", ERANGE);
		ll_set(llist, index, param);
		verbose("actual errno  : %d", errno);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_ll_add__valid(void) {
	static int extra = 8;
	data *const param = &extra;
	int expected, got;
	const unsigned int index = 3;
	info("test ll_add -- valid index");
	info("ll_add(llist, %u, %p)", index, param);
	expected = (signed)index;
	verbose("expected: %d", expected);
	got = ll_add(llist, index, param);
	verbose("got     : %d", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_ll_add__invalid(void) {
	int extra = 9;
	data *const param = &extra;
	const unsigned int invalid_indices[3] = {
		ll_len(llist) + 1,
		ll_len(llist) + 2,
		668
	};
	int got;
	unsigned int index;
	info("test ll_add -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("ll_add(llist, %u, %p)", index, param);
		verbose("expected: -1");
		got = ll_add(llist, index, param);
		verbose("got     : %d", got);
		assert(got == -1);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_ll_drop__valid(void) {
	data *expected, *got;
	const unsigned int index = 0;
	info("test ll_drop -- valid index");
	info("ll_drop(llist, %u)", index);
	expected = VALUES + index;
	verbose("expected: %p", expected);
	got = ll_drop(llist, index);
	verbose("got     : %p", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_ll_drop__invalid(void) {
	const unsigned int invalid_indices[3] = {
		ll_len(llist),
		ll_len(llist) + 1,
		99
	};
	data *got;
	unsigned int index;
	info("test ll_drop -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("ll_drop(llist, %u)", index);
		verbose("expected: (nil)");
		got = ll_drop(llist, index);
		verbose("got     : %p", got);
		assert(got == NULL);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_ll_swap__valid(void) {
	static int extra = -32;
	data *const param = &extra;
	const unsigned int index = 2;
	data *expected, *got;
	info("test ll_swap -- valid index");
	info("ll_swap(llist, %u, %p)", index, param);
	expected = VALUES + index;
	verbose("expected: %p = %d", expected, *(int*)expected);
	got = ll_swap(llist, index - 1, param); /* -1 because of ll_drop */
	verbose("got     : %p = %d", got, *(int*)got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_ll_swap__invalid(void) {
	int extra = 4;
	data *const param = &extra;
	const unsigned int invalid_indices[3] = {
		ll_len(llist),
		ll_len(llist) + 1,
		13
	};
	data *got;
	unsigned int index;
	info("test ll_swap -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("ll_swap(llist, %u, %p)", index, param);
		verbose("expected: (nil)");
		got = ll_swap(llist, index, param);
		verbose("got     : %p", got);
		assert(got == NULL);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_ll_cond__found(void) {
	const int value = VALUES[1];
	const data *const param = &value;
	data *expected, *got;
	info("test ll_cond -- found");
	expected = VALUES + 1;
	info("ll_cond(array, *%d, %s)", *(int*)param, eq_as_int_repr);
	verbose("expected: %p", expected);
	got = ll_cond(llist, param, eq_as_int);
	verbose("got     : %p", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_ll_cond__not_found(void) {
	static int value = 1024;
	data *const param = &value;
	data *got;
	info("test ll_cond -- not found");
	info("ll_cond(array, *%d, %s)", value, eq_as_int_repr);
	verbose("expected: (nil)");
	got = ll_cond(llist, param, eq_as_int);
	verbose("got     : %p", got);
	assert(got == NULL);
	info("OK\n");
}

static void test_ll_remove__found(void) {
	const int value = 13;
	const data *const param = &value;
	data *expected, *got;
	info("test ll_remove -- item found");
	info("ll_remove(llist, %p, %s)", param, eq_as_int_repr);
	expected = VALUES + 3;
	verbose("expected: %p", expected);
	got = ll_remove(llist, param, eq_as_int);
	verbose("got     : %p", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_ll_remove__not_found(void) {
	const int value = 4096;
	const data *const param = &value;
	data *got;
	info("test ll_remove -- item not found");
	info("ll_remove(llist, %p, %s)", param, eq_as_int_repr);
	verbose("expected: (nil)");
	got = ll_remove(llist, param, eq_as_int);
	verbose("got     : %p", got);
	assert(got == NULL);
	assert(errno == EINVAL);
	info("OK\n");
}


static void print_as_int(const data *const e) {
	if(e)
		printf("%d", *(int*)e);
	else
		printf("(null)");
}
void test_linkedlist(void) {

	init();

	test_ll_len__empty();

	test_ll_append();
	ll_printf(llist, print_as_int);

	test_ll_len__full();

	test_ll_get__valid();

	test_ll_get__invalid();

	test_ll_set__valid();
	ll_printf(llist, print_as_int);

	test_ll_set__invalid();

	test_ll_add__valid();
	ll_printf(llist, print_as_int);

	test_ll_add__invalid();

	test_ll_drop__valid();
	ll_printf(llist, print_as_int);

	test_ll_drop__invalid();

	test_ll_swap__valid();
	ll_printf(llist, print_as_int);

	test_ll_swap__invalid();

	test_ll_cond__found();

	test_ll_cond__not_found();

	ll_printf(llist, print_as_int);
	test_ll_remove__found();

	test_ll_remove__not_found();

	cleanup();
}