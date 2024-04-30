#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include "crc32_avx512.h"

/** Lookup tables */
typedef uint32_t lookup[256];
static lookup lut_refl_3309, lut_refl_castagnoli, lut_3309, lut_castagnoli;

/** Initialize a lookup table for a CRC-32 polynomial */
template<bool reflect>
static void init_lut(lookup &lut, uint32_t polynomial)
{
  for (uint_fast32_t i = 0; i < 256; i++) {
    uint_fast32_t crc = reflect ? i : i << 24;
    for (uint_fast32_t bits = 8; bits--; )
      if (reflect)
        crc = (crc & 1) ? (crc >> 1) ^ polynomial : (crc >> 1);
      else
        crc = (crc & 1U << 31) ? (crc << 1) ^ polynomial : (crc << 1);
    lut[i] = crc;
  }
}

/** Compute a CRC-32 one octet at a time based on a lookup table */
template<bool reflect>
static uint32_t crc_lut(uint32_t crc, const void *buf, size_t len,
                        const lookup &lut)
{
  const unsigned char *b = static_cast<const unsigned char*>(buf);
  const unsigned char *const end = b + len;
  if (reflect) {
    crc ^= 0xffffffff;
    while (b != end)
      crc = lut[uint8_t(crc ^ *b++)] ^ (crc >> 8);
    crc ^= 0xffffffff;
  } else
    while (b != end)
      crc = lut[crc >> 24 ^ *b++] ^ (crc << 8);
  return crc;
}

static uint32_t crc32_refl(uint32_t crc, const void *buf, size_t len)
{ return crc_lut<true>(crc, buf, len, lut_refl_3309); }
static uint32_t crc32c_refl(uint32_t crc, const void *buf, size_t len)
{ return crc_lut<true>(crc, buf, len, lut_refl_castagnoli); }
static uint32_t crc32(uint32_t crc, const void *buf, size_t len)
{ return ~crc_lut<false>(~crc, buf, len, lut_3309); }
static uint32_t crc32c(uint32_t crc, const void *buf, size_t len)
{ return ~crc_lut<false>(~crc, buf, len, lut_castagnoli); }

typedef uint32_t (*crc_func)(uint32_t, const void*, size_t);

static size_t test_buf(const void *buf, size_t size, crc_func c1, crc_func c2)
{
  for (; size; size--)
    if (c1(0, buf, size) != c2(0, buf, size))
      break;
  return size;
}

struct sut
{
  const char *name;
  crc_func c1, c2;
};

static struct sut funcs[] = {
  { "CRC-32 (refl)", crc32_refl, crc32_refl_vpclmulqdq },
  { "CRC-32C (refl)", crc32c_refl, crc32c_refl_vpclmulqdq },
  { "CRC-32", crc32, crc32_vpclmulqdq },
  { "CRC-32C", crc32c, crc32c_vpclmulqdq }
};

int main(int, char**)
{
  static char buf[16384];
  init_lut<true>(lut_refl_3309, 0xedb88320/* 0x4c11db7 */);
  init_lut<true>(lut_refl_castagnoli, 0x82f63b78/* 0x1edc6f41 */);
  init_lut<false>(lut_3309, 0x4c11db7);
  init_lut<false>(lut_castagnoli, 0x1edc6f41);

  for (auto &t : buf) t = char(rand());

  int status = EXIT_SUCCESS;

  if (have_vpclmulqdq()) {
    fputs("Testing AVX512+VPCLMULQDQ: ", stderr);
    for (size_t i = sizeof funcs / sizeof(*funcs); i--; ) {
      fputs(funcs[i].name, stderr);
      if (size_t s = test_buf(buf, sizeof buf, funcs[i].c1, funcs[i].c2)) {
        fprintf(stderr, "(failed at %zu)", s);
        status = EXIT_FAILURE;
      }
      if (!i)
        break;
      fputs(", ", stderr);
    }
    fputs(".\n", stderr);
  }
  else
    fputs("AVX512 or VPCLMULQDQ is not available.\n", stderr);
  return status;
}
