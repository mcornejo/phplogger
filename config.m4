PHP_ARG_ENABLE(instrumentation, whether to enable instrumentation support,
[--enable-instrumentation   Enable instrumentation support])

if test "$PHP_INSTRUMENTATION" = "yes"; then
    AC_DEFINE(HAVE_INSTRUMENTATION, 1, [Whether you have instrumentation])
    PHP_NEW_EXTENSION(instrumentation, instrumentation.c, $ext_shared)
fi