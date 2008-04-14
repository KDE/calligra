/*
 * This file is an example of how to embed web-server functionality
 * into existing application.
 * Compilation line:
 * cc example.c shttpd.c -DEMBEDDED
 */

#ifdef _WIN32
#include <winsock.h>
#define	snprintf			_snprintf

#ifndef _WIN32_WCE
#ifdef _MSC_VER /* pragmas not valid on MinGW */
#pragma comment(lib,"ws2_32")
#endif /* _MSC_VER */
#define ALIAS_URI "/my_c"
#define ALIAS_DIR "c:\\"

#else /* _WIN32_WCE */
/* Windows CE-specific definitions */
#pragma comment(lib,"ws2")
//#include "compat_wince.h"
#define ALIAS_URI "/my_root"
#define ALIAS_DIR "\\"
#endif /* _WIN32_WCE */

#else
#include <sys/types.h>
#include <sys/select.h>
#define ALIAS_URI "/my_etc"
#define ALIAS_DIR "/etc/"
#endif

#ifndef _WIN32_WCE /* Some ANSI #includes are not available on Windows CE */
#include <time.h>
#include <errno.h>
#include <signal.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "shttpd.h"

/*
 * This callback function is attached to the "/" and "/abc.html" URIs,
 * thus is acting as "index.html" file. It shows a bunch of links
 * to other URIs, and allows to change the value of program's
 * internal variable. The pointer to that variable is passed to the
 * callback function as arg->user_data.
 */
static void
show_index(struct shttpd_arg *arg)
{
	int		*p = arg->user_data;	/* integer passed to us */
	char		value[20];
	const char	*host, *request_method, *query_string, *request_uri;

	request_method = shttpd_get_env(arg, "REQUEST_METHOD");
	request_uri = shttpd_get_env(arg, "REQUEST_URI");
	query_string = shttpd_get_env(arg, "QUERY_STRING");

	/* Change the value of integer variable */
	value[0] = '\0';
	if (!strcmp(request_method, "POST")) {
		/* If not all data is POSTed, wait for the rest */
		if (arg->flags & SHTTPD_MORE_POST_DATA)
			return;
		(void) shttpd_get_var("name1", arg->in.buf, arg->in.len,
		    value, sizeof(value));
	} else if (query_string != NULL) {
		(void) shttpd_get_var("name1", query_string,
		    strlen(query_string), value, sizeof(value));
	}
	if (value[0] != '\0') {
		*p = atoi(value);

		/*
		 * Suggested by Luke Dunstan. When POST is used,
		 * send 303 code to force the browser to re-request the
		 * page using GET method. This prevents the possibility of
		 * the user accidentally resubmitting the form when using
		 * Refresh or Back commands in the browser.
		 */
		if (!strcmp(request_method, "POST")) {
			shttpd_printf(arg, "HTTP/1.1 303 See Other\r\n"
				"Location: %s\r\n\r\n", request_uri);
			arg->flags |= SHTTPD_END_OF_OUTPUT;
			return;
		}
	}

	shttpd_printf(arg, "%s",
		"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
		"<html><body><h1>Welcome to embedded example of SHTTPD");
	shttpd_printf(arg, " v. %s </h1><ul>", shttpd_version());

	shttpd_printf(arg, "<li><code>REQUEST_METHOD: %s "
	    "REQUEST_URI: \"%s\" QUERY_STRING: \"%s\""
	    " REMOTE_ADDR: %s REMOTE_USER: \"(null)\"</code><hr>",
	    request_method, request_uri,
	    query_string ? query_string : "(null)",
	    shttpd_get_env(arg, "REMOTE_ADDR"));
	shttpd_printf(arg, "<li>Internal int variable value: <b>%d</b>", *p);

	shttpd_printf(arg, "%s",
		"<form method=\"GET\">Enter new value: "
		"<input type=\"text\" name=\"name1\"/>"
		"<input type=\"submit\" "
		"value=\"set new value using GET method\"></form>");
	shttpd_printf(arg, "%s",
		"<form method=\"POST\">Enter new value: "
		"<input type=\"text\" name=\"name1\"/>"
		"<input type=\"submit\" "
		"value=\"set new value using POST method\"></form>");
		
	shttpd_printf(arg, "%s",
		"<hr><li><a href=\"/secret\">"
		"Protected page</a> (guest:guest)<hr>"
		"<li><a href=\"/huge\">Output lots of data</a><hr>"
		"<li><a href=\"" ALIAS_URI "/\">Aliased " ALIAS_DIR " directory</a><hr>");
	shttpd_printf(arg, "%s",
		"<li><a href=\"/Makefile\">Regular file (Makefile)</a><hr>"
		"<li><a href=\"/ssi_test.shtml\">SSI file "
			"(ssi_test.shtml)</a><hr>"
		"<li><a href=\"/users/joe/\">Wildcard URI example</a><hr>"
		"<li><a href=\"/not-existent/\">Custom 404 handler</a><hr>");

	host = shttpd_get_header(arg, "Host");
	shttpd_printf(arg, "<li>'Host' header value: [%s]<hr>",
	    host ? host : "NOT SET");

	shttpd_printf(arg, "<li>Upload file example. "
	    "<form method=\"post\" enctype=\"multipart/form-data\" "
	    "action=\"/post\"><input type=\"file\" name=\"file\">"
	    "<input type=\"submit\"></form>");

	shttpd_printf(arg, "%s", "</body></html>");
	arg->flags |= SHTTPD_END_OF_OUTPUT;
}

/*
 * This callback is attached to the URI "/post"
 * It uploads file from a client to the server. This is the demostration
 * of how to use POST method to send lots of data from the client.
 * The uploaded file is saved into "uploaded.txt".
 * This function is called many times during single request. To keep the
 * state (how many bytes we have received, opened file etc), we allocate
 * a "struct state" structure for every new connection.
 */
static void
show_post(struct shttpd_arg *arg)
{
	const char	*s, *path = "uploaded.txt";
	struct state {
		size_t	cl;		/* Content-Length	*/
		size_t	nread;		/* Number of bytes read	*/
		FILE	*fp;
	} *state;

	/* If the connection was broken prematurely, cleanup */
	if (arg->flags & SHTTPD_CONNECTION_ERROR && arg->state) {
		(void) fclose(((struct state *) arg->state)->fp);
		free(arg->state);
	} else if ((s = shttpd_get_header(arg, "Content-Length")) == NULL) {
		shttpd_printf(arg, "HTTP/1.0 411 Length Required\n\n");
		arg->flags |= SHTTPD_END_OF_OUTPUT;
	} else if (arg->state == NULL) {
		/* New request. Allocate a state structure, and open a file */
		arg->state = state = calloc(1, sizeof(*state));
		state->cl = strtoul(s, NULL, 10);
		state->fp = fopen(path, "wb+");
		shttpd_printf(arg, "HTTP/1.0 200 OK\n"
			"Content-Type: text/plain\n\n");
	} else {
		state = arg->state;

		/*
		 * Write the POST data to a file. We do not do any URL
		 * decoding here. File will contain form-urlencoded stuff.
		 */
		(void) fwrite(arg->in.buf, arg->in.len, 1, state->fp);
		state->nread += arg->in.len;

		/* Tell SHTTPD we have processed all data */
		arg->in.num_bytes = arg->in.len;

		/* Data stream finished? Close the file, and free the state */
		if (state->nread >= state->cl) {
			shttpd_printf(arg, "Written %d bytes to %s",
			    state->nread, path);
			(void) fclose(state->fp);
			free(state);
			arg->flags |= SHTTPD_END_OF_OUTPUT;
		}
	}
}

/*
 * This callback function is attached to the "/secret" URI.
 * It shows simple text message, but in order to be shown, user must
 * authorized himself against the passwords file "passfile".
 */
static void
show_secret(struct shttpd_arg *arg)
{
	shttpd_printf(arg, "%s", "HTTP/1.1 200 OK\r\n");
	shttpd_printf(arg, "%s", "Content-Type: text/html\r\n\r\n");
	shttpd_printf(arg, "%s", "<html><body>");
	shttpd_printf(arg, "%s", "<p>This is a protected page</body></html>");
	arg->flags |= SHTTPD_END_OF_OUTPUT;
}

/*
 * This callback function is attached to the "/huge" URI.
 * It outputs binary data to the client.
 * The number of bytes already sent is stored directly in the arg->state.
 */
static void
show_huge(struct shttpd_arg *arg)
{
	int		state = (int) arg->state;

	if (state == 0) {
		shttpd_printf(arg, "%s", "HTTP/1.1 200 OK\r\n");
		shttpd_printf(arg, "%s", "Content-Type: text/plain\r\n\r\n");
	}

	while (arg->out.num_bytes < arg->out.len) {
		arg->out.buf[arg->out.num_bytes] = state % 72 ? 'A' : '\n';
		arg->out.num_bytes++;
		state++;

		if (state > 1024 * 1024) {	/* Output 1Mb Kb of data */
			arg->flags |= SHTTPD_END_OF_OUTPUT;
			break;
		}
	}
	
	arg->state = (void *) state;
}

/*
 * This callback function is used to show how to handle 404 error
 */
static void
show_404(struct shttpd_arg *arg)
{
	shttpd_printf(arg, "%s", "HTTP/1.1 200 OK\r\n");
	shttpd_printf(arg, "%s", "Content-Type: text/plain\r\n\r\n");
	shttpd_printf(arg, "%s", "Oops. File not found! ");
	shttpd_printf(arg, "%s", "This is a custom error handler.");
	arg->flags |= SHTTPD_END_OF_OUTPUT;
}

/*
 * This callback function is attached to the wildcard URI "/users/.*"
 * It shows a greeting message and an actual URI requested by the user.
 */
static void
show_users(struct shttpd_arg *arg)
{
	shttpd_printf(arg, "%s", "HTTP/1.1 200 OK\r\n");
	shttpd_printf(arg, "%s", "Content-Type: text/html\r\n\r\n");
	shttpd_printf(arg, "%s", "<html><body>");
	shttpd_printf(arg, "%s", "<h1>Hi. This is a wildcard uri handler"
	    "for the URI /users/*/ </h1>");
	shttpd_printf(arg, "<h2>URI: %s</h2></body></html>",
		shttpd_get_env(arg, "REQUEST_URI"));
	arg->flags |= SHTTPD_END_OF_OUTPUT;
}

/*
 * This function will be called on SSI directive <!--#if true -->, or
 * <!--#elif true -->, and 'returns' TRUE condition
 */
static void
ssi_test_true(struct shttpd_arg	*arg)
{
	arg->flags |= SHTTPD_SSI_EVAL_TRUE;
}

/*
 * This function will be called on SSI directive <!--#if false -->, or
 * <!--#elif false -->, and 'returns' FALSE condition
 */
static void
ssi_test_false(struct shttpd_arg *arg)
{
	/* Do not set SHTTPD_SSI_EVAL_TRUE flag, that means FALSE */
}

/*
 * This function will be called on SSI <!--#call print_stuff -->
 */
static void
ssi_print_stuff(struct shttpd_arg *arg)
{
	time_t	t = time(NULL);

	shttpd_printf(arg,
	    "SSI user callback output: Current time: %s", ctime(&t));
	if (arg->in.buf != NULL)
		shttpd_printf(arg, "SSI param passed: [%s]", arg->in.buf);
	arg->flags |= SHTTPD_END_OF_OUTPUT;
}

int main(int argc, char *argv[])
{
	int			data = 1234567;
	struct shttpd_ctx	*ctx;
	
	/* Get rid of warnings */
	argc = argc;
	argv = argv;

#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif /* !_WIN32 */

	/*
	 * Initialize SHTTPD context.
	 * Attach folder c:\ to the URL /my_c  (for windows), and
	 * /etc/ to URL /my_etc (for UNIX). These are Apache-like aliases.
	 * Set WWW root to current directory.
	 * Start listening on ports 8080 and 8081
	 */
	ctx = shttpd_init();
	shttpd_set_option(ctx, "ssl_cert", "shttpd.pem");
	shttpd_set_option(ctx, "aliases", ALIAS_URI "=" ALIAS_DIR);
	shttpd_set_option(ctx, "ports", "8080,8081s");

	/* Register an index page under two URIs */
	shttpd_register_uri(ctx, "/", &show_index, (void *) &data);
	shttpd_register_uri(ctx, "/abc.html", &show_index, (void *) &data);

	/* Register a callback on wildcard URI */
	shttpd_register_uri(ctx, "/users/*/", &show_users, NULL);

	/* Show how to use password protection */
	shttpd_register_uri(ctx, "/secret", &show_secret, NULL);
	shttpd_set_option(ctx, "protect", "/secret=passfile");

	/* Show how to use stateful big data transfer */
	shttpd_register_uri(ctx, "/huge", &show_huge, NULL);

	/* Register URI for file upload */
	shttpd_register_uri(ctx, "/post", &show_post, NULL);

	/* Register SSI callbacks */
	shttpd_register_ssi_func(ctx, "true", ssi_test_true, NULL);
	shttpd_register_ssi_func(ctx, "false", ssi_test_false, NULL);
	shttpd_register_ssi_func(ctx, "print_stuff", ssi_print_stuff, NULL);

	shttpd_handle_error(ctx, 404, show_404, NULL);

	/* Serve connections infinitely until someone kills us */
	for (;;)
		shttpd_poll(ctx, 1000);

	/* Probably unreached, because we will be killed by a signal */
	shttpd_fini(ctx);

	return (EXIT_SUCCESS);
}
