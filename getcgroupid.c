// SPDX-License-Identifier: GPL-2.0

// Borrowed from https://github.com/torvalds/linux/blob/f269099a7e7a0c6732c4a817d0e99e92216414d9/tools/testing/selftests/bpf/cgroup_helpers.c

#define _GNU_SOURCE
#include <sched.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <errno.h>
#include <string.h>


#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_err(MSG, ...) fprintf(stderr, "(%s:%d: errno: %s) " MSG "\n", \
        __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)


/**
 * get_cgroup_id() - Get cgroup id for a particular cgroup path
 * @path: The cgroup path, relative to the workdir, to join
 *
 * On success, it returns the cgroup id. On failure it returns 0,
 * which is an invalid cgroup id.
 * If there is a failure, it prints the error to stderr.
 */
unsigned long long get_cgroup_id(char *path)
{
	int dirfd, err, flags, mount_id, fhsize;
	union {
		unsigned long long cgid;
		unsigned char raw_bytes[8];
	} id;
	struct file_handle *fhp, *fhp2;
	unsigned long long ret = 0;
 	dirfd = AT_FDCWD;
	flags = 0;
	fhsize = sizeof(*fhp);
	fhp = calloc(1, fhsize);
	if (!fhp) {
		log_err("calloc");
		return 0;
	}
	err = name_to_handle_at(dirfd, path, fhp, &mount_id, flags);
	if (err >= 0 || fhp->handle_bytes != 8) {
		log_err("name_to_handle_at");
		goto free_mem;
	}
 	fhsize = sizeof(struct file_handle) + fhp->handle_bytes;
	fhp2 = realloc(fhp, fhsize);
	if (!fhp2) {
		log_err("realloc");
		goto free_mem;
	}
	err = name_to_handle_at(dirfd, path, fhp2, &mount_id, flags);
	fhp = fhp2;
	if (err < 0) {
		log_err("name_to_handle_at");
		goto free_mem;
	}
 	memcpy(id.raw_bytes, fhp->f_handle, 8);
	ret = id.cgid;
 free_mem:
	free(fhp);
	return ret;
}

int main(int argc, char *argv[])
{
	unsigned long long id;

	if (argc != 2) {
		log_err("missing_param");
		return 1;
	}

	id = get_cgroup_id(argv[1]);

	printf("0x%llx\n%llu\n", id, id);
	return 0;
}
