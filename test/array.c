#include "array.h"
#include "array_funcs.h"

#include <assert.h>
#include <errno.h> /* for errno, EINVAL, ERANGE */
#include <log.h> /* for info(), verbose(), log_setfilter() */
#include <stdio.h> /* for printf() */
#include <stdlib.h> /* for NULL */



extern int errno;



static Array *array;

static const unsigned int INT_ARRAY_SIZE = 10;
static int VALUES[] = {-1, 42, 666, 13, 28, -54, 0, 7 , 6, 5};

static bool eq_as_int(const data *const e1, const data *const e2) {
	assert(e1 != NULL && e2 != NULL);
	return *(int*)e1 == *(int*)e2;
}
static const char eq_as_int_repr[] = "(data *e1, data *e2) -> (*(int*)e1 == *(int*)e2)";


static void init(void) {
	info("array = a_new(%u)", INT_ARRAY_SIZE);
	array = a_new(INT_ARRAY_SIZE);
	assert(array != NULL);
	info("OK\n");
}

static void cleanup(void) {
	a_free(array);
}


static void test_a_new__0_null(void) {
	unsigned int index;
	Array *got;
	info("test a_new -- size 0 => NULL array");
	index = 0;
	info("a_new(%u)", index);
	verbose("expected: (nil)");
	got = a_new(index);
	verbose("got     : %p", (void*)got);
	assert(got == NULL);
	assert(errno == EINVAL);
	info("OK\n");
}

static void test_a_size__empty(void) {
	unsigned int got;
	info("test a_size -- empty array");
	info("a_size(array)");
	verbose("expected: 0");
	got = a_size(array);
	verbose("got     : %u", got);
	assert(got == 0);
	info("OK\n");
}

static void test_a_append(void) {
	data *param;
	int expected, got;
	info("test a_append");
	verbose("VALUES = [-1, 42, 666, 13, 28, -54, 0, 7, 6, 5]");
	for(unsigned int index = 0; index < INT_ARRAY_SIZE; ++index) {
		param = VALUES + index;
		info("a_append(array, %p)", param);
		expected = index;
		verbose("expected: %d", expected);
		got = a_append(array, param);
		verbose("got     : %d", got);
		assert(got == expected);
		assert(errno == 0);
	}
	info("OK\n");
}

static void test_a_size__full(void) {
	unsigned int expected, got;
	info("test a_size -- array full");
	info("a_size(array)");
	expected = INT_ARRAY_SIZE;
	verbose("expected: %u", expected);
	got = a_size(array);
	verbose("got     : %u", got);
	assert(got == expected);
	info("OK\n");
}

static void test_a_get__valid(void) {
	data *expected, *got;
	info("tests a_get -- valid indices");
	for(unsigned int index = 0; index < INT_ARRAY_SIZE; ++index) {
		info("a_get(array, %d)", index);
		expected = VALUES + index;
		verbose("expected: %p", expected);
		got = a_get(array, index);
		verbose("got     : %p", got);
		assert(got == expected);
		assert(errno == 0);
	}
	info("OK\n");
}

static void test_a_get__invalid(void) {
	const unsigned int invalid_indices[3] = {
		a_size(array),
		a_size(array) + 1,
		73 /* big "arbitrary" value */
	};
	unsigned int index;
	data *got;
	info("tests a_get -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("a_get(array, %u)", index);
		verbose("expected: (nil)");
		got = a_get(array, index);
		verbose("got     : %p", got);
		assert(got == NULL);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_a_set__valid(void) {
	static int extra = 64;
	data *const param = &extra;
	const unsigned int index = 7;
	info("test a_set -- valid index");
	info("a_set(array, %u, %p)", index, param);
	verbose("expected errno: 0");
	a_set(array, index, param);
	verbose("actual errno  : %d", errno);
	assert(errno == 0);
	info("OK\n");
}

static void test_a_set__invalid(void) {
	static int extra = 23;
	data *const param = &extra;
	const unsigned int invalid_indices[3] = {
		a_size(array),
		a_size(array) + 1,
		73 /* big "arbitrary" value */
	};
	unsigned int index;
	info("tests a_set -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("a_set(array, %u, %p)", index, param);
		verbose("expected errno: %d", ERANGE);
		a_set(array, index, param);
		verbose("actual errno  : %d", errno);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_a_add__invalid(void) {
	int value = 42;
	const unsigned int invalid_indices[3] = {
		a_size(array) + 1,
		a_size(array) + 2,
		73
	};
	int got;
	data *const param = &value;
	unsigned int index;
	info("tests a_add -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("a_add(array, %u, %p)", index, param);
		verbose("expected: -1");
		got = a_add(array, index, param);
		verbose("got     : %d", got);
		assert(got == -1);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_a_append__overflow(void) {
	static int extra = 73;
	int expected, got;
	data *const param = &extra;
	info("test a_append -- overflow size (=> realloc)");
	info("a_append(array, %p)", param);
	expected = INT_ARRAY_SIZE;
	verbose("expected: %d", expected);
	got = a_append(array, param);
	verbose("got     : %d", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_a_drop__valid(void) {
	const unsigned int index = 4;
	data *expected, *got;
	info("test a_drop -- valid index");
	info("a_drop(array, %u)", index);
	expected = VALUES + index;
	verbose("expected: %p", expected);
	got = a_drop(array, index);
	verbose("got     : %p", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_a_drop__invalid(void) {
	const unsigned int invalid_indices[3] = {
		a_size(array),
		a_size(array) + 1,
		73
	};
	data *got;
	unsigned int index;
	info("test a_drop -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("a_drop(array, %u)", index);
		verbose("expected: (nil)");
		got = a_drop(array, index);
		verbose("got     : %p", got);
		assert(got == NULL);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

void test_a_swap__valid(void) {
	static int extra = 777;
	data *const param = &extra;
	int expected, got;
	unsigned int index;
	info("test a_swap -- valid indices");
	index = 2;
	info("a_swap(array, %u, %p)", index, param);
	expected = VALUES[index];
	verbose("expected: %d", expected);
	got = *(int*)a_swap(array, index, param);
	verbose("got     : %d", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_a_swap__invalid(void) {
	int value = 42;
	const unsigned int invalid_indices[3] = {
		a_size(array),
		a_size(array) + 1,
		73
	};
	data *const param = &value;
	unsigned int index;
	data *got;
	info("tests a_swap -- invalid indices");
	for(unsigned int i = 0; i < 3; ++i) {
		index = invalid_indices[i];
		info("a_swap(array, %u, %p)", index, param);
		verbose("expected: (nil)");
		got = a_swap(array, index, param);
		verbose("got     : %p", got);
		assert(got == NULL);
		assert(errno == ERANGE);
	}
	info("OK\n");
}

static void test_a_remove__found(void) {
	const int value = VALUES[3];
	const data *const param = &value;
	data *expected, *got;
	info("test a_remove -- item found");
	info("a_remove(array, *%d, %s)", value, eq_as_int_repr);
	expected = VALUES + 3;
	verbose("expected: %p", expected);
	got = a_remove(array, param, eq_as_int);
	verbose("got     : %p", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_a_remove__not_found(void) {
	const int value = -42;
	const data *const param = &value;
	data *got;
	info("test a_remove -- item not found");
	info("a_remove(array, *%d, %s)", value, eq_as_int_repr);
	verbose("expected: (nil)");
	got = a_remove(array, param, eq_as_int);
	verbose("got     : %p", got);
	assert(got == NULL);
	assert(errno == EINVAL);
	info("OK\n");
}

static void test_a_cond__found(void) {
	const int value = VALUES[1];
	const data *const param = &value;
	data *expected, *got;
	info("test a_cond -- found");
	expected = VALUES + 1;
	info("a_cond(array, *%d, %s)", *(int*)param, eq_as_int_repr);
	verbose("expected: %p", expected);
	got = a_cond(array, param, eq_as_int);
	verbose("got     : %p", got);
	assert(got == expected);
	assert(errno == 0);
	info("OK\n");
}

static void test_a_cond__not_found(void) {
	static int value = 1024;
	data *const param = &value;
	data *got;
	info("test a_cond -- not found");
	info("a_cond(array, *%d, %s)", value, eq_as_int_repr);
	verbose("expected: (nil)");
	got = a_cond(array, param, eq_as_int);
	verbose("got     : %p", got);
	assert(got == NULL);
	info("OK\n");
}


static void print_as_int(const data *const e) {
	if(e)
		printf("%d", *(int*)e);
	else
		printf("(null)");
}
void test_array(void) {

	init();

	test_a_new__0_null();

	test_a_size__empty();

	test_a_append();
	a_printf(array, print_as_int);

	test_a_size__full();

	test_a_get__valid();

	test_a_get__invalid();

	test_a_set__valid();
	a_printf(array, print_as_int);

	test_a_set__invalid();

	test_a_add__invalid();

	test_a_append__overflow();
	a_printf(array, print_as_int);

	test_a_drop__valid();

	test_a_drop__invalid();

	test_a_swap__valid();
	a_printf(array, print_as_int);

	test_a_swap__invalid();

	test_a_remove__found();

	test_a_remove__not_found();

	test_a_cond__found();

	test_a_cond__not_found();

	cleanup();
}