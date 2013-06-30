CoreFoundation's C APIs are kind of a pain to use from C++.

This is an experiment to make them better, since I couldn't find any C++ CF bindings through cursory Googling.


Headers you might be interested in are under 'cfxx'.

There are component tests! You can run them with:
- mkdir build
- cd build
- cmake ..
- make
