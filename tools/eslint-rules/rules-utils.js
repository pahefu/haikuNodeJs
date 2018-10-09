/**
 * Utility functions common to ESLint rules.
 */
'use strict';

module.exports.isDefiningError = function(node) {
  return node.expression &&
         node.expression.type === 'CallExpression' &&
         node.expression.callee &&
         node.expression.callee.name === 'E' &&
         node.expression.arguments.length !== 0;
};

/**
 * Returns true if any of the passed in modules are used in
 * require calls.
 */
module.exports.isRequired = function(node, modules) {
  return node.callee.name === 'require' && node.arguments.length !== 0 &&
    modules.includes(node.arguments[0].value);
};

/**
* Return true if common module is required
* in AST Node under inspection
*/
var commonModuleRegExp = new RegExp(/^(\.\.\/)*common(\.js)?$/);
module.exports.isCommonModule = function(node) {
  return node.callee.name === 'require' &&
         node.arguments.length !== 0 &&
         commonModuleRegExp.test(node.arguments[0].value);
};

/**
 * Returns true if any of the passed in modules are used in
 * binding calls.
 */
module.exports.isBinding = function(node, modules) {
  if (node.callee.object) {
    return node.callee.object.name === 'process' &&
           node.callee.property.name === 'binding' &&
           modules.includes(node.arguments[0].value);
  }
};

/**
 * Returns true is the node accesses any property in the properties
 * array on the 'common' object.
 */
module.exports.usesCommonProperty = function(node, properties) {
  if (node.name) {
    return properties.includes(node.name);
  }
  if (node.property) {
    return properties.includes(node.property.name);
  }
  return false;
};

/**
 * Returns true if the passed in node is inside an if statement block,
 * and the block also has a call to skip.
 */
module.exports.inSkipBlock = function(node) {
  var hasSkipBlock = false;
  if (node.test &&
      node.test.type === 'UnaryExpression' &&
      node.test.operator === '!') {
    const consequent = node.consequent;
    if (consequent.body) {
      consequent.body.some(function(expressionStatement) {
        if (hasSkip(expressionStatement.expression)) {
          return hasSkipBlock = true;
        }
        return false;
      });
    } else if (hasSkip(consequent.expression)) {
      hasSkipBlock = true;
    }
  }
  return hasSkipBlock;
};

function hasSkip(expression) {
  return expression &&
         expression.callee &&
         (expression.callee.name === 'skip' ||
         expression.callee.property &&
         expression.callee.property.name === 'skip');
}
