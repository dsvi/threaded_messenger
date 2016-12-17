project "threaded_messenger"
  kind "StaticLib"
  language "C++"
  files {
     "*.h",
     "message_queue.c++",
     "group.c++"
  }
  links {"pthread" }
  


