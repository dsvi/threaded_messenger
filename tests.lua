project "tests"
  kind "ConsoleApp"
  links { "threaded_messenger", "pthread" }
  language "C++"
  files {
     "lest.hpp",
     "test.c++"
  }

