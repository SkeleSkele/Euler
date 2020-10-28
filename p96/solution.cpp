#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using std::chrono::microseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

// Top row:   0,  1, ...,  8,
// 2nd row:   9, 10, ..., 16,
// ...
// Last row: 72, 73, ..., 80
struct Sudoku {
  int grid[81];
  std::vector<int> emptyCells; // contains indices of empty cells
};

// Returns a vector containing the moves that could be legally made in the
// given cell
std::vector<int> getLegalMoves(int cell, Sudoku &sudoku) {
  bool usedMoves[10] = {}; // available legal moves will remain false

  // Iterate over row
  int row = cell / 9;
  for (int i = row * 9; i < (row + 1) * 9; i++)
    usedMoves[sudoku.grid[i]] = true;
  // Iterate over column
  int col = cell % 9;
  for (int i = col % 9; i < 81; i += 9)
    usedMoves[sudoku.grid[i]] = true;

  // Find index of upper left cell sharing this cell's 3x3 box
  int topLeft = (row / 3) * 27 + (col / 3) * 3;
  // Iterate over the rest of the box
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int x = topLeft + i * 9 + j;
      usedMoves[sudoku.grid[x]] = true;
    }
  }

  // Generate vector of remaining moves
  std::vector<int> legalMoves;
  for (int i = 1; i <= 9; i++)
    if (!usedMoves[i])
      legalMoves.push_back(i);

  return legalMoves;
}

// Recursive method to solve the sudoku. When done, s will be solved.
bool solve(Sudoku &s) {
  // Base case: puzzle already solved
  if (s.emptyCells.size() == 0)
    return true;

  // Pick an empty cell and generate possible moves
  int cell = s.emptyCells.back();
  s.emptyCells.pop_back();
  std::vector<int> moves = getLegalMoves(cell, s);

  // Try each legal move
  for (int m : moves) {
    s.grid[cell] = m;
    if (solve(s))
      return true;
  }

  // Nothing worked, so leave the cell empty and go back
  s.emptyCells.push_back(cell);
  s.grid[cell] = 0;
  return false;
}

// Reads from the file and returns a Sudoku struct
Sudoku loadSudoku(std::ifstream &file) {
  std::string line;
  getline(file, line); // The first line is unused
  Sudoku s;
  for (int i = 0; i < 9; i++) {
    getline(file, line);
    for (int j = 0; j < 9; j++) {
      char c = line[j];
      s.grid[i*9 + j] = c - '0';
      if (c == '0')
        s.emptyCells.push_back(i*9 + j);
    }
  }
  return s;
}

int main() {
  // Start timer
  auto t1 = high_resolution_clock::now();

  // Find answer
  std::ifstream file("sudoku.txt");
  int sum = 1;
  for (int i = 1; i <= 50; i++) {
    auto r1 = high_resolution_clock::now(); // 
    Sudoku s = loadSudoku(file);
    solve(s);
    sum += s.grid[0]*100 + s.grid[1]*10 + s.grid[2];
    auto r2 = high_resolution_clock::now();
    auto d = duration_cast<microseconds>(r2-r1).count();
    std::cout << i << ": " << (d/1000) << " ms" << std::endl;
  }
  std::cout << "Answer: " << sum << std::endl;

  // Calculate run time
  auto t2 = high_resolution_clock::now();
  auto d = duration_cast<microseconds>(t2-t1).count();
  std::cout << "Runtime: " << (d / 1000) << " ms" << std::endl;
}
