// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

// Note: In 0.8 and before, crypto functions all defaulted to using
// binary-encoded strings rather than buffers.

'use strict';

const {
  assertCrypto,
  deprecate
} = require('internal/util');
assertCrypto();

const { internalBinding } = require('internal/bootstrap/loaders');
const {
  ERR_CRYPTO_FIPS_FORCED,
  ERR_CRYPTO_FIPS_UNAVAILABLE
} = require('internal/errors').codes;
const constants = process.binding('constants').crypto;
const {
  fipsMode,
  fipsForced
} = process.binding('config');
const { getFipsCrypto, setFipsCrypto } = internalBinding('crypto');
const {
  randomBytes,
  randomFill,
  randomFillSync
} = require('internal/crypto/random');
const {
  pbkdf2,
  pbkdf2Sync
} = require('internal/crypto/pbkdf2');
const {
  scrypt,
  scryptSync
} = require('internal/crypto/scrypt');
const {
  generateKeyPair,
  generateKeyPairSync
} = require('internal/crypto/keygen');
const {
  DiffieHellman,
  DiffieHellmanGroup,
  ECDH
} = require('internal/crypto/diffiehellman');
const {
  Cipher,
  Cipheriv,
  Decipher,
  Decipheriv,
  privateDecrypt,
  privateEncrypt,
  publicDecrypt,
  publicEncrypt
} = require('internal/crypto/cipher');
const {
  Sign,
  Verify
} = require('internal/crypto/sig');
const {
  Hash,
  Hmac
} = require('internal/crypto/hash');
const {
  getCiphers,
  getCurves,
  getDefaultEncoding,
  getHashes,
  setDefaultEncoding,
  setEngine,
  timingSafeEqual,
  toBuf
} = require('internal/crypto/util');
const Certificate = require('internal/crypto/certificate');

// These helper functions are needed because the constructors can
// use new, in which case V8 cannot inline the recursive constructor call
function createHash(algorithm, options) {
  return new Hash(algorithm, options);
}

function createCipher(cipher, password, options) {
  return new Cipher(cipher, password, options);
}

function createCipheriv(cipher, key, iv, options) {
  return new Cipheriv(cipher, key, iv, options);
}

function createDecipher(cipher, password, options) {
  return new Decipher(cipher, password, options);
}

function createDecipheriv(cipher, key, iv, options) {
  return new Decipheriv(cipher, key, iv, options);
}

function createDiffieHellman(sizeOrKey, keyEncoding, generator, genEncoding) {
  return new DiffieHellman(sizeOrKey, keyEncoding, generator, genEncoding);
}

function createDiffieHellmanGroup(name) {
  return new DiffieHellmanGroup(name);
}

function createECDH(curve) {
  return new ECDH(curve);
}

function createHmac(hmac, key, options) {
  return new Hmac(hmac, key, options);
}

function createSign(algorithm, options) {
  return new Sign(algorithm, options);
}

function createVerify(algorithm, options) {
  return new Verify(algorithm, options);
}

module.exports = exports = {
  // Methods
  createCipheriv,
  createDecipheriv,
  createDiffieHellman,
  createDiffieHellmanGroup,
  createECDH,
  createHash,
  createHmac,
  createSign,
  createVerify,
  getCiphers,
  getCurves,
  getDiffieHellman: createDiffieHellmanGroup,
  getHashes,
  pbkdf2,
  pbkdf2Sync,
  generateKeyPair,
  generateKeyPairSync,
  privateDecrypt,
  privateEncrypt,
  publicDecrypt,
  publicEncrypt,
  randomBytes,
  randomFill,
  randomFillSync,
  scrypt,
  scryptSync,
  setEngine,
  timingSafeEqual,
  getFips: !fipsMode ? getFipsDisabled :
    fipsForced ? getFipsForced : getFipsCrypto,
  setFips: !fipsMode ? setFipsDisabled :
    fipsForced ? setFipsForced : setFipsCrypto,

  // Classes
  Certificate,
  Cipher,
  Cipheriv,
  Decipher,
  Decipheriv,
  DiffieHellman,
  DiffieHellmanGroup,
  ECDH,
  Hash,
  Hmac,
  Sign,
  Verify
};

function setFipsDisabled() {
  throw new ERR_CRYPTO_FIPS_UNAVAILABLE();
}

function setFipsForced(val) {
  if (val) return;
  throw new ERR_CRYPTO_FIPS_FORCED();
}

function getFipsDisabled() {
  return 0;
}

function getFipsForced() {
  return 1;
}

Object.defineProperties(exports, {
  _toBuf: {
    enumerable: false,
    value: deprecate(toBuf, 'crypto._toBuf is deprecated.', 'DEP0114')
  },
  createCipher: {
    enumerable: false,
    value: deprecate(createCipher,
                     'crypto.createCipher is deprecated.', 'DEP0106')
  },
  createDecipher: {
    enumerable: false,
    value: deprecate(createDecipher,
                     'crypto.createDecipher is deprecated.', 'DEP0106')
  },
  // crypto.fips is deprecated. DEP0093. Use crypto.getFips()/crypto.setFips()
  fips: {
    get: !fipsMode ? getFipsDisabled :
      fipsForced ? getFipsForced : getFipsCrypto,
    set: !fipsMode ? setFipsDisabled :
      fipsForced ? setFipsForced : setFipsCrypto
  },
  DEFAULT_ENCODING: {
    enumerable: true,
    configurable: true,
    get: deprecate(getDefaultEncoding,
                   'crypto.DEFAULT_ENCODING is deprecated.', 'DEP0091'),
    set: deprecate(setDefaultEncoding,
                   'crypto.DEFAULT_ENCODING is deprecated.', 'DEP0091')
  },
  constants: {
    configurable: false,
    enumerable: true,
    value: constants
  },

  // Aliases for randomBytes are deprecated.
  // The ecosystem needs those to exist for backwards compatibility with
  // ancient Node.js runtimes (0.10, 0.12).
  prng: {
    enumerable: false,
    value: deprecate(randomBytes, 'crypto.prng is deprecated.', 'DEP0115')
  },
  pseudoRandomBytes: {
    enumerable: false,
    value: deprecate(randomBytes,
                     'crypto.pseudoRandomBytes is deprecated.', 'DEP0115')
  },
  rng: {
    enumerable: false,
    value: deprecate(randomBytes, 'crypto.rng is deprecated.', 'DEP0115')
  }
});
