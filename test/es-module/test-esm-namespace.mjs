// Flags: --experimental-modules
import '../common';
import * as fs from 'fs';
import assert from 'assert';
import Module from 'module';

const keys = Object.entries(
  Object.getOwnPropertyDescriptors(new Module().require('fs')))
  .filter(([name, d]) => d.enumerable)
  .map(([name]) => name)
  .concat('default')
  .sort();

assert.deepStrictEqual(Object.keys(fs).sort(), keys);
