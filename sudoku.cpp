// sudoku.c
// libraries
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <cstring>
using namespace std;

// grid of sudoku board
int grid[9][9];
// grid to keep track of empty boxes
bool tracker[9][9];
// matrix of vectors to keep track of domains of variables
vector<vector<vector<int> > > domain;
// vector used to facilitate backtracking and finding MRV
vector<int> pos(7);
// structure used for backtracking
struct backtrack {
	int puzzle[9][9];
	int row;
	int col;
	int val;
	vector<int> value; // keep track of values already assigned to variable
	backtrack *parent; // keep track of steps to complete sudoku grid
} root, child;
// counter for number of assignments
int numOfAssignments = 0;

// read sudoku board from command line
int input(istream& in=cin) {
	int x;
	in >> x;
	return x;
}

// determine if placing num at (row,col) is a legal move
bool isAvailable(int puzzle[9][9], int row, int col, int num) {
	int rowStart = (row/3) * 3;
	int colStart = (col/3) * 3;
	for(int i = 0; i < 9; i++) {
		if (puzzle[row][i] == num) return false;
		if (puzzle[i][col] == num) return false;
		if (puzzle[rowStart + (i%3)][colStart + (i/3)] == num) return false;
	}
	return true;
}

// find LCV
int findLCV(int row, int col, vector<int> domainofVar) {
	int rowStart = (row/3) * 3;
	int colStart = (col/3) * 3;
	int counter = 0; // keep track of number of constrained variables
	int min = 81; // keep track of smallest LCV
	int LCV;
	// loop through domain of variable
	for (int j = 0; j < domainofVar.size(); j++) {
		int value = domainofVar.at(j);
		// check 3x3 grid
		for (int i = 0; i < 9; i++) {
			if (rowStart+(i%3) == row || colStart+(i/3) == col) { break; } // avoid double counting variables in 3x3 grid, will count in next for loop
			else if (find(domain[rowStart+(i%3)][colStart+(i/3)].begin(),domain[rowStart+(i%3)][colStart+(i/3)].end(),value) != domain[rowStart+(i%3)][colStart+(i/3)].end()){				
			counter++;
			}
		}
		// check row and column
		for (int i = 0; i <= 8; i++) {
			if (find(domain[row][i].begin(),domain[row][i].end(),value) != domain[row][i].end() && col == i) {
				counter++;
			}
			if (find(domain[i][col].begin(),domain[i][col].end(),value) != domain[i][col].end() && row == i) {
				counter++;
			}
		}
		// check if the current variable is the LCV
		if (counter < min) {
			min = counter;
			LCV = value;
		}
	}	
	return LCV;
}

// find MRV
vector<int> findMRV () {
	int min = 10; // keep track of MRV
	pos.at(0) = 9; // row
	pos.at(1) = 9; // column
	pos.at(2) = 10; // value
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (pos.at(3) == 1 && pos.at(4) == i && pos.at(5) == j && tracker[i][j] == true) { // case for backtracking
				for (int k = 0; k < domain[i][j].size(); k++) { // check other values in domain of variable in backtracking step
					int LCV = domain[i][j].at(k);
					if (find(root.value.begin(),root.value.end(),LCV) == root.value.end()) { // if another value found, assign to variable 
						pos.at(0) = i;
						pos.at(1) = j;
						pos.at(2) = LCV;
						pos.at(3) = 0;
						pos.at(4) = 9;
						pos.at(5) = 9;
						pos.at(6) = 10;
						root.value.push_back(LCV);
						return pos;
					}
				}
				pos.at(3) = 1; // keep backtracking
				return pos;
			}
			else if (domain[i][j].size() == 0 && tracker[i][j] == true) { // backtracking: empty cell with no domain
				pos.at(3) = 1;
				return pos;
			}
			else if (domain[i][j].size() < min && tracker[i][j] == true) { // keep track of MRV
				pos.at(0) = i;
				pos.at(1) = j;
				pos.at(2) = findLCV(i,j,domain[i][j]);
				min = domain[i][j].size(); 
			}
			else if (domain[i][j].size() == min && tracker[i][j] == true) { // use LCV as tiebreaker if two variables have the same MRV
				int LCV = findLCV(i,j,domain[i][j]);
				if (LCV < pos.at(2)) {
					pos.at(0) = i;
					pos.at(1) = j;
					pos.at(2) = LCV;
					min = domain[i][j].size(); 
				}
			}
		}
	}
	return pos;
}

// once a number has been added to grid, update domain of variables
void updateDomain(int row, int col, int value, int type) {
	int rowStart = (row/3) * 3;
	int colStart = (col/3) * 3;
	// remove variable from domain
	if (type == 1) {	
		// check 3x3 grid
		for (int i = 0; i < 9; i++) {
			if (find(domain[rowStart+(i%3)][colStart+(i/3)].begin(),domain[rowStart+(i%3)][colStart+(i/3)].end(),value) != domain[rowStart+(i%3)][colStart+(i/3)].end()){				
				domain[rowStart+(i%3)][colStart+(i/3)].erase(remove(domain[rowStart+(i%3)][colStart+(i/3)].begin(),domain[rowStart+(i%3)][colStart+(i/3)].end(),value),domain[rowStart+(i%3)][colStart+(i/3)].end());
			}
		}
		// check row and column
		for (int i = 0; i <= 8; i++) {
			if (find(domain[row][i].begin(),domain[row][i].end(),value) != domain[row][i].end()) {
				domain[row][i].erase(remove(domain[row][i].begin(),domain[row][i].end(),value),domain[row][i].end());
			}
			if (find(domain[i][col].begin(),domain[i][col].end(),value) != domain[i][col].end()) {
				domain[i][col].erase(remove(domain[i][col].begin(),domain[i][col].end(),value),domain[i][col].end());
			}
		}
	}
	// add variable to domain
	else {
		// check 3x3 grid
		for (int i = 0; i < 9; i++) {
			if (rowStart+(i%3) == row || colStart+(i/3) == col) { break; }
			else if (find(domain[rowStart+(i%3)][colStart+(i/3)].begin(),domain[rowStart+(i%3)][colStart+(i/3)].end(),value) == domain[rowStart+(i%3)][colStart+(i/3)].end()){				
				domain[rowStart+(i%3)][colStart+(i/3)].push_back(value);
			}
		}
		// check row and column
		for (int i = 0; i <= 8; i++) {
			if (find(domain[row][i].begin(),domain[row][i].end(),value) == domain[row][i].end() && col != i) {
				domain[row][i].push_back(value);
			}
			if (find(domain[i][col].begin(),domain[i][col].end(),value) == domain[i][col].end() && row != i) {
				domain[i][col].push_back(value);
			}
		}
	}
}


// fill sudoku board
int fillSudoku(int puzzle[9][9]){
	findMRV();
	if (pos.at(0) < 9 && pos.at(1) < 9 && pos.at(2) < 10) { // first value to add to sudoku board
		grid[pos.at(0)][pos.at(1)] = pos.at(2);
		numOfAssignments++;
		updateDomain(pos.at(0),pos.at(1),pos.at(2),1); // forward checking
		tracker[pos.at(0)][pos.at(1)] = false;
		memcpy(&root.puzzle,&grid,sizeof(grid));
		root.row = pos.at(0);
		root.col = pos.at(1);
		root.val = pos.at(2);
		root.value.push_back(pos.at(2));
	}
	while (numOfAssignments >= 0) {
		findMRV();
		if (numOfAssignments >= 10000) { // more than 10,000 step needed, stop running algorithm
			cout << "More than 10,000 steps needed" << endl; 
			break;		
		}
		else if (pos.at(3) == 1) { // backtracking needed
			grid[root.row][root.col] = 0;
			updateDomain(root.row,root.col,root.val,0); // forward checking
			tracker[root.row][root.col] = true;
			pos.at(4) = root.row; // variable used for backtracking
			pos.at(5) = root.col; // variable used for backtracking
			pos.at(6) = root.val; // variable used for backtracking
			root = *(root.parent); // backtrack to previous assignment
		}
		else if (pos.at(0) < 9 && pos.at(1) < 9 && pos.at(2) < 10) { // MRV found, assign value to variable
			grid[pos.at(0)][pos.at(1)] = pos.at(2);
			numOfAssignments++;
			updateDomain(pos.at(0),pos.at(1),pos.at(2),1); // forward checking
			tracker[pos.at(0)][pos.at(1)] = false;
			memcpy(&child.puzzle,&grid,sizeof(grid));
			child.row = pos.at(0);
			child.col = pos.at(1);
			child.val = pos.at(2);
			child.value.push_back(pos.at(2));
			child.parent = new backtrack(root);
			root = child;
		}
		else { break; }
	}
	return 1;
}

int main() {

	// create matrix of vectors for keeping track of domain of variables
	domain.resize(9);
	for (int i = 0; i < 9; i++) {
		domain[i].resize(9);
		for (int j = 0; j < 9; j++) {
			domain[i][j].resize(0);
		}
	}

	// read sudoku board
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			grid[i][j] = input();
			if (grid[i][j] == 0) { 
				tracker[i][j] = 1; // space is empty in initial state
			}
			else { 
				tracker[i][j] = 0; // space is not empty in initial state
			} 

		}
	}
	
	// update domain of variables
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			for (int k = 1; k <= 9; k++) {
				if (isAvailable(grid,i,j,k) && tracker[i][j]) { 
					domain[i][j].push_back(k); 
				}
			}
		}
	}

	// define pos vector values
	pos.at(0) = 9; // current row
	pos.at(1) = 9; // current col
	pos.at(2) = 10; // current value
	pos.at(3) = 0; // need to backtrack?
	pos.at(4) = 9; // backtracking row
	pos.at(5) = 9; // backtracking col
	pos.at(6) = 10; // backtracking value


	// fill sudoku board and print solution
	if (fillSudoku(grid)){
		for (int i=0; i<9; i++) {
			for (int j=0; j<9; j++){
				cout << grid[i][j]  << "  ";
			}
			cout << endl;
		}
		cout << "Number of Assignments: " << numOfAssignments << endl;
	}

	// no solution
	else {
		cout << "No Solution" << endl;
	}

}
