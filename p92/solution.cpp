#include <iostream>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

// Returns the terminal value of n, either 1 or 89. log contains previous
// results.
int eval(int n, int log[100000000]) {
  // Calculated already?
  if (log[n] != 0)
    return log[n];

  // Recursively find answer
  int next = 0;
  int m = n;
  while (m > 0) {
    next += (m % 10) * (m % 10);
    m /= 10;
  }
  log[n] = eval(next, log);
  return log[n];
}

int main() {
  auto t1 = high_resolution_clock::now();

  int* log = new int[10000000];
  log[1] = 1;
  log[89] = 89;
  int count = 0;
  for (int i = 1; i < 10000000; i++)
    if (eval(i, log) == 89)
      count++;
  std::cout << "Answer: " << count << std::endl;

  auto t2 = high_resolution_clock::now();
  auto d = duration_cast<microseconds>(t2 - t1).count();
  std::cout << "Run time: " << (d / 1000) << " ms" << std::endl;
}
