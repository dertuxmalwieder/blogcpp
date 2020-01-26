/*
 * blogcpp :: https://www.blogcpp.org
 * Filesystem class detector.
 */

#if defined(__has_include)
#  if __has_include(<experimental/filesystem>)
//   Some compilers are not really fast.
#    include <experimental/filesystem>
#    define NAMESPACE_FILESYSTEM std::experimental::filesystem
#  endif
#endif
#ifndef NAMESPACE_FILESYSTEM
#  include <filesystem>
#  define NAMESPACE_FILESYSTEM std::filesystem
#endif
