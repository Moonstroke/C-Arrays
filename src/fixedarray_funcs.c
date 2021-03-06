#include "fixedarray_funcs.h"

#include <errno.h> /* for errno */
#include <stdio.h> /* for printf() */



extern int errno;


static bool default_equals(const data_t *const e1, const data_t *const e2) {
	return e1 == e2;
}

static void default_print_item(const data_t *const item) {
	printf("%p", item);
}


void fa_freer(FixedArray *const fa, void (*const f)(data_t*)) {
	fa_clear(fa, f);
	fa_free(fa);
}

void fa_clear(FixedArray *const fa, void (*const f)(data_t*)) {
	const size_t s = fa_size(fa);
	if(f) {
		fa_each(fa, f);
	}
	for(size_t i = 0; i < s; ++i) {
		fa_unset(fa, i);
	}
}

size_t fa_count(const FixedArray *const fa) {
	const size_t s = fa_size(fa);
	size_t n = 0;
	for(size_t i = 0; i < s; ++i) {
		if(fa_get(fa, i))
			++n;
	}
	return n;
}

ssize_t fa_put(FixedArray *const fa, data_t *const item) {
	const size_t s = fa_size(fa);
	for(size_t i = 0; i < s; ++i)
		if(!fa_get(fa, i)) {
			fa_set(fa, i, item);
			return i;
		}
	return -1;
}

data_t *fa_swap(FixedArray *const fa, const size_t i, data_t *const e) {
	data_t *const d = fa_get(fa, i); /* this sets errno */
	if(errno) {
		return NULL;
	} else {
		fa_set(fa, i, e);
		return d;
	}
}
extern data_t *fa_unset(FixedArray*, size_t);


data_t *fa_cond(const FixedArray *fa, bool (*f)(const data_t*, const data_t*), const data_t *v) {
	const size_t s = fa_size(fa);
	data_t *e;
	if(!f) {
		f = default_equals;
	}
	for(size_t i = 0; i < s; ++i) {
		e = fa_get(fa, i);
		if(f(e, v)) {
			errno = 0;
			return e;
		}
	}
	errno = EINVAL;
	return NULL;
}

data_t *fa_remove(FixedArray *fa, bool (*f)(const data_t*, const data_t*), const data_t *const v) {
	const size_t s = fa_size(fa);
	if(!f) {
		f = default_equals;
	}
	for(size_t i = 0; i < s; ++i) {
		if(f(fa_get(fa, i), v)) {
			errno = 0;
			return fa_unset(fa, i);
		}
	}
	errno = EINVAL;
	return NULL;
}

void fa_each(FixedArray *const fa, void (*const f)(data_t*)) {
	const size_t s = fa_size(fa);
	for(size_t i = 0; i < s; ++i) {
		data_t *const item = fa_get(fa, i);
		if(item)
			f(item);
	}
}

void fa_printf(const FixedArray *const fa, void (*p)(const data_t*)) {
	const size_t s = fa_size(fa);
	printf("[");
	if(s) {
		if(!p)
			p = default_print_item;
		p(fa_get(fa, 0));
		for(size_t i = 1; i < s; ++i) {
			printf(", ");
			p(fa_get(fa, i));
		}
	}
	printf("]\n");
}
