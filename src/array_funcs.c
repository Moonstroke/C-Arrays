#include "array_funcs.h"

#include <errno.h> /* for errno, EINVAL */
#include <stdio.h>



extern int errno;


static bool _equals(const data *const e1, const data *const e2) {
	return e1 == e2;
}


void a_freer(Array *a, void (*const f)(data*)) {
	a_each(a, f);
	a_free(a);
}

data *a_remove(Array *const a, const data *const e, bool (*f)(const data*, const data*)) {
	if(!f) {
		if(!e) {
			errno = EINVAL;
			return NULL;
		}
		f = _equals;
	}
	const unsigned int s = a_size(a);
	for(unsigned int i = 0; i < s; ++i) {
		if(f(a_get(a, i), e)) {
			errno = 0;
			return a_drop(a, i);
		}
	}
	errno = EINVAL;
	return NULL;
}

void a_each(Array *const a, void (*f)(data*)) {
	const unsigned int s = a_size(a);
	for(unsigned int i = 0; i < s; ++i) {
		f(a_get(a, i));
	}
}

data *a_cond(const Array *a, const data *const e, bool (*f)(const data*, const data*)) {
	const unsigned int s = a_size(a);
	data *item;
	if(!f) {
		if(!e) {
			errno = EINVAL;
			return NULL;
		}
		f = _equals;
	}
	for(unsigned int i = 0; i < s; ++i) {
		item = a_get(a, i);
		if(f(item, e))
			return item;
	}
	errno = EINVAL;
	return NULL;
}


Array *a_make(const unsigned int n, data *const elements[static n]) {
	Array *const arr = a_new(n);
	if(!arr) {
		return NULL;
	}
	for(unsigned int i = 0; i < n; ++i) {
		a_append(arr, elements[i]);
	}
	return arr;
}

static void printitem_default(const data *item) {
	printf("%p", item);
}
void a_printf(const Array *a, void (*print)(const data*)) {
	const unsigned int n = a_size(a);
	printf("[");
	if(n) {
		if(!print)
			print = printitem_default;
		print(a_get(a, 0));
		for(unsigned int i = 1; i < n; ++i) {
			printf(", ");
			print(a_get(a, i));
		}
	}
	printf("]\n");
}
