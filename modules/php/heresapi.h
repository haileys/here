#ifndef HERESAPI_H
#define HERESAPI_H

#include <php.h>
#include <SAPI.h>
#include <php_main.h>
#include <php_variables.h>
#include <php_ini.h>
#include <zend_ini.h>

#include <stdbool.h>

bool herephp_initialize();

#endif