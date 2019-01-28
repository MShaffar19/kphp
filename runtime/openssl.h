#pragma once

#include "runtime/kphp_core.h"

enum openssl_algo : int {
  OPENSSL_ALGO_SHA1 = 1,
  OPENSSL_ALGO_MD5 = 2,
  OPENSSL_ALGO_MD4 = 3,
  OPENSSL_ALGO_MD2 = 4,
  OPENSSL_ALGO_DSS1 = 5,
  OPENSSL_ALGO_SHA224 = 6,
  OPENSSL_ALGO_SHA256 = 7,
  OPENSSL_ALGO_SHA384 = 8,
  OPENSSL_ALGO_SHA512 = 9,
  OPENSSL_ALGO_RMD160 = 10,
};

array<string> f$hash_algos();

string f$hash(const string &algo, const string &s, bool raw_output = false);

string f$hash_hmac(const string &algo, const string &data, const string &key, bool raw_output = false);

string f$sha1(const string &s, bool raw_output = false);

string f$md5(const string &s, bool raw_output = false);

OrFalse<string> f$md5_file(const string &file_name, bool raw_output = false);

int f$crc32(const string &s);

int f$crc32_file(const string &file_name);


bool f$openssl_public_encrypt(const string &data, string &result, const string &key);

bool f$openssl_public_encrypt(const string &data, var &result, const string &key);

bool f$openssl_private_decrypt(const string &data, string &result, const string &key);

bool f$openssl_private_decrypt(const string &data, var &result, const string &key);

OrFalse<string> f$openssl_pkey_get_private(const string &key, const string &passphrase = string());

OrFalse<string> f$openssl_pkey_get_public(const string &key);

int f$openssl_verify(const string &data, const string &signature, const string &pub_key_id, int algo = OPENSSL_ALGO_SHA1);

OrFalse<string> f$openssl_random_pseudo_bytes(int length);

OrFalse<array<var>> f$openssl_x509_parse(const string &data, bool shortnames = true);

var f$openssl_x509_checkpurpose(const string &data, int purpose);

array<string> f$openssl_get_cipher_methods(bool aliases = false);

OrFalse<int> f$openssl_cipher_iv_length(const string &method);

OrFalse<string> f$openssl_encrypt(const string &data, const string &method,
                                  const string &key, int options = 0, const string &iv = string());
OrFalse<string> f$openssl_decrypt(const string &data, const string &method,
                                  const string &key, int options = 0, const string &iv = string());

void openssl_init_static_once();

void openssl_init_static();

void openssl_free_static();
