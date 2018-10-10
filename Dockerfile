FROM alpine:3.8

COPY build-release/src/bpftrace /bin/
COPY getcgroupid /bin/
ENTRYPOINT ["/bin/bpftrace"]

# To build:
# docker build -t albanc/bpftrace .
# 
# To run:
# sudo docker run --rm --privileged -ti -v /sys/kernel/debug:/sys/kernel/debug albanc/bpftrace -e 'kprobe:__x64_sys_unshare /cgroup == 0x100000001/ { printf("unshare by %s\n", comm); }'
#
# sudo docker run --rm --privileged -ti -v /sys/kernel/debug:/sys/kernel/debug -v /sys/fs/cgroup/unified:/sys/fs/cgroup/unified --entrypoint=/bin/getcgroupid albanc/bpftrace /sys/fs/cgroup/unified
# 
