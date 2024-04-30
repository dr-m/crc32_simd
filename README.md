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
