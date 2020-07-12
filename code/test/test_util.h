#ifndef HC_TEST_UTIL_H
#define HC_TEST_UTIL_H

static const bool debug = false;
#define derr if (debug) std::cerr

#define IMPLIES(a, b) (!a || b)

#endif //HC_TEST_UTIL_H
