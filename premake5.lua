workspace "threaded_messenger_wps"
location ("build")
configurations { "release", "debug" }

path.iscppfile = function (fname)
  return path.hasextension(fname, { ".cc", ".cpp", ".c++", ".cxx", ".c", ".s", ".m", ".mm" })
end

newaction {
   trigger     = "clean",
   description = "cleans all the build files",
   execute     = function ()
      os.rmdir("./build")
      print("done.")
   end
}

buildoptions "-std=c++14"

filter "toolset:clang"
  buildoptions "-stdlib=libc++"
  linkoptions "-stdlib=libc++"
filter {}

filter "configurations:debug"
  defines { "DEBUG" }
  symbols "On"

filter "configurations:release"
  optimize "Full"

include "lib.lua"
include "tests.lua"



