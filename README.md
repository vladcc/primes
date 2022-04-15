# primes
prime numbers generator

Uses a segmented sieve to generate primes in a range up to UINT64_MAX

```
$ ./primes.bin 
Use: primes [option] [start] <stop>
Try: primes --help
```
```
$ ./primes.bin --help
primes -- generates prime numbers in a range up to uint64_t max
Use: primes [option] [start] <stop>

If [start] is not given, or is less than 2, 2 is used.

Options:
   --count   - print the approx. number of primes from 2 up to <stop>
   --memory  - shows approx. how much memory is needed for the given range
   --limit   - shows the max number limit
   --help    - this screen
   --version - version info
```
```
$ ./primes.bin 30 50
31
37
41
43
47
```
