#ifndef __OPRP_DES_CRYPT_H__
#define __OPRP_DES_CRYPT_H__

#define _UFC_64_

#ifndef STATIC
#define STATIC static
#endif

#include <libc-lock.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define _ufc_clearmem(start, cnt) memset(start, 0, cnt)
#define ascii_to_bin(c)                                                        \
  ((c) >= 'a' ? (c - 59) : (c) >= 'A' ? ((c)-53) : (c) - '.')
#define bin_to_ascii(c)                                                        \
  ((c) >= 38 ? ((c)-38 + 'a') : (c) >= 12 ? ((c)-12 + 'A') : (c) + '.')

typedef uint_fast32_t ufc_long;
typedef uint64_t long64;
typedef uint32_t long32;

struct crypt_data {
  char keysched[16 * 8];
  char sb0[32768];
  char sb1[32768];
  char sb2[32768];
  char sb3[32768];
  /* end-of-aligment-critical-data */
  char crypt_3_buf[14];
  char current_salt[2];
  long int current_saltbits;
  int direction, initialized;
};

static ufc_long do_pc1[8][2][128];
static ufc_long do_pc2[8][128];
static ufc_long eperm32tab[4][256][2];
static ufc_long efp[16][64][2];
static const int rots[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

static const char b64t[65] =
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static const ufc_long BITMASK[24] = {
    0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000, 0x02000000,
    0x01000000, 0x00800000, 0x00400000, 0x00200000, 0x00100000, 0x00080000,
    0x00004000, 0x00002000, 0x00001000, 0x00000800, 0x00000400, 0x00000200,
    0x00000100, 0x00000080, 0x00000040, 0x00000020, 0x00000010, 0x00000008};
static const unsigned char bytemask[8] = {0x80, 0x40, 0x20, 0x10,
                                          0x08, 0x04, 0x02, 0x01};
static const ufc_long longmask[32] = {
    0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000,
    0x02000000, 0x01000000, 0x00800000, 0x00400000, 0x00200000, 0x00100000,
    0x00080000, 0x00040000, 0x00020000, 0x00010000, 0x00008000, 0x00004000,
    0x00002000, 0x00001000, 0x00000800, 0x00000400, 0x00000200, 0x00000100,
    0x00000080, 0x00000040, 0x00000020, 0x00000010, 0x00000008, 0x00000004,
    0x00000002, 0x00000001};

char *crypt_r(const char *key, const char *salt,
              struct crypt_data *__restrict data) {
  ufc_long res[4];
  char ktab[9];
  ufc_long xx = 25; /* to cope with GCC long long compiler bugs */

  /*
   * Hack DES tables according to salt
   */
  if (!_ufc_setup_salt_r(salt, data)) {
    printf("cagou no _ufc_setup_salt_r(salt, data)\n");
    return NULL;
  }

  /*
   * Setup key schedule
   */
  _ufc_clearmem(ktab, (int)sizeof(ktab));
  (void)strncpy(ktab, key, 8);

  _ufc_mk_keytab_r(ktab, data);
  /*
   * Go for the 25 DES encryptions
   */
  _ufc_clearmem((char *)res, (int)sizeof(res));
  _ufc_doit_r(xx, data, &res[0]);
  /*
   * Do final permutations
   */
  _ufc_dofinalperm_r(res, data);
  /*
   * And convert back to 6 bit ASCII
   */
  _ufc_output_conversion_r(res[0], res[1], salt, data);
  /*
   * Erase key-dependent intermediate data.  Data dependent only on
   * the salt is not considered sensitive.
   */
  explicit_bzero(ktab, sizeof(ktab));
  explicit_bzero(data->keysched, sizeof(data->keysched));
  explicit_bzero(res, sizeof(res));
  return data->crypt_3_buf;
}

#define SBA(sb, v) (*(long64 *)((char *)(sb) + (v)))

void _ufc_doit_r(ufc_long itr, struct crypt_data *__restrict __data,
                 ufc_long *res) {
  int i;
  long64 l, r, s, *k;
  long64 *sb01 = (long64 *)__data->sb0;
  long64 *sb23 = (long64 *)__data->sb2;

  l = (((long64)res[0]) << 32) | ((long64)res[1]);
  r = (((long64)res[2]) << 32) | ((long64)res[3]);

  while (itr--) {
    k = (long64 *)__data->keysched;
    for (i = 8; i--;) {
      s = *k++ ^ r;
      l ^= SBA(sb23, (s)&0xffff);
      l ^= SBA(sb23, (s >>= 16) & 0xffff);
      l ^= SBA(sb01, (s >>= 16) & 0xffff);
      l ^= SBA(sb01, (s >>= 16));

      s = *k++ ^ l;
      r ^= SBA(sb23, (s)&0xffff);
      r ^= SBA(sb23, (s >>= 16) & 0xffff);
      r ^= SBA(sb01, (s >>= 16) & 0xffff);
      r ^= SBA(sb01, (s >>= 16));
    }
    s = l;
    l = r;
    r = s;
  }

  res[0] = l >> 32;
  res[1] = l & 0xffffffff;
  res[2] = r >> 32;
  res[3] = r & 0xffffffff;
}

/*
 * Setup the unit for a new salt
 * Hopefully we'll not see a new salt in each crypt call.
 * Return false if an unexpected character was found in s[0] or s[1].
 */
bool _ufc_setup_salt_r(const char *s, struct crypt_data *__restrict __data) {
  ufc_long i, j, saltbits;
  char s0, s1;
  if (__data->initialized == 0)
    __init_des_r(__data);
  s0 = s[0];
  if (bad_for_salt(s0))
    return false;
  s1 = s[1];
  if (bad_for_salt(s1))
    return false;
  if (s0 == __data->current_salt[0] && s1 == __data->current_salt[1])
    return true;
  __data->current_salt[0] = s0;
  __data->current_salt[1] = s1;
  /*
   * This is the only crypt change to DES:
   * entries are swapped in the expansion table
   * according to the bits set in the salt.
   */
  saltbits = 0;
  for (i = 0; i < 2; i++) {
    long c = ascii_to_bin(s[i]);
    for (j = 0; j < 6; j++) {
      if ((c >> j) & 0x1)
        saltbits |= BITMASK[6 * i + j];
    }
  }
  /*
   * Permute the sb table values
   * to reflect the changed e
   * selection table
   */
#ifdef _UFC_32_
#define LONGG long32 *
#endif
#ifdef _UFC_64_
#define LONGG long64 *
#endif
  shuffle_sb((LONGG)__data->sb0, __data->current_saltbits ^ saltbits);
  shuffle_sb((LONGG)__data->sb1, __data->current_saltbits ^ saltbits);
  shuffle_sb((LONGG)__data->sb2, __data->current_saltbits ^ saltbits);
  shuffle_sb((LONGG)__data->sb3, __data->current_saltbits ^ saltbits);
  __data->current_saltbits = saltbits;
  return true;
}

void _ufc_mk_keytab_r(const char *key, struct crypt_data *__restrict __data) {
  ufc_long v1, v2, *k1;
  int i;
#ifdef _UFC_32_
  long32 v, *k2;
  k2 = (long32 *)__data->keysched;
#endif
#ifdef _UFC_64_
  long64 v, *k2;
  k2 = (long64 *)__data->keysched;
#endif
  v1 = v2 = 0;
  k1 = &do_pc1[0][0][0];
  for (i = 8; i--;) {
    v1 |= k1[*key & 0x7f];
    k1 += 128;
    v2 |= k1[*key++ & 0x7f];
    k1 += 128;
  }
  for (i = 0; i < 16; i++) {
    k1 = &do_pc2[0][0];
    v1 = (v1 << rots[i]) | (v1 >> (28 - rots[i]));
    v = k1[(v1 >> 21) & 0x7f];
    k1 += 128;
    v |= k1[(v1 >> 14) & 0x7f];
    k1 += 128;
    v |= k1[(v1 >> 7) & 0x7f];
    k1 += 128;
    v |= k1[(v1)&0x7f];
    k1 += 128;
#ifdef _UFC_32_
    *k2++ = (v | 0x00008000);
    v = 0;
#endif
#ifdef _UFC_64_
    v = (v << 32);
#endif
    v2 = (v2 << rots[i]) | (v2 >> (28 - rots[i]));
    v |= k1[(v2 >> 21) & 0x7f];
    k1 += 128;
    v |= k1[(v2 >> 14) & 0x7f];
    k1 += 128;
    v |= k1[(v2 >> 7) & 0x7f];
    k1 += 128;
    v |= k1[(v2)&0x7f];
#ifdef _UFC_32_
    *k2++ = (v | 0x00008000);
#endif
#ifdef _UFC_64_
    *k2++ = v | 0x0000800000008000l;
#endif
  }
  __data->direction = 0;
}

void _ufc_dofinalperm_r(ufc_long *res, struct crypt_data *__restrict __data) {
  ufc_long v1, v2, x;
  ufc_long l1, l2, r1, r2;
  l1 = res[0];
  l2 = res[1];
  r1 = res[2];
  r2 = res[3];
  x = (l1 ^ l2) & __data->current_saltbits;
  l1 ^= x;
  l2 ^= x;
  x = (r1 ^ r2) & __data->current_saltbits;
  r1 ^= x;
  r2 ^= x;
  v1 = v2 = 0;
  l1 >>= 3;
  l2 >>= 3;
  r1 >>= 3;
  r2 >>= 3;
  v1 |= efp[15][r2 & 0x3f][0];
  v2 |= efp[15][r2 & 0x3f][1];
  v1 |= efp[14][(r2 >>= 6) & 0x3f][0];
  v2 |= efp[14][r2 & 0x3f][1];
  v1 |= efp[13][(r2 >>= 10) & 0x3f][0];
  v2 |= efp[13][r2 & 0x3f][1];
  v1 |= efp[12][(r2 >>= 6) & 0x3f][0];
  v2 |= efp[12][r2 & 0x3f][1];
  v1 |= efp[11][r1 & 0x3f][0];
  v2 |= efp[11][r1 & 0x3f][1];
  v1 |= efp[10][(r1 >>= 6) & 0x3f][0];
  v2 |= efp[10][r1 & 0x3f][1];
  v1 |= efp[9][(r1 >>= 10) & 0x3f][0];
  v2 |= efp[9][r1 & 0x3f][1];
  v1 |= efp[8][(r1 >>= 6) & 0x3f][0];
  v2 |= efp[8][r1 & 0x3f][1];
  v1 |= efp[7][l2 & 0x3f][0];
  v2 |= efp[7][l2 & 0x3f][1];
  v1 |= efp[6][(l2 >>= 6) & 0x3f][0];
  v2 |= efp[6][l2 & 0x3f][1];
  v1 |= efp[5][(l2 >>= 10) & 0x3f][0];
  v2 |= efp[5][l2 & 0x3f][1];
  v1 |= efp[4][(l2 >>= 6) & 0x3f][0];
  v2 |= efp[4][l2 & 0x3f][1];
  v1 |= efp[3][l1 & 0x3f][0];
  v2 |= efp[3][l1 & 0x3f][1];
  v1 |= efp[2][(l1 >>= 6) & 0x3f][0];
  v2 |= efp[2][l1 & 0x3f][1];
  v1 |= efp[1][(l1 >>= 10) & 0x3f][0];
  v2 |= efp[1][l1 & 0x3f][1];
  v1 |= efp[0][(l1 >>= 6) & 0x3f][0];
  v2 |= efp[0][l1 & 0x3f][1];
  res[0] = v1;
  res[1] = v2;
}

void _ufc_output_conversion_r(ufc_long v1, ufc_long v2, const char *salt,
                              struct crypt_data *__restrict __data) {
  int i, s, shf;
  __data->crypt_3_buf[0] = salt[0];
  __data->crypt_3_buf[1] = salt[1] ? salt[1] : salt[0];
  for (i = 0; i < 5; i++) {
    shf = (26 - 6 * i); /* to cope with MSC compiler bug */
    __data->crypt_3_buf[i + 2] = bin_to_ascii((v1 >> shf) & 0x3f);
  }
  s = (v2 & 0xf) << 2;
  v2 = (v2 >> 2) | ((v1 & 0x3) << 30);
  for (i = 5; i < 10; i++) {
    shf = (56 - 6 * i);
    __data->crypt_3_buf[i + 2] = bin_to_ascii((v2 >> shf) & 0x3f);
  }
  __data->crypt_3_buf[12] = bin_to_ascii(s);
  __data->crypt_3_buf[13] = 0;
}

void __init_des_r(struct crypt_data *__restrict __data) {
  int comes_from_bit;
  int bit, sg;
  ufc_long j;
  ufc_long mask1, mask2;
  int e_inverse[64];
  static volatile int small_tables_initialized = 0;
#ifdef _UFC_32_
  long32 *sb[4];
  sb[0] = (long32 *)__data->sb0;
  sb[1] = (long32 *)__data->sb1;
  sb[2] = (long32 *)__data->sb2;
  sb[3] = (long32 *)__data->sb3;
#endif
#ifdef _UFC_64_
  long64 *sb[4];
  sb[0] = (long64 *)__data->sb0;
  sb[1] = (long64 *)__data->sb1;
  sb[2] = (long64 *)__data->sb2;
  sb[3] = (long64 *)__data->sb3;
#endif
  if (small_tables_initialized == 0) {
#ifdef __GNU_LIBRARY__
    __libc_lock_lock(_ufc_tables_lock);
    if (small_tables_initialized)
      goto small_tables_done;
#endif
    /*
     * Create the do_pc1 table used
     * to affect pc1 permutation
     * when generating keys
     */
    _ufc_clearmem((char *)do_pc1, (int)sizeof(do_pc1));
    for (bit = 0; bit < 56; bit++) {
      comes_from_bit = pc1[bit] - 1;
      mask1 = bytemask[comes_from_bit % 8 + 1];
      mask2 = longmask[bit % 28 + 4];
      for (j = 0; j < 128; j++) {
        if (j & mask1)
          do_pc1[comes_from_bit / 8][bit / 28][j] |= mask2;
      }
    }
    /*
     * Create the do_pc2 table used
     * to affect pc2 permutation when
     * generating keys
     */
    _ufc_clearmem((char *)do_pc2, (int)sizeof(do_pc2));
    for (bit = 0; bit < 48; bit++) {
      comes_from_bit = pc2[bit] - 1;
      mask1 = bytemask[comes_from_bit % 7 + 1];
      mask2 = BITMASK[bit % 24];
      for (j = 0; j < 128; j++) {
        if (j & mask1)
          do_pc2[comes_from_bit / 7][j] |= mask2;
      }
    }
    /*
     * Now generate the table used to do combined
     * 32 bit permutation and e expansion
     *
     * We use it because we have to permute 16384 32 bit
     * longs into 48 bit in order to initialize sb.
     *
     * Looping 48 rounds per permutation becomes
     * just too slow...
     *
     */
    _ufc_clearmem((char *)eperm32tab, (int)sizeof(eperm32tab));
    for (bit = 0; bit < 48; bit++) {
      ufc_long mask1, comes_from;
      comes_from = perm32[esel[bit] - 1] - 1;
      mask1 = bytemask[comes_from % 8];
      for (j = 256; j--;) {
        if (j & mask1)
          eperm32tab[comes_from / 8][j][bit / 24] |= BITMASK[bit % 24];
      }
    }
    /*
     * Create an inverse matrix for esel telling
     * where to plug out bits if undoing it
     */
    for (bit = 48; bit--;) {
      e_inverse[esel[bit] - 1] = bit;
      e_inverse[esel[bit] - 1 + 32] = bit + 48;
    }
    /*
     * create efp: the matrix used to
     * undo the E expansion and effect final permutation
     */
    _ufc_clearmem((char *)efp, (int)sizeof efp);
    for (bit = 0; bit < 64; bit++) {
      int o_bit, o_long;
      ufc_long word_value, mask1, mask2;
      int comes_from_f_bit, comes_from_e_bit;
      int comes_from_word, bit_within_word;
      /* See where bit i belongs in the two 32 bit long's */
      o_long = bit / 32; /* 0..1  */
      o_bit = bit % 32;  /* 0..31 */
      /*
       * And find a bit in the e permutated value setting this bit.
       *
       * Note: the e selection may have selected the same bit several
       * times. By the initialization of e_inverse, we only look
       * for one specific instance.
       */
      comes_from_f_bit = final_perm[bit] - 1;         /* 0..63 */
      comes_from_e_bit = e_inverse[comes_from_f_bit]; /* 0..95 */
      comes_from_word = comes_from_e_bit / 6;         /* 0..15 */
      bit_within_word = comes_from_e_bit % 6;         /* 0..5  */
      mask1 = longmask[bit_within_word + 26];
      mask2 = longmask[o_bit];
      for (word_value = 64; word_value--;) {
        if (word_value & mask1)
          efp[comes_from_word][word_value][o_long] |= mask2;
      }
    }
    atomic_write_barrier();
    small_tables_initialized = 1;
#ifdef __GNU_LIBRARY__
  small_tables_done:
    __libc_lock_unlock(_ufc_tables_lock);
#endif
  } else
    atomic_read_barrier();
  /*
   * Create the sb tables:
   *
   * For each 12 bit segment of an 48 bit intermediate
   * result, the sb table precomputes the two 4 bit
   * values of the sbox lookups done with the two 6
   * bit halves, shifts them to their proper place,
   * sends them through perm32 and finally E expands
   * them so that they are ready for the next
   * DES round.
   *
   */
  if (__data->sb0 + sizeof(__data->sb0) == __data->sb1 &&
      __data->sb1 + sizeof(__data->sb1) == __data->sb2 &&
      __data->sb2 + sizeof(__data->sb2) == __data->sb3)
    _ufc_clearmem(__data->sb0,
                  (int)sizeof(__data->sb0) + (int)sizeof(__data->sb1) +
                      (int)sizeof(__data->sb2) + (int)sizeof(__data->sb3));
  else {
    _ufc_clearmem(__data->sb0, (int)sizeof(__data->sb0));
    _ufc_clearmem(__data->sb1, (int)sizeof(__data->sb1));
    _ufc_clearmem(__data->sb2, (int)sizeof(__data->sb2));
    _ufc_clearmem(__data->sb3, (int)sizeof(__data->sb3));
  }
  for (sg = 0; sg < 4; sg++) {
    int j1, j2;
    int s1, s2;
    for (j1 = 0; j1 < 64; j1++) {
      s1 = s_lookup(2 * sg, j1);
      for (j2 = 0; j2 < 64; j2++) {
        ufc_long to_permute, inx;
        s2 = s_lookup(2 * sg + 1, j2);
        to_permute = (((ufc_long)s1 << 4) | (ufc_long)s2)
                     << (24 - 8 * (ufc_long)sg);
#ifdef _UFC_32_
        inx = ((j1 << 6) | j2) << 1;
        sb[sg][inx] = eperm32tab[0][(to_permute >> 24) & 0xff][0];
        sb[sg][inx + 1] = eperm32tab[0][(to_permute >> 24) & 0xff][1];
        sb[sg][inx] |= eperm32tab[1][(to_permute >> 16) & 0xff][0];
        sb[sg][inx + 1] |= eperm32tab[1][(to_permute >> 16) & 0xff][1];
        sb[sg][inx] |= eperm32tab[2][(to_permute >> 8) & 0xff][0];
        sb[sg][inx + 1] |= eperm32tab[2][(to_permute >> 8) & 0xff][1];
        sb[sg][inx] |= eperm32tab[3][(to_permute)&0xff][0];
        sb[sg][inx + 1] |= eperm32tab[3][(to_permute)&0xff][1];
#endif
#ifdef _UFC_64_
        inx = ((j1 << 6) | j2);
        sb[sg][inx] =
            ((long64)eperm32tab[0][(to_permute >> 24) & 0xff][0] << 32) |
            (long64)eperm32tab[0][(to_permute >> 24) & 0xff][1];
        sb[sg][inx] |=
            ((long64)eperm32tab[1][(to_permute >> 16) & 0xff][0] << 32) |
            (long64)eperm32tab[1][(to_permute >> 16) & 0xff][1];
        sb[sg][inx] |=
            ((long64)eperm32tab[2][(to_permute >> 8) & 0xff][0] << 32) |
            (long64)eperm32tab[2][(to_permute >> 8) & 0xff][1];
        sb[sg][inx] |= ((long64)eperm32tab[3][(to_permute)&0xff][0] << 32) |
                       (long64)eperm32tab[3][(to_permute)&0xff][1];
#endif
      }
    }
  }
  __data->current_saltbits = 0;
  __data->current_salt[0] = 0;
  __data->current_salt[1] = 0;
  __data->initialized++;
}

static bool bad_for_salt(char c) {
  switch (c) {
  case '0' ... '9':
  case 'A' ... 'Z':
  case 'a' ... 'z':
  case '.':
  case '/':
    return false;
  default:
    return true;
  }
}

#ifdef _UFC_64_
STATIC void shuffle_sb(long64 *k, ufc_long saltbits) {
  ufc_long j;
  long64 x;
  for (j = 4096; j--;) {
    x = ((*k >> 32) ^ *k) & (long64)saltbits;
    *k++ ^= (x << 32) | x;
  }
}
#endif

#endif