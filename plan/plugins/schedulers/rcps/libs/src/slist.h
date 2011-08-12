#ifndef SLIST_H
#define SLIST_H

struct slist_node;
struct slist;

struct slist *slist_new(int (*data_cmp)(const void *, const void *));
void slist_free(struct slist *l, void (*free_data)(void *));

struct slist_node *slist_node_new(void *data);
void slist_node_free(struct slist_node *n, void (*free_data)(void *));
const void *slist_node_getdata(struct slist_node *n);

void slist_add(struct slist *l, struct slist_node *n);
int slist_add_nodup(struct slist *l, struct slist_node *n);
void slist_unlink(struct slist *l, struct slist_node *n);

struct slist_node *slist_at(struct slist *l, const int pos);
struct slist_node *slist_find_closest(struct slist *l, void *value);
struct slist_node *slist_find(struct slist *l, void *value);

struct slist_node *slist_next(const struct slist_node *n);
struct slist_node *slist_prev(const struct slist_node *n);
struct slist_node *slist_first(const struct slist *l);
struct slist_node *slist_last(const struct slist *l);
int slist_count(const struct slist *l);

#endif /* SLIST_H */
