#ifndef DUMAGEVIEW_ASSERT_H_
#define DUMAGEVIEW_ASSERT_H_

#include <boost/assert.hpp>

/**
 * Custom assertion macro.
 * This avoids depending on a specific implementation.
 */
#define DUMAGEVIEW_ASSERT BOOST_ASSERT

#endif  // DUMAGEVIEW_ASSERT_H_
