# CMake generated Testfile for 
# Source directory: /home/koki/projects/latency-benchmarking
# Build directory: /home/koki/projects/latency-benchmarking/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(socket_wrapper_tests "/home/koki/projects/latency-benchmarking/build/tests")
set_tests_properties(socket_wrapper_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/koki/projects/latency-benchmarking/CMakeLists.txt;53;add_test;/home/koki/projects/latency-benchmarking/CMakeLists.txt;0;")
subdirs("external/googletest")
