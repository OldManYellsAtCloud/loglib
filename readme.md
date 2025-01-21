A small library to log using the `log` service.

Usage:

```
#include <loglib/loglib.h>

// ...

// setname must be done before anything else
// Name is used as filename, or indicated in console output
loglib::logger().setName("Mr logger");

// register the required loggers
// can be done at any time, new ones can be registered later too
// ideally at least 1 logger should be registered, before logging anything
// currently LOGGER_FILE or LOGGER_CONSOLE are available
loglib::logger().registerLogger(LOGGER_TYPE::LOGGER_FILE);

// start logging, anywhere.
loglib::logger().debug("xxxxx don't segfault xxxx");
loglib::logger().fatal("that's not good");

// or use one of the macros (which is equivalent to the previous ones)
LOG_DEBUG("xxxxx don't segfault xxxx");
LOG_FATAL("that's not good");

// or use one of the macros that format the output (_F postfix)
LOG_DEBUG_F("this is a number: {}", 2);
LOG_WARNING_F("I warned you not {}, but {}", "once", "twice");

// it is also possible to register multiple names, if needed
loglib::logger().registerLogger(LOGGER_TYPE::LOGGER_FILE, "name2");

// use the macros having _N postfix (n = named) to use other than
// the default name
LOG_DEBUG_N("debug message from another section", "name2");
LOG_DEBUG_NF("a {} debug message this time", "formatted", "name2");

```

In case the logger service is not available when the library is used, it caches the
requests, and keeps checking is the service is able to accept new logging requests.
