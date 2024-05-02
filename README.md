## crc32_simd: SIMD accelerated CRC-32 calculation

This is a simple C++ framework for implementing and testing SIMD
accelerated Cyclic Redundancy Calculation checks, using straight and
reflected polynomials. The CRC-32 polynomials defined in ISO 3309 and
by Castagnoli et al. (CRC-32C) are included.

The reflected ISO 3309 polynomial implementation `crc32_refl`,
`crc32_refl_vpclmulqdq` is what is commonly called `crc32()` and
implemented in `zlib` and in many other places.

A SIMD accelerated implementation for the AVX512 family of
IA-32 and AMD64 ISA extensions is included. It is based on the
assembler implementation in https://github.com/intel/intel-ipsec-mb/.

### Building and Testing

The necessary bits of AVX512 support (most notably, AVX512BW to allow
unaligned 64-byte loads) was introduced in Clang 8 and GCC 11.

This has been tested on an Intel® Core™ i7-6500U CPU

You can try it out as follows:
```sh
mkdir build
cd build
cmake ..
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++-11 ..
#cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_COMPILER=clang++-18 ..
#cmake -DCMAKE_CXX_FLAGS='-m32 -g' ..
cmake --build .
test/test_crc32
# Microsoft Windows:
test/Debug/test_crc32
```
The output of the `test_crc32` program should be as follows:
```
AVX512 or VPCLMULQDQ is not available.
```
or
```
Testing AVX512+VPCLMULQDQ: CRC-32C, CRC-32, CRC-32C (refl), CRC-32 (refl).
```

You can also use the test program to compute various CRC on arbitrary
files, by specifying the algorithm and its implementation, followed by
a number of file names. This could be useful for performance
evaluation. The interface is rather rudimentary, with minimal
diagnostics on errors (returning `EXIT_FAILURE` on any failure):
```sh
test/test_crc32 'CRC-32 (refl)' 0 ../lib/crc32_avx512.cc ../test/test_crc32.cc
```
This should report something like the following:
```
0616edf0	../lib/crc32_avx512.cc
42a3a831	../test/test_crc32.cc
```
Notably, the above output should be identical to that of the `crc32`
utility from the Debian `libarchive-zip-perl` package, when invoked on
the same files.

Implementation `0` is the reference implementation. If you request
an implementation that is supported by the code but not available
at runtime, an error will be displayed:
```
AVX512 or VPCLMULQDQ is not available.
```
I tested a GCC 13.2.0 `CMAKE_BUILD_TYPE=RelWithDebInfo` on a
200-megabyte file on one system to which I was granted access by Intel
Corporation. The reference implementation would typically consume
between 0.45 to 0.50 seconds when using one of the reflected
polynomials, and 0.53 to 0.56 seconds when using a non-reflected
polynomial. The AVX512BW and VPCLMULQDQ based SIMD implementation
would consume between 0.058 and 0.060 seconds on the reflected
polynomials, and between 0.063 and 0.065 seconds on the non-reflected
ones. This is about 8 times the speed of the reference
implementation. The difference could be a lot larger if the checksum
computation was not interleaved with synchronous system calls that
read the input file, only 16,384 bytes at a time.
