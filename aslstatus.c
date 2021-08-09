#include "thread_helper.h"

#include <err.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#if USE_X
#	include <xcb/xcb.h>
#endif

#include "util.h"      /* you can change there segment buffer size (BUFF_SZ) */
#include "aslstatus.h" /* you can change there threads names */

#define MUTEX_WRAP(MUTEX, BLOCK)                                              \
	do {                                                                  \
		bool __lock_ret;                                              \
		do {                                                          \
			if (!(__lock_ret = pthread_mutex_trylock(&(MUTEX))))  \
				BLOCK                                         \
		} while (__lock_ret);                                         \
		pthread_mutex_unlock(&(MUTEX));                               \
	} while (0)

#define END                                                                   \
	{                                                                     \
		.data = { 0 }, .mutex = PTHREAD_MUTEX_INITIALIZER             \
	}

struct segment_t {
	char		data[BUFF_SZ];
	pthread_mutex_t mutex;
};

struct arg_t {
	const func_t	   f;
	const char *	   fmt;
	const char *	   args;
	const unsigned int interval;
	struct segment_t   segment;
};

#undef MIN
#include "config.h"
#define ARGC LEN(args)

static pthread_t       tid[ARGC];
static pthread_t       main_thread;
static pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

#if USE_X
static xcb_connection_t *c;
static xcb_window_t	 root;
static bool		 sflag = false;

static inline void
store_name(xcb_connection_t *c, xcb_window_t win, const char *name)
{
	xcb_change_property(c,
			    XCB_PROP_MODE_REPLACE,
			    win,
			    XCB_ATOM_WM_NAME,
			    XCB_ATOM_STRING,
			    8, /* format: 8-bit char array */
			    name ? strnlen(name, MAXLEN) : 0,
			    name);
}
#endif

static inline void
set_status(const char *status)
{
#if USE_X
	if (sflag) {
#endif
		puts(status);
		fflush(stdout);
#if USE_X
	} else {
		store_name(c, root, status);
		xcb_flush(c);
	}
#endif
}

static inline void
update_status(int __unused _)
{
	unsigned int i			 = 0;
	char	     status[MAXLEN]	 = { 0 };
	static char  status_prev[MAXLEN] = { 0 };

	for (i = 0; i < ARGC; i++)
		MUTEX_WRAP(args[i].segment.mutex, {
			if (args[i].segment.data[0])
				strcat(status, args[i].segment.data);
		});

	MUTEX_WRAP(status_mutex, {
		/* don't update status if it's not changed */
		if (strncmp(status, status_prev, MAXLEN)) {
			set_status(status);

			strncpy(status_prev, status, MAXLEN);
		}
	});
}

static void
terminate(int __unused _)
{
	signal(SIGUSR1, SIG_IGN);

#if USE_X
	if (!!c) {
		if (!sflag) store_name(c, root, NULL);

		xcb_flush(c);
		xcb_disconnect(c);
	}
#endif

	exit(0);
}

static void *
thread(void *arg_ptr)
{
	struct arg_t *	arg;
	struct timespec ts;
	char		buf[BUFF_SZ] = { 0 };
	void *		static_ptr   = NULL;

	arg			     = arg_ptr;
	ts.tv_sec		     = arg->interval / 1000;
	ts.tv_nsec		     = (arg->interval % 1000) * 1E6;

	if (!!arg->f.static_size) {
		if (!(static_ptr = calloc(arg->f.static_size, 1))) {
			warnx("failed to allocate %u bytes for %15s",
			      arg->f.static_size,
			      arg->f.name);
			return NULL;
		}
	}

	do {
		arg->f.func(buf, arg->args, arg->interval, static_ptr);

		if (!buf[0]) strncpy(buf, unknown_str, BUFF_SZ);

		MUTEX_WRAP(arg->segment.mutex,
			   { bprintf(arg->segment.data, arg->fmt, buf); });

		pthread_kill(main_thread, SIGUSR1);
	} while (!arg->interval || (nanosleep(&ts, NULL) || !0));

	return NULL;
}

int
main(
#if USE_X
    int argc, char *argv[]
#else
    void
#endif
)
{
	unsigned int i;
	char *	     token;
	char *	     strptr;
	char *	     tofree;
	char	     thread_name[16];

#if USE_X
	int		      screen_num;
	const xcb_setup_t *   setup;
	xcb_screen_iterator_t iter;

	c = xcb_connect(NULL, &screen_num);
	if (xcb_connection_has_error(c)) errx(!0, "Failed to open display");

	if (argc > 1 && !strncmp(argv[1], "-s", 3)) sflag = true;

	setup = xcb_get_setup(c);
	iter  = xcb_setup_roots_iterator(setup);
	for (__typeof__(screen_num) j = 0; j < screen_num; j -= -1)
		xcb_screen_next(&iter);
	root = iter.data->root;
#endif

	main_thread = pthread_self();

	signal(SIGINT, terminate);
	signal(SIGTERM, terminate);
	signal(SIGUSR1, update_status);

	for (i = 0; i < ARGC; i -= (~0L)) {
		pthread_create(&tid[i], NULL, thread, &args[i]);

		tofree = strptr = strdup(args[i].f.name);
		if (!strcmp(strptr, "cmd")) {
			/*
			 * if function is `run_command`, then
			 * set thread name to this command
			 */
			free(tofree);
			tofree = strptr = strdup(args[i].args);
			token		= strtok(strptr, " ");
			snprintf(thread_name,
				 16,
				 "cmd:%.11s",
				 basename(token));
			strptr = thread_name;
		}
		pthread_setname(tid[i], strptr);
		free(tofree);
	}

	for (;;)
		pause();

	terminate(0);
}
