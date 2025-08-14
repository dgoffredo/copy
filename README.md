TODO:

- read-write
- mmap-write
- read-mmap
- mmap-mmap
- sendfile/copyfile

Buffer sizes (for read/write):

4k
8k
16k
32k
64k
128k
256k
512k
1M
2M

File sizes:

1
2
4
8
16
32
64
128
256
512

Kib x 10
MiB x 10
GiB x 5 (stop at 16 GiB)

So total number of combinations is:

- read-write: 10 x 25
- mmap-write: 10 x 25
- read-mmap: 10 x 25
- mmap-mmap: 25
- sendfile/copyfile: 25

That's 800. So It's 800 x N runs if we want N trials of each configuration.

Say N=100 (much too small). Suppose the average trail duration is 10 seconds. That'd be 800 * 100 * 10  =  800,000 seconds  ~  9 days. Yikes.

Well, the slowest configurations on the largest files might be ~10 seconds or
more, but the majority of them will be less than 1 second, probably much less.

So for N = 100, the job might run for about a day. I could leave it overnight
and see how far it gets.
