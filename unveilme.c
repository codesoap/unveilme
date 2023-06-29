#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
usage() {
	fprintf(stderr, "usage: unveilme <path-permissions> program [argument ...]\n");
	fprintf(stderr, "       unveilme -p <path-permissions> ... program [argument ...]\n");
	exit(1);
}

void
unveil_path_permissions(char* arg) {
	char* i = strrchr(arg, ':');
	if (i == NULL) {
		usage();
	}
	*i = 0;
	if (unveil(arg, i+1) == -1) {
		errx(1, "unveil");
	}
}

int
main(int argc, char **argv)
{
	if (argc < 3) {
		usage();
	}

	int ch;
	int p_seen = 0;
	while ((ch = getopt(argc, argv, "p:")) != -1) {
		if (ch != 'p') {
			usage();
		}
		p_seen = 1;
		unveil_path_permissions(optarg);
	}
	argc -= optind;
	argv += optind;

	if (!p_seen) {
		unveil_path_permissions(argv[0]);
		argc--;
		argv++;
	}

	if (argc < 1) {
		usage();
	}

	// Further calls to unveil are disabled:
	if (unveil(NULL, NULL) == -1) {
		errx(1, "unveil");
	}

	errno = 0;
	execvp(argv[0], &argv[0]);
	err((errno == ENOENT) ? 127 : 126, "%s", argv[0]);
}
