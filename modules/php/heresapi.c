#include <stdlib.h>
#include <stdio.h>
#include "heresapi.h"

static int herephp_startup(sapi_module_struct* module);
static int herephp_ubwrite(const char* str, uint len TSRMLS_DC);
static void herephp_logmsg(char* str);

sapi_module_struct here_sapi_module = {
    "herephp",          // name
    "here PHP Module",  // friendly name
    // functions:
    herephp_startup,     // startup
    php_module_shutdown_wrapper, // shutdown
    NULL,               // activate
    NULL,               // deactivate
    herephp_ubwrite,    // unbuffered write
    NULL,               // flush
    NULL,               // get uid
    NULL,               // getenv
    php_error,          // error handler
    NULL,               // header handler
    NULL,               // send headers handler
    NULL,               // send header handler
    NULL,               // read POST data
    NULL,               // read cookies
    NULL,               // register server variables
    herephp_logmsg,     // log message
    
    STANDARD_SAPI_MODULE_PROPERTIES
};

static int herephp_startup(sapi_module_struct* module)
{
    return php_module_startup(module, NULL, 0);
}
static int herephp_ubwrite(const char* str, uint len TSRMLS_DC)
{
    printf("PHP wants to write %d bytes\n", len);
    return 0;
}
static void herephp_logmsg(char* str)
{
    printf("PHP: %s\n", str);
}

static void herephp_sapi_error(int type, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf(stderr, fmt, ap);
    printf("\n");
    va_end(ap);
}

bool herephp_initialize()
{
    here_sapi_module.sapi_error = herephp_sapi_error;
    zend_llist global_vars;
    
#ifdef ZTS
    zend_compiler_globals* compiler_globals;
    zend_executor_globals* executor_globals;
    php_core_globals* core_globals;
    sapi_globals_struct* sapi_globals;
    void*** tsrm_ls;
    
    tsrm_startup(1, 1, 0, NULL);
    
    compiler_globals = ts_resource(compiler_globals_id);
    executor_globals = ts_resource(executor_globals_id);
    core_globals = ts_resource(core_globals_id);
    sapi_globals = ts_resource(sapi_globals_id);
    tsrm_ls = ts_resource(0);
    *ptsrm_ls = tsrm_ls;
#endif

    sapi_startup(&here_sapi_module);
    if(here_sapi_module.startup(&here_sapi_module) == FAILURE) {
        return false;
    }
    here_sapi_module.executable_location = (char*)__func__;
    
    zend_llist_init(&global_vars, sizeof(char*), NULL, 0);

    if(php_request_startup(TSRMLS_C) == FAILURE) {
        php_module_shutdown(TSRMLS_C);
        return false;
    }
    
    SG(headers_sent) = 1;
    SG(request_info).no_headers = 1;
    php_register_variable("PHP_SELF", "-", NULL TSRMLS_CC);
    
    return true;
}