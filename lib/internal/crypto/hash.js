'use strict';

const {
  Hash: _Hash,
  Hmac: _Hmac
} = process.binding('crypto');

const {
  getDefaultEncoding,
  kHandle,
  legacyNativeHandle,
  toBuf
} = require('internal/crypto/util');

const { Buffer } = require('buffer');

const {
  ERR_CRYPTO_HASH_DIGEST_NO_UTF16,
  ERR_CRYPTO_HASH_FINALIZED,
  ERR_CRYPTO_HASH_UPDATE_FAILED,
  ERR_INVALID_ARG_TYPE
} = require('internal/errors').codes;
const { validateString } = require('internal/validators');
const { inherits } = require('util');
const { normalizeEncoding } = require('internal/util');
const { isArrayBufferView } = require('internal/util/types');
const LazyTransform = require('internal/streams/lazy_transform');
const kState = Symbol('kState');
const kFinalized = Symbol('kFinalized');

function Hash(algorithm, options) {
  if (!(this instanceof Hash))
    return new Hash(algorithm, options);
  validateString(algorithm, 'algorithm');
  legacyNativeHandle(this, new _Hash(algorithm));
  this[kState] = {
    [kFinalized]: false
  };
  LazyTransform.call(this, options);
}

inherits(Hash, LazyTransform);

Hash.prototype._transform = function _transform(chunk, encoding, callback) {
  this[kHandle].update(chunk, encoding);
  callback();
};

Hash.prototype._flush = function _flush(callback) {
  this.push(this[kHandle].digest());
  callback();
};

Hash.prototype.update = function update(data, encoding) {
  const state = this[kState];
  if (state[kFinalized])
    throw new ERR_CRYPTO_HASH_FINALIZED();

  if (typeof data !== 'string' && !isArrayBufferView(data)) {
    throw new ERR_INVALID_ARG_TYPE('data',
                                   ['string', 'TypedArray', 'DataView'], data);
  }

  if (!this[kHandle].update(data, encoding || getDefaultEncoding()))
    throw new ERR_CRYPTO_HASH_UPDATE_FAILED();
  return this;
};


Hash.prototype.digest = function digest(outputEncoding) {
  const state = this[kState];
  if (state[kFinalized])
    throw new ERR_CRYPTO_HASH_FINALIZED();
  outputEncoding = outputEncoding || getDefaultEncoding();
  if (normalizeEncoding(outputEncoding) === 'utf16le')
    throw new ERR_CRYPTO_HASH_DIGEST_NO_UTF16();

  // Explicit conversion for backward compatibility.
  const ret = this[kHandle].digest(`${outputEncoding}`);
  state[kFinalized] = true;
  return ret;
};


function Hmac(hmac, key, options) {
  if (!(this instanceof Hmac))
    return new Hmac(hmac, key, options);
  validateString(hmac, 'hmac');
  if (typeof key !== 'string' && !isArrayBufferView(key)) {
    throw new ERR_INVALID_ARG_TYPE('key',
                                   ['string', 'TypedArray', 'DataView'], key);
  }
  legacyNativeHandle(this, new _Hmac());
  this[kHandle].init(hmac, toBuf(key));
  this[kState] = {
    [kFinalized]: false
  };
  LazyTransform.call(this, options);
}

inherits(Hmac, LazyTransform);

Hmac.prototype.update = Hash.prototype.update;

Hmac.prototype.digest = function digest(outputEncoding) {
  const state = this[kState];
  outputEncoding = outputEncoding || getDefaultEncoding();
  if (normalizeEncoding(outputEncoding) === 'utf16le')
    throw new ERR_CRYPTO_HASH_DIGEST_NO_UTF16();

  if (state[kFinalized]) {
    const buf = Buffer.from('');
    return outputEncoding === 'buffer' ? buf : buf.toString(outputEncoding);
  }

  // Explicit conversion for backward compatibility.
  const ret = this[kHandle].digest(`${outputEncoding}`);
  state[kFinalized] = true;
  return ret;
};

Hmac.prototype._flush = Hash.prototype._flush;
Hmac.prototype._transform = Hash.prototype._transform;

module.exports = {
  Hash,
  Hmac
};
