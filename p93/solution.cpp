#include <algorithm>
#include <chrono>
#include <string>
#include <iostream>

/*  This program uses a brute force solution by iterating over every possible
 *  combination of numbers, operations, and parentheses. It sounds inefficient
 *  and could definitely be improved, however the search space for the problem
 *  is not actually that big and the program still runs in under 200 ms on my
 *  machine.
 *
 *  The brute force iterates through each of the following layers:
 *  1. Iterate over all numbers 0 <= a < b < c < d <= 9. (10 c 4 = 210 choices)
 *  2. Permute the set of 4 numbers in every possible way. (4! = 24 choices)
 *  3. Iterate over every sequence of operations. (4 * 4 * 4 = 64 choices)
 *  4. Iterate over every possible arrangement of parentheses. (5 choices)
 *
 *  In total, the compute function is called 210 * 24 * 64 * 5 = 1,612,800
 *  times; not actually that much for a simple function.
 */

// These represent the four different operations.
enum Op {
  PLUS = 0,
  MINUS = 1,
  TIMES = 2,
  DIVIDE = 3
};

// These represent the five different arrangements of parentheses that can be
// used. There wasn't really a more useful way to name these.
enum Prec {
  A = 0, // (a + b) + (c + d)
  B = 1, // a + ((b + c) + d)
  C = 2, // (a + (b + c)) + d
  D = 3, // ((a + b) + c) + d
  E = 4, // a + (b + (c + d))
};

int trial(int, int, int, int);
int compute(int[4], Op[3], Prec);
double operate(double, double, Op);

// Main method.
int main() {

  // Start timer
  auto t1 = std::chrono::high_resolution_clock::now();

  // Best solution found so far
  int answer[4] = {};
  int bestStreak = 0;

  // Iterate over all 0<=a<b<c<d<=9
  for (int a = 0; a <= 6; a++) {
    for (int b = a + 1; b <= 7; b++) {
      for (int c = b + 1; c <= 8; c++) {
        for (int d = c + 1; d <= 9; d++) {
          // Run trial for a,b,c,d
          int streak = trial(a,b,c,d);
          if (streak > bestStreak) {
            bestStreak = streak;
            answer[0] = a;
            answer[1] = b;
            answer[2] = c;
            answer[3] = d;
          }
        }
      }
    }
  }

  std::cout << "Answer: " << answer[0] << answer[1] << answer[2] << answer[3];
  std::cout << std::endl;

  // Calculate run time
  auto t2 = std::chrono::high_resolution_clock::now();
  auto d =std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
  std::cout << "Runtime: " << (d / 1000) << " ms" << std::endl;
}

// With the given abcd, returns the largest value n such that all integers from
// 1 to n can be expressed arithmetically, or 0 if 1 cannot even be expressed.
int trial(int a, int b, int c, int d) {
  bool canExpress[3025] = {};

  // Permute over all orderings of abcd
  int perms[] = {a, b, c, d};
  do {
    // Iterate over all choices of operations
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
          Op ops[3] = {(Op)i, (Op)j, (Op)k};
          // Iterate over all precedence trees
          for (int t = 0; t < 5; t++) {
            Prec prec = (Prec)t;
            int val = compute(perms, ops, prec);
            canExpress[val] = true;
          }
        }
      }
    }
  } while (std::next_permutation(perms, perms + 4));

  // Find n
  int n = 0;
  while (canExpress[++n]);
  return n - 1;
}

// Returns the numerical value of the arithmetic expression with the given
// values, operations, and precedence tree. If a non-integral value is
// calculated or division by 0 occurs, returns 0.
int compute(int nums[4], Op ops[3], Prec p) {
  double val = 0;
  try {
    if (p == Prec::A) { // (a + b) + (c + d)
      double left = operate(nums[0], nums[1], ops[0]);
      double right = operate(nums[2], nums[3], ops[2]);
      val = operate(left, right, ops[1]);
    }
    else if (p == Prec::B) { // a + ((b + c) + d)
      val = operate(nums[1], nums[2], ops[1]);
      val = operate(val, nums[3], ops[2]);
      val = operate(nums[0], val, ops[0]);
    }
    else if (p == Prec::C) { // (a + (b + c)) + d
      val = operate(nums[1], nums[2], ops[1]);
      val = operate(nums[0], val, ops[0]);
      val = operate(val, nums[3], ops[2]);
    }
    else if (p == Prec::D) { // ((a + b) + c) + d
      val = operate(nums[0], nums[1], ops[0]);
      val = operate(val, nums[2], ops[1]);
      val = operate(val, nums[3], ops[2]);
    }
    else if (p == Prec::E) { // a + (b + (c + d))
      val = operate(nums[2], nums[3], ops[2]);
      val = operate(nums[1], val, ops[1]);
      val = operate(nums[0], val, ops[0]);
    }
  }
  // Catch division by 0
  catch (...) {
    return 0;
  }
  // Check if resulting value is an integer. Note that this does not work in
  // general but should suffice for this level of accuracy.
  if (val == (int)val && val > 0)
    return (int)val;
  else
    return 0;
}

// Performs the operation l * r, where * is some operation. Throws a string
// exception if division by 0 is attempted.
double operate(double l, double r, Op op) {
  if (op == Op::PLUS)
    return l + r;
  else if (op == Op::MINUS)
    return l - r;
  else if (op == Op::TIMES)
    return l * r;
  else if (op == Op::DIVIDE && r != 0.0)
    return l / r;
  else
    throw "Division by 0";
}
