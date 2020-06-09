# Introduction

CodeTimer is a very simple utility for recording the performance of blocks of code in C++. It lets us record timings against a specific key,
and then aggregates all of the timings for a key.

CodeTimer is designed to perform well when called concurrently by multiple threads. Internally it uses a
[libcuckoo](https://github.com/efficient/libcuckoo) hash table.

CodeTimer meaures [wall-clock time](https://en.wikipedia.org/wiki/Wall-clock_time) (as opposed to [CPU time](https://en.wikipedia.org/wiki/CPU_time)).
It uses the [&lt;chrono&gt; library](http://www.cplusplus.com/reference/chrono/s)'s
[high_resolution_clock](http://www.cplusplus.com/reference/chrono/high_resolution_clock/) to measure timings with high precision.

Here is a simple usage example:

```c++
#include <chrono>
#include <iostream>
#include <thread>
#include "codetimer.h"
void sleepMillis(long milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int main () {

    auto start = std::chrono::high_resolution_clock::now();
    sleepMillis(100);
    CodeTimer::record("key1", start);

    start = std::chrono::high_resolution_clock::now();
    sleepMillis(200);
    CodeTimer::record("key2", start);

    start = std::chrono::high_resolution_clock::now();
    sleepMillis(300);
    CodeTimer::record("key1", start);

    CodeTimer::printStats();

    return 0;
}
```

When run this will produce something like the following:

```
key1: total=0.400211; occurrences=2
key2: total=0.200093; occurrences=1
```

For each key that we recorded a timing against, we can see the total time for that key, and the number of times we recorded a timing for the key.

In the above example we use `CodeTimer::record` to record a timing for a specific key, and ```CodeTimer::printStats()``` to print an aggregated view
of the timings we have recorded. See the API docs in [codetimer.h](codetimer.h) for more details.

# Building and Running

Updated from upstream fork to use Modern CMake. Build and install using standard cmake build process
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make install
```

Designed so that it can also be used within a larger project as an ```add_subdirectory()```
# License

CodeTimer is licensed under the [Apache License, Version 2.0](LICENSE.md)















