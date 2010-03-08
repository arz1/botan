/*
* Rabin-Williams
* (C) 1999-2007 Jack Lloyd
*
* Distributed under the terms of the Botan license
*/

#ifndef BOTAN_RW_H__
#define BOTAN_RW_H__

#include <botan/if_algo.h>
#include <botan/reducer.h>

namespace Botan {

/*
* Rabin-Williams Public Key
*/
class BOTAN_DLL RW_PublicKey : public virtual IF_Scheme_PublicKey
   {
   public:
      std::string algo_name() const { return "RW"; }

      RW_PublicKey(const AlgorithmIdentifier& alg_id,
                   const MemoryRegion<byte>& key_bits) :
         IF_Scheme_PublicKey(alg_id, key_bits)
         {}

      RW_PublicKey(const BigInt& mod, const BigInt& exponent) :
         IF_Scheme_PublicKey(mod, exponent)
         {}

   protected:
      RW_PublicKey() {}
   };

/*
* Rabin-Williams Private Key
*/
class BOTAN_DLL RW_PrivateKey : public RW_PublicKey,
                                public IF_Scheme_PrivateKey
   {
   public:
      RW_PrivateKey(const AlgorithmIdentifier& alg_id,
                    const MemoryRegion<byte>& key_bits,
                    RandomNumberGenerator& rng) :
         IF_Scheme_PrivateKey(rng, alg_id, key_bits) {}

      RW_PrivateKey(RandomNumberGenerator& rng,
                    const BigInt& p, const BigInt& q,
                    const BigInt& e, const BigInt& d = 0,
                    const BigInt& n = 0) :
         IF_Scheme_PrivateKey(rng, p, q, e, d, n) {}

      RW_PrivateKey(RandomNumberGenerator& rng, u32bit bits, u32bit = 2);

      bool check_key(RandomNumberGenerator& rng, bool) const;
   };

class BOTAN_DLL RW_Signature_Operation : public PK_Ops::Signature
   {
   public:
      RW_Signature_Operation(const RW_PrivateKey& rw);

      u32bit max_input_bits() const { return (n.bits() - 1); }

      SecureVector<byte> sign(const byte msg[], u32bit msg_len,
                              RandomNumberGenerator& rng) const;
   private:
      const BigInt& q;
      const BigInt& c;
      const BigInt& n;

      Fixed_Exponent_Power_Mod powermod_d1_p, powermod_d2_q;
      Modular_Reducer mod_p;
   };

class BOTAN_DLL RW_Verification_Operation : public PK_Ops::Verification
   {
   public:
      RW_Verification_Operation(const RW_PublicKey& rw) :
         n(rw.get_n()), powermod_e_n(rw.get_e(), rw.get_n())
         {}

      u32bit max_input_bits() const { return (n.bits() - 1); }
      bool with_recovery() const { return true; }

      SecureVector<byte> verify_mr(const byte msg[], u32bit msg_len) const;

   private:
      const BigInt& n;
      Fixed_Exponent_Power_Mod powermod_e_n;
   };

}

#endif
