// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
// LEGO Dimensions PC: as funcoes de crypto.cpp do shadPS4 0.7.0 usadas pelo extrator de PKG, com a criptografia do
// Windows (CNG / bcrypt) no lugar do Crypto++: RSA-2048 PKCS#1 v1.5 com chave privada, SHA-256, HMAC-SHA256, AES-128
// CBC e AES-128 XTS (o XTS por trecho inteiro numa chamada ECB, mesmo resultado do bloco a bloco).

#include <array>
#include <cstring>
#include <stdexcept>
#include <vector>

#include <windows.h>

#include <bcrypt.h>

#include "crypto.h"

namespace {

void Check(NTSTATUS status, const char* what) {
    if (!BCRYPT_SUCCESS(status)) {
        throw std::runtime_error(what);
    }
}

BCRYPT_ALG_HANDLE Provider(const wchar_t* alg, ULONG flags = 0, const wchar_t* mode = nullptr) {
    BCRYPT_ALG_HANDLE handle = nullptr;
    Check(BCryptOpenAlgorithmProvider(&handle, alg, nullptr, flags), "BCryptOpenAlgorithmProvider");
    if (mode) {
        Check(BCryptSetProperty(handle, BCRYPT_CHAINING_MODE, reinterpret_cast<PUCHAR>(const_cast<wchar_t*>(mode)),
                                static_cast<ULONG>((wcslen(mode) + 1) * sizeof(wchar_t)), 0),
              "BCryptSetProperty");
    }
    return handle;
}

BCRYPT_ALG_HANDLE AesCbc() {
    static BCRYPT_ALG_HANDLE h = Provider(BCRYPT_AES_ALGORITHM, 0, BCRYPT_CHAIN_MODE_CBC);
    return h;
}

BCRYPT_ALG_HANDLE AesEcb() {
    static BCRYPT_ALG_HANDLE h = Provider(BCRYPT_AES_ALGORITHM, 0, BCRYPT_CHAIN_MODE_ECB);
    return h;
}

struct Key {
    BCRYPT_KEY_HANDLE handle = nullptr;
    Key(BCRYPT_ALG_HANDLE alg, const u8* secret, ULONG size) {
        Check(BCryptGenerateSymmetricKey(alg, &handle, nullptr, 0, const_cast<u8*>(secret), size, 0), "BCryptGenerateSymmetricKey");
    }
    ~Key() {
        BCryptDestroyKey(handle);
    }
};

// AES-128 CBC sem padding, so os blocos inteiros (o Crypto++ fazia ProcessData de 16 em 16 bytes, encadeado)
void AesCbcDecrypt(const u8* key, const u8* iv_in, const u8* src, u8* dst, size_t size) {
    size &= ~size_t(15);
    if (!size) {
        return;
    }
    Key k(AesCbc(), key, 16);
    u8 iv[16];
    std::memcpy(iv, iv_in, 16);
    ULONG done = 0;
    Check(BCryptDecrypt(k.handle, const_cast<u8*>(src), static_cast<ULONG>(size), nullptr, iv, 16, dst, static_cast<ULONG>(size), &done, 0),
          "BCryptDecrypt CBC");
}

// Chave RSA privada completa (BCRYPT_RSAFULLPRIVATE_BLOB), numeros em big-endian como em keys.h
template <typename KeySet>
std::vector<u8> RsaBlob() {
    auto trimmed = [](const u8* p, size_t n) {
        while (n > 1 && *p == 0) {
            ++p, --n;
        }
        return std::vector<u8>(p, p + n);
    };
    const std::vector<u8> e = trimmed(KeySet::PublicExponent, sizeof(KeySet::PublicExponent));
    BCRYPT_RSAKEY_BLOB header{};
    header.Magic = BCRYPT_RSAFULLPRIVATE_MAGIC;
    header.BitLength = 2048;
    header.cbPublicExp = static_cast<ULONG>(e.size());
    header.cbModulus = sizeof(KeySet::Modulus);
    header.cbPrime1 = sizeof(KeySet::Prime1);
    header.cbPrime2 = sizeof(KeySet::Prime2);
    std::vector<u8> blob(reinterpret_cast<const u8*>(&header), reinterpret_cast<const u8*>(&header) + sizeof(header));
    auto add = [&](const u8* p, size_t n) { blob.insert(blob.end(), p, p + n); };
    add(e.data(), e.size());
    add(KeySet::Modulus, sizeof(KeySet::Modulus));
    add(KeySet::Prime1, sizeof(KeySet::Prime1));
    add(KeySet::Prime2, sizeof(KeySet::Prime2));
    add(KeySet::Exponent1, sizeof(KeySet::Exponent1));
    add(KeySet::Exponent2, sizeof(KeySet::Exponent2));
    add(KeySet::Coefficient, sizeof(KeySet::Coefficient));
    add(KeySet::PrivateExponent, sizeof(KeySet::PrivateExponent));
    return blob;
}

} // namespace

void Crypto::RSA2048Decrypt(std::span<u8, 32> dec_key, std::span<const u8, 256> ciphertext,
                            bool is_dk3) { // RSAES_PKCS1v15_
    static BCRYPT_ALG_HANDLE rsa = Provider(BCRYPT_RSA_ALGORITHM);
    std::vector<u8> blob = is_dk3 ? RsaBlob<PkgDerivedKey3Keyset>() : RsaBlob<FakeKeyset>();
    BCRYPT_KEY_HANDLE key = nullptr;
    Check(BCryptImportKeyPair(rsa, nullptr, BCRYPT_RSAFULLPRIVATE_BLOB, &key, blob.data(), static_cast<ULONG>(blob.size()), 0),
          "BCryptImportKeyPair");
    std::array<u8, 256> decrypted{};
    ULONG done = 0;
    const NTSTATUS status = BCryptDecrypt(key, const_cast<u8*>(ciphertext.data()), 256, nullptr, nullptr, 0, decrypted.data(),
                                          static_cast<ULONG>(decrypted.size()), &done, BCRYPT_PAD_PKCS1);
    BCryptDestroyKey(key);
    Check(status, "BCryptDecrypt RSA");
    std::copy(decrypted.begin(), decrypted.begin() + dec_key.size(), dec_key.begin());
}

void Crypto::ivKeyHASH256(std::span<const u8, 64> cipher_input, std::span<u8, 32> ivkey_result) {
    static BCRYPT_ALG_HANDLE sha = Provider(BCRYPT_SHA256_ALGORITHM);
    std::array<u8, 32> hash{};
    Check(BCryptHash(sha, nullptr, 0, const_cast<u8*>(cipher_input.data()), static_cast<ULONG>(cipher_input.size()), hash.data(), 32),
          "BCryptHash");
    std::copy(hash.begin(), hash.begin() + ivkey_result.size(), ivkey_result.begin());
}

void Crypto::aesCbcCfb128Decrypt(std::span<const u8, 32> ivkey, std::span<const u8, 256> ciphertext,
                                 std::span<u8, 256> decrypted) {
    AesCbcDecrypt(ivkey.data() + 16, ivkey.data(), ciphertext.data(), decrypted.data(), decrypted.size());
}

void Crypto::aesCbcCfb128DecryptEntry(std::span<const u8, 32> ivkey, std::span<u8> ciphertext,
                                      std::span<u8> decrypted) {
    AesCbcDecrypt(ivkey.data() + 16, ivkey.data(), ciphertext.data(), decrypted.data(), decrypted.size());
}

void Crypto::PfsGenCryptoKey(std::span<const u8, 32> ekpfs, std::span<const u8, 16> seed,
                             std::span<u8, 16> dataKey, std::span<u8, 16> tweakKey) {
    static BCRYPT_ALG_HANDLE hmac = Provider(BCRYPT_SHA256_ALGORITHM, BCRYPT_ALG_HANDLE_HMAC_FLAG);
    std::array<u8, 20> d{};
    const u32 index = 1;
    std::memcpy(d.data(), &index, sizeof(index));
    std::memcpy(d.data() + sizeof(index), seed.data(), seed.size());
    std::array<u8, 32> data_tweak_key{};
    Check(BCryptHash(hmac, const_cast<u8*>(ekpfs.data()), static_cast<ULONG>(ekpfs.size()), d.data(), static_cast<ULONG>(d.size()),
                     data_tweak_key.data(), 32),
          "BCryptHash HMAC");
    std::copy(data_tweak_key.begin(), data_tweak_key.begin() + dataKey.size(), tweakKey.begin());
    std::copy(data_tweak_key.begin() + tweakKey.size(), data_tweak_key.begin() + tweakKey.size() + dataKey.size(), dataKey.begin());
}

void Crypto::decryptPFS(std::span<const u8, 16> dataKey, std::span<const u8, 16> tweakKey,
                        std::span<const u8> src_image, std::span<u8> dst_image, u64 sector) {
    // XTS com setores de 0x1000: tweak de cada setor = AES(tweakKey, numero do setor), multiplicado a cada bloco.
    const size_t sectors = src_image.size() / 0x1000;
    if (!sectors) {
        return;
    }
    Key tweak_key(AesEcb(), tweakKey.data(), 16);
    Key data_key(AesEcb(), dataKey.data(), 16);
    std::vector<u8> tweaks(sectors * 16, 0);
    for (size_t s = 0; s < sectors; s++) {
        const u64 current_sector = sector + s;
        std::memcpy(tweaks.data() + s * 16, &current_sector, sizeof(u64));
    }
    ULONG done = 0;
    Check(BCryptEncrypt(tweak_key.handle, tweaks.data(), static_cast<ULONG>(tweaks.size()), nullptr, nullptr, 0, tweaks.data(),
                        static_cast<ULONG>(tweaks.size()), &done, 0),
          "BCryptEncrypt XTS tweak");
    const size_t size = sectors * 0x1000;
    std::vector<u8> per_block(size); // o tweak de cada bloco de 16 bytes
    for (size_t s = 0; s < sectors; s++) {
        std::array<u8, 16> t;
        std::memcpy(t.data(), tweaks.data() + s * 16, 16);
        for (size_t off = 0; off < 0x1000; off += 16) {
            std::memcpy(per_block.data() + s * 0x1000 + off, t.data(), 16);
            xtsMult(t);
        }
    }
    std::vector<u8> work(size);
    for (size_t i = 0; i < size; i++) {
        work[i] = src_image[i] ^ per_block[i];
    }
    Check(BCryptDecrypt(data_key.handle, work.data(), static_cast<ULONG>(size), nullptr, nullptr, 0, work.data(), static_cast<ULONG>(size),
                        &done, 0),
          "BCryptDecrypt XTS");
    for (size_t i = 0; i < size; i++) {
        dst_image[i] = work[i] ^ per_block[i];
    }
}
