#include <codetimer/codetimer.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

using namespace std::chrono;

struct RecordedValue {
 public:
  std::vector<float> secs;
  std::string name;
};

void CodeTimer::record(std::string name, std::chrono::high_resolution_clock::time_point start) {
  auto stop = high_resolution_clock::now();
  auto diff = duration_cast<duration<float>>(stop - start);
  auto updateFunction = [&diff](std::unique_ptr<RecordedValue>& existingValue) {
    existingValue->secs.push_back(diff.count());
  };
  // We try a regular update before calling upsert here. We assume that updates will be much
  // more common than insertions, and this avoids creating unnecessary RecordedValue instances
  // when performing updates. Unnecessary RecordedValue instances will only be created when
  // there are race conditions between threads, which is likely to be rare.
  if (!CodeTimer::timerMap.update_fn(name, updateFunction)) {
    RecordedValue* newValue = new RecordedValue();
    newValue->secs.push_back(diff.count());
    newValue->name = name;
    CodeTimer::timerMap.upsert(name, updateFunction, std::unique_ptr<RecordedValue>(newValue));
  }
}

void CodeTimer::printStats() {
  std::vector<RecordedValue*> vals;
  vals.reserve(timerMap.size());
  for (const auto& item : CodeTimer::timerMap.lock_table()) {
    vals.push_back(item.second.get());
  }
  std::sort(begin(vals), end(vals), [](RecordedValue* t1, RecordedValue* t2) {
    return t1->name > t2->name;
  });
  for (auto val : vals) {
    float sum = std::accumulate(val->secs.begin(), val->secs.end(), 0.0f);
    float mean = sum / static_cast<float>(val->secs.size());
    std::vector<float> diff(val->secs.size());
    std::transform(val->secs.begin(), val->secs.end(), diff.begin(), [mean](float x) { return x - mean; });
    float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0f);
    float stdev = std::sqrt(sq_sum / static_cast<float>(val->secs.size()));
    std::cout << val->name << ": total=" << sum << "s; occurrences=" << val->secs.size() << "; average=" << mean << "s; std. dev=" << stdev << "s";
    std::cout << std::endl;
  }
}

std::stringstream CodeTimer::streamStats() {
  std::vector<RecordedValue*> vals;
  vals.reserve(timerMap.size());
  std::stringstream str;
  for (const auto& item : CodeTimer::timerMap.lock_table()) {
    vals.push_back(item.second.get());
  }
  std::sort(begin(vals), end(vals), [](RecordedValue* t1, RecordedValue* t2) {
    return t1->name > t2->name;
  });
  for (auto val : vals) {
    float sum = std::accumulate(val->secs.begin(), val->secs.end(), 0.0f);
    float mean = sum / static_cast<float>(val->secs.size());
    std::vector<float> diff(val->secs.size());
    std::transform(val->secs.begin(), val->secs.end(), diff.begin(), [mean](float x) { return x - mean; });
    float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0f);
    float stdev = std::sqrt(sq_sum / static_cast<float>(val->secs.size()));
    str << val->name << ": total=" << sum << "s; occurrences=" << val->secs.size() << "; average=" << mean << "s; std. dev=" << stdev << "s";
    str << std::endl;
  }
  return str;
}

libcuckoo::cuckoohash_map<std::string, std::unique_ptr<RecordedValue>> CodeTimer::timerMap = {};