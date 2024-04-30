#ifdef __cplusplus
extern "C" {
#endif

int have_vpclmulqdq();
unsigned crc32_refl_vpclmulqdq(unsigned crc, const void *buf, size_t size);
unsigned crc32c_refl_vpclmulqdq(unsigned crc, const void *buf, size_t size);
unsigned crc32_vpclmulqdq(unsigned crc, const void *buf, size_t size);
unsigned crc32c_vpclmulqdq(unsigned crc, const void *buf, size_t size);

#ifdef __cplusplus
}
#endif
