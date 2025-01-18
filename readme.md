A small library to log using the `log` service.

Usage:

```
#include <loglib.h>

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
```

In case the logger service is not available when the library is used, it caches the
requests, and keeps checking is the service is able to accept new logging requests.
