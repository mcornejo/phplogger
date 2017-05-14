#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "main/SAPI.h"
#include "instrumentation.h"
#include <unistd.h>
#include <syslog.h>

ZEND_DECLARE_MODULE_GLOBALS(instrumentation);

/* PHP_FE adds new PHP function to the functions list. */
/* function_name, argument_types (NULL, array of flags, etc) */
/* set {NULL, NULL, NULL} as the last record to mark the end of list */
static zend_function_entry instrumentation_functions[] = {
        PHP_FE(sqreenOn, NULL)
        PHP_FE(sqreenOff, NULL)
        {NULL, NULL, NULL}
};

/* To make the extension be source-compatible with PHP 4.0.x we add some conditions */
zend_module_entry instrumentation_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
        STANDARD_MODULE_HEADER,
#endif
        PHP_MY_EXTENSION_EXTNAME,
        instrumentation_functions,
        PHP_MINIT(instrumentation),
        PHP_MSHUTDOWN(instrumentation),
        NULL,
        NULL,
        NULL,
#if ZEND_MODULE_API_NO >= 20010901
        PHP_MY_EXTENSION_VERSION,
#endif
        STANDARD_MODULE_PROPERTIES
};


/* C code to build a dynamic loaded extension */
ZEND_GET_MODULE(instrumentation);

// Init the globals
static void php_globals_init(zend_instrumentation_globals *instrumentation_global TSRMLS_DC) {
    instrumentation_global->my_functions = NULL;
    instrumentation_global->activated = false;
}

PHP_MINIT_FUNCTION(instrumentation) {
    #ifdef ZTS
        ts_allocate_id(&instrumentation_globals_id, sizeof(zend_instrumentation_globals), php_globals_init, NULL);
    #else
        php_globals_init(&instrumentation_globals);
    #endif
    M_G(my_functions) = (HashTable *) pemalloc(sizeof(HashTable), 1);
    zend_hash_init(M_G(my_functions), 1000000, NULL, NULL, 1);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(instrumentation){

        if (M_G(my_functions)) {
            zend_hash_destroy(M_G(my_functions));
            pefree(M_G(my_functions), 1);
            M_G(my_functions) = NULL;
        }

        return SUCCESS;
}

void log_data(zval ***args){
    char *arg1 = (*args[0])->value.str.val;
    char *arg2 = (*args[1])->value.str.val;
    syslog(LOG_WARNING, "filename: %s, mode: %s", arg1, arg2);
}


PHP_NAMED_FUNCTION(execute_logger){
        php_my_zif *fe;
        // tries to access the executor global function_state_ptr and get the name.
        char *fname = (char *) get_active_function_name(TSRMLS_C);

        /*Get the parameters*/
        zval **args[4];
        int argc = ZEND_NUM_ARGS();
        if (argc >= 1 && zend_get_parameters_array_ex(argc, args) != FAILURE) {
            log_data(args);
        }

        if (zend_hash_find(M_G(my_functions), fname, strlen(fname) + 1, (void**)&fe) != FAILURE) {
            (*fe)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
}

PHP_NAMED_FUNCTION(execute_original){
        php_my_zif *fe;
        char *fname = (char *) get_active_function_name(TSRMLS_C);
        if (zend_hash_find(M_G(my_functions), fname, strlen(fname) + 1, (void**)&fe) != FAILURE) {
            (*fe)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
}

int activate_sqreen(char *fname TSRMLS_DC) {
    zend_internal_function *fe;
    int fname_len = strlen(fname);

    if (zend_hash_find(CG(function_table), fname, fname_len + 1, (void**)&fe) == FAILURE) {
        return FAILURE;
    }

    if (fe->type != ZEND_INTERNAL_FUNCTION) {
        return FAILURE;
    }

    // if(zend_hash_add(M_G(stolen_functions), fname, strlen(fname)+1, (void*)&(fe->handler), sizeof(zend_function), NULL) == FAILURE){
    if(zend_hash_add(M_G(my_functions), fname, strlen(fname)+1, (void*)&(fe->handler), sizeof(php_my_zif), NULL) == FAILURE){
        return FAILURE;
    }

    fe->handler = execute_logger;
    return SUCCESS;
}

int deactivate_sqreen(char *fname TSRMLS_DC){
    zend_internal_function *fe;
    void *fo_handler;
    int fname_len = strlen(fname);

    if (zend_hash_find(CG(function_table), fname, fname_len + 1, (void**)&fe) == FAILURE) {
        return FAILURE;
    }

    if (zend_hash_find(M_G(my_functions), fname, fname_len + 1, (void**)&fo_handler) == FAILURE) {
        return FAILURE;
    }

    fe->handler = execute_original;

    /*
     // A better option would be to recover the original handler, then delete the handler from the HashTable
     if(zend_hash_del(M_G(my_functions), fname, strlen(fname)+1) == FAILURE){
        return FAILURE;
    }
    */
    return SUCCESS;
}

PHP_FUNCTION(sqreenOn) {
        M_G(activated) = true;
        char *orig_func = "fopen";
        activate_sqreen(orig_func);
}

PHP_FUNCTION(sqreenOff) {
        M_G(activated) = false;
        deactivate_sqreen("fopen");
}