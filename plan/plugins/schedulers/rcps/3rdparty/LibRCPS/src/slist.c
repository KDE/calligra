#include <assert.h>
#include <stdio.h>
#ifdef _WIN32
# include <malloc.h>
#else
#include <stdlib.h>
#endif
#include <string.h>

#include "slist.h"

#define NODE_LEFT		0
#define NODE_RIGHT		1
#define NODE_FIRST		0
#define NODE_LAST		1
#define NODE_PREV		0
#define NODE_NEXT		1
#define ROTATE_LEFT		0
#define ROTATE_RIGHT	1
#define ADD_BEFORE		0
#define ADD_AFTER		1

struct slist_node {
	struct slist_node *parent;
	struct slist_node *children[2];
	struct slist_node *brothers[2];
	int count;
	int height;
	void *data;
};

struct slist {
	struct slist_node *root;
	struct slist_node *ends[2];
	int (*data_cmp)(const void *, const void *);
};

struct slist *slist_new(int (*data_cmp)(const void *, const void *)) {
	struct slist *l = (struct slist*)malloc(sizeof(struct slist));
	assert(l != NULL);
	assert(data_cmp != NULL);
	l->root = NULL;
	l->ends[NODE_FIRST] = NULL;
	l->ends[NODE_LAST] = NULL;
	l->data_cmp = data_cmp;
	return l;
}

void slist_free(struct slist *l, void (*free_data)(void *)) {
	struct slist_node *cn;
	struct slist_node *on;
	assert(l != NULL);
	
	cn = l->ends[NODE_FIRST];
	while (cn) {
		on = cn;
		cn = slist_next(cn);
		if (free_data != NULL) {
			free_data(on->data);
		}
		free(on);
	}
	memset(l, 0, sizeof(struct slist));
	free(l);
}

struct slist_node *slist_node_new(void *data) {
	struct slist_node *n = (struct slist_node*)malloc(
		sizeof(struct slist_node));
	memset(n, 0, sizeof(struct slist_node));
	n->data = data;
	return n;
}
void slist_node_free(struct slist_node *n, void (*free_data)(void *)) {
	assert(n != NULL);
	if (free_data != NULL) {
		free_data(n->data);
	}
	memset(n, 0, sizeof(struct slist_node));
	free(n);
}

const void *slist_node_getdata(struct slist_node *n) {
	assert(n != NULL);
	return n->data;
}

struct slist_node *slist_find_closest(struct slist *l, void *value) {
	struct slist_node *n;
	int c;

	assert(l != NULL);
	
	n = l->root;
	if (n == NULL) {
		return NULL;
	}

	while (1) {
		c = l->data_cmp(n->data, value);
		if (c > 0) {
			if (n->children[NODE_LEFT]) {
				n = n->children[NODE_LEFT];
			}
			else {
				return n;
			}
		}
		else if (c < 0) {
			if (n->children[NODE_RIGHT]) {
				n = n->children[NODE_RIGHT];
			}
			else {
				return n;
			}
		}
		else {
			return n;
		}
	}
}

struct slist_node *slist_find(struct slist *l, void *value) {
	struct slist_node *n = slist_find_closest(l, value);
	if (n == NULL) {
		return n;
	}
	if (l->data_cmp(n->data, value) == 0) {
		return n;
	}
	return NULL;
}

void slist_add_root(struct slist *l, struct slist_node *n) {
	assert(l != NULL);
	assert(l->root == NULL);
	assert(n != NULL);
	
	l->root = n;
	l->ends[NODE_FIRST] = n;
	l->ends[NODE_LAST] = n;
	n->parent = NULL;
	n->children[NODE_LEFT] = NULL;
	n->children[NODE_RIGHT] = NULL;
	n->count = 1;
	n->height = 1;
	n->brothers[NODE_PREV] = NULL;
	n->brothers[NODE_NEXT] = NULL;
}

/*inline*/ void slist_inc_count(struct slist_node *n) {
	while (n) {
		n->count++;
		n = n->parent;
	}
}

/*inline*/ void slist_dec_count(struct slist_node *n) {
	while (n) {
		n->count--;
		n = n->parent;
	}
}

/*inline*/ int hmax(const int a, const int b) {
	return a > b ? a : b;
}

void slist_fixheight(struct slist_node *n) {
	int c;
	c = hmax(
			n->children[NODE_LEFT] ? n->children[NODE_LEFT]->height : 0,
			n->children[NODE_RIGHT] ? n->children[NODE_RIGHT]->height : 0
			) + 1;
	if (c != n->height) {
		n->height = c;
		if (n->parent != NULL) {
			slist_fixheight(n->parent);
		}
	}
}

struct slist_node *slist_sr(struct slist *l, struct slist_node *n, int dir) {
	struct slist_node *tn;
	struct slist_node *pn;
	struct slist_node **sn;

	pn = n->parent;
	tn = n->children[!dir];

	/* update the counts */
	n->count -= tn->count;
	if (tn->children[dir]) {
		n->count += tn->children[dir]->count;
		tn->count -= tn->children[dir]->count;
	}
	tn->count += n->count;
	/* rotate */	
	n->children[!dir] = tn->children[dir];
	if (tn->children[dir]) {
		tn->children[dir]->parent = n;
	}
	tn->children[dir] = n;
	n->parent = tn;
	tn->parent = pn;
	slist_fixheight(n);
	/* update pn */
	if (pn) {
		sn = pn->children[!dir] == n ? 
			&pn->children[!dir] : 
			&pn->children[dir];
		*sn = tn;
	}
	else {
		l->root = tn;
	}
	return pn;
}

/*inline*/ struct slist_node *slist_dr(struct slist *l, struct slist_node *n,
		int dir) {
	slist_sr(l, n->children[dir], dir);
	return slist_sr(l, n, !dir);
}

void slist_rebalance(struct slist *l, struct slist_node *n) {
	int c;
	int cs;

	assert(l != NULL);
	assert(n != NULL);
			
	while (n) {
		c = (n->children[NODE_LEFT] ? 
				n->children[NODE_LEFT]->height : 0) 
			- (n->children[NODE_RIGHT] ? 
					n->children[NODE_RIGHT]->height: 0);
		if (c > 1) {
			cs = (n->children[NODE_LEFT]->children[NODE_LEFT] ? 
					n->children[NODE_LEFT]->children[NODE_LEFT]->height : 0) 
				- (n->children[NODE_LEFT]->children[NODE_RIGHT] ? 
				 n->children[NODE_LEFT]->children[NODE_RIGHT]->height : 0);
			if (cs < 0) {
				n = slist_dr(l, n, ROTATE_LEFT);
			}
			else {
				n = slist_sr(l, n, ROTATE_RIGHT);
			}
		}
		else if (c < -1) {
			cs = (n->children[NODE_RIGHT]->children[NODE_LEFT] ? 
					n->children[NODE_RIGHT]->children[NODE_LEFT]->height : 0) 
				- (n->children[NODE_RIGHT]->children[NODE_RIGHT] ? 
				 n->children[NODE_RIGHT]->children[NODE_RIGHT]->height : 0);
			if (cs > 0) {
				n = slist_dr(l, n, ROTATE_RIGHT);
			}
			else {
				n = slist_sr(l, n, ROTATE_LEFT);
			}
		}
		else {
			slist_fixheight(n);
			n = n->parent;
		}
	}
}

void slist_add_relative(struct slist *l, struct slist_node *o, 
		struct slist_node *n, int dir) {

	assert(l != NULL);
	assert(o != NULL);
	assert(n != NULL);

	if (o->children[dir]) {
		slist_add_relative(l, o->brothers[dir], n, !dir);
	}
	else {
		o->children[dir] = n;
		n->parent = o;
		n->height = 1;
		n->children[dir] = NULL;
		n->children[!dir] = NULL;
		n->count = 1;
		n->brothers[!dir] = o;
		n->brothers[dir] = o->brothers[dir];
		o->brothers[dir] = n;
		if (n->brothers[dir] != NULL) {
			n->brothers[dir]->brothers[!dir] = n;
		}
		else {
			l->ends[dir] = n;
		}
		slist_inc_count(n->parent);
		slist_rebalance(l, n->parent);
	}
}

int slist_add_nodup(struct slist *l, struct slist_node *n) {
	struct slist_node *o;
	int c;
	
	assert(l != NULL);
	assert(n != NULL);

	if (!l->root) {
		slist_add_root(l, n);
	}
	else {
		o = slist_find_closest(l, n->data);	
		assert(o != NULL);
		c = l->data_cmp(n->data, o->data);
		if (c == 0) {
			return 0;
		}
		slist_add_relative(l, o, n, c < 0 ? ADD_BEFORE : ADD_AFTER);
	}
	return 1;
}

void slist_add(struct slist *l, struct slist_node *n) {
	struct slist_node *o;
	int c;
	
	assert(l != NULL);
	assert(n != NULL);

	if (!l->root) {
		slist_add_root(l, n);
	}
	else {
		o = slist_find_closest(l, n->data);	
		assert(o != NULL);
		c = l->data_cmp(n->data, o->data);
		slist_add_relative(l, o, n, c < 0 ? ADD_BEFORE : ADD_AFTER);
	}
}

void slist_unlink(struct slist *l, struct slist_node *n) {
	struct slist_node *pn;
	struct slist_node *bn;
	struct slist_node *ln;
	struct slist_node *rn;
	struct slist_node *su;
	struct slist_node *cn;
	struct slist_node **sn;

	assert(l != NULL);
	assert(n != NULL);
	
	bn = NULL;
	pn = n->parent;
	ln = n->children[NODE_LEFT];
	rn = n->children[NODE_RIGHT];

	// fix the linked list
	if (n->brothers[NODE_NEXT] != NULL) {
		n->brothers[NODE_NEXT]->brothers[NODE_PREV] = n->brothers[NODE_PREV];
	}
	else {
		l->ends[NODE_LAST] = n->brothers[NODE_PREV];
	}
	if (n->brothers[NODE_PREV] != NULL) {
		n->brothers[NODE_PREV]->brothers[NODE_NEXT] = n->brothers[NODE_NEXT];
	}
	else {
		l->ends[NODE_FIRST] = n->brothers[NODE_NEXT];
	}
	
	if (!ln) {
		if (pn && (pn->children[NODE_LEFT] == n)) {
			pn->children[NODE_LEFT] = rn;
		}
		else if (pn) {
			pn->children[NODE_RIGHT] = rn;
		}
		else {
			l->root = rn;
		}
		if (rn) {
			rn->parent = pn;
		}
		bn = pn;
	}
	else if (!rn) {
		if (pn && (pn->children[NODE_LEFT] == n)) {
			pn->children[NODE_LEFT] = ln;
		}
		else if (pn) {
			pn->children[NODE_RIGHT] = ln;
		}
		else {
			l->root = ln;
		}
		if (ln) {
			ln->parent = pn;
		}
		bn = pn;
	}
	else {
		su = ln;
		while (su->children[NODE_RIGHT]) {
			su = su->children[NODE_RIGHT];
		}
		// su is the largest node that is smaller than n
		if (su == ln) {
			// in this case we can simply shift the node up
			su->children[NODE_RIGHT] = rn;
			su->count += rn ? rn->count : 0;
			su->height = hmax(su->height, rn ? rn->height+1 : 1);
			// rebalancing needed at this node
			bn = pn;
			rn->parent = su;
		}
		else {
			if (su->children[NODE_LEFT]) {
				su->parent->children[NODE_RIGHT] = su->children[NODE_LEFT];
				su->children[NODE_LEFT]->parent = su->parent;
			}
			else {
				su->parent->children[NODE_RIGHT] = NULL;
			}
			cn = su->parent;
			while (cn != n) {
				cn->count = (cn->children[NODE_LEFT] ? 
					cn->children[NODE_LEFT]->count : 0) + 
					(cn->children[NODE_RIGHT] ? 
					cn->children[NODE_RIGHT]->count : 0) + 1;
				cn->height = hmax (
					(cn->children[NODE_LEFT] ? 
					cn->children[NODE_LEFT]->height : 0),
					(cn->children[NODE_RIGHT] ? 
					cn->children[NODE_RIGHT]->height : 0))+1;
				cn = cn->parent;
			}
			assert(ln);
			assert(rn);
			su->children[NODE_LEFT] = ln;
			ln->parent = su;
			su->children[NODE_RIGHT] = rn;
			rn->parent = su;
			su->count = (rn ? rn->count : 0) + (ln ? ln->count : 0) + 1;
			su->height = hmax(ln ? ln->height : 0, rn ? rn->height : 0) + 1;
			// rebalancing needed
			bn = pn;
		}
		// fix the pn pointer to the node
		su->parent = pn;
		if (pn) {
			sn = pn->children[NODE_RIGHT] == n ? 
				&pn->children[NODE_RIGHT] : 
				&pn->children[NODE_LEFT];
			*sn = su;
		}
		else {
			l->root = su;
		}
	}
	slist_dec_count(pn);
	if (bn) {
		slist_rebalance(l, bn);
	}
	n->parent = NULL;
	n->children[NODE_LEFT] = NULL;
	n->children[NODE_RIGHT] = NULL;
	n->brothers[NODE_LEFT] = NULL;
	n->brothers[NODE_RIGHT] = NULL;
}

struct slist_node *slist_at(struct slist *l, int pos) {
	struct slist_node *n;
	int c;

	assert(l != NULL);

	n = l->root;

	while(n) {
		c = n->children[NODE_LEFT] ? n->children[NODE_LEFT]->count : 0;
		if (c > pos) {
			n = n->children[NODE_LEFT];
		}
		else if (c < pos) {
			n = n->children[NODE_RIGHT];
			pos -= c + 1;
		}
		else {
			return n;
		}
	}
	return NULL;
}

struct slist_node *slist_next(const struct slist_node *n) {
	return n->brothers[NODE_NEXT];
}

struct slist_node *slist_prev(const struct slist_node *n) {
	return n->brothers[NODE_PREV];
}

struct slist_node *slist_first(const struct slist *l) {
	return l->ends[NODE_FIRST];
}

struct slist_node *slist_last(const struct slist *l) {
	return l->ends[NODE_LAST];
}

int slist_count(const struct slist *l) {
	return l->root ? l->root->count : 0;
}
