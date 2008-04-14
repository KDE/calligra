#include <windows.h>
#include <assert.h>

#include "shttpd.h"

#define	MAX_CONNECTIONS_PER_THREAD	20

struct thread {
	struct thread		*next;
	struct shttpd_ctx	*ctx;
};

static struct thread	*threads;	/* List of worker threads */


static struct shttpd_ctx *
create_new_context(void)
{
	return (shttpd_init(NULL, "document_root", "c:\\", NULL));
}

static DWORD WINAPI thread_function(void *param)
{
	struct thread *thread = param;

	for (;;)
		shttpd_poll(thread->ctx, 100);
}

static struct thread *
spawn_new_thread(void)
{
	struct shttpd_ctx	*ctx;
	struct thread		*thread;
	DWORD			tid;

	thread	= malloc(sizeof(*thread));
	ctx	= create_new_context();

	assert(ctx != NULL);
	assert(thread != NULL);

	thread->ctx	= ctx;
	thread->next	= threads;
	threads		= thread;

	CreateThread(NULL, 0, thread_function, thread, 0, &tid);

	return (thread);
}

static struct thread *
find_not_busy_thread(void)
{
	struct thread	*thread;

	for (thread = threads; thread != NULL; thread = thread->next)
		if (shttpd_active(thread->ctx) < MAX_CONNECTIONS_PER_THREAD)
			return (thread);

	return (NULL);
}

int main(int argc, char *argv[])
{
	struct shttpd_ctx	*ctx;
	struct thread		*thread;
	int			lsn, sock;

	ctx = shttpd_init(NULL, NULL);
	lsn = shttpd_listen(ctx, 80, 0);

	for (;;) {
		if ((sock = shttpd_accept(lsn, 1000)) == -1) 
			continue;

		if ((thread = find_not_busy_thread()) == NULL)
			thread = spawn_new_thread();

		shttpd_add_socket(thread->ctx, sock);
	}
		
}
