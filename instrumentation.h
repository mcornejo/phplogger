// // right here! https://devzone.zend.com/303/extension-writing-part-i-introduction-to-php-and-zend/
#ifndef CODE_INSTRUMENTATION_H
#define CODE_INSTRUMENTATION_H

/* We define an extern variable instrumentation of type zend_module_entry */
typedef void (*php_my_zif)(INTERNAL_FUNCTION_PARAMETERS);

ZEND_BEGIN_MODULE_GLOBALS(instrumentation)
HashTable *my_functions;
zend_bool activated;
ZEND_END_MODULE_GLOBALS(instrumentation)
extern ZEND_DECLARE_MODULE_GLOBALS(instrumentation);



PHP_MINIT_FUNCTION(instrumentation);
PHP_MSHUTDOWN_FUNCTION(instrumentation);
PHP_RINIT_FUNCTION(instrumentation);


#define PHP_MY_EXTENSION_VERSION "1.0"
#define PHP_MY_EXTENSION_EXTNAME "instrumentation"

#ifdef  ZTS
#define	M_G(v) TSRMG(instrumentation_globals_id, zend_instrumentation_globals *, v)
#define M_TSRMLS_C TSRMLS_C
#else
#define	M_G(v) (instrumentation_globals.v)
#define M_TSRMLS_C  NULL
#endif



/* Function definition */
PHP_FUNCTION(sqreenOn);
PHP_FUNCTION(sqreenOff);


extern zend_module_entry instrumentation_module_entry;
#define phpext_my_extension_ptr &instrumentation_module_entry





#endif //CODE_INSTRUMENTATION_H


// IDEA: https://github.com/abiusx/phpx/blob/master/sample.c#L262