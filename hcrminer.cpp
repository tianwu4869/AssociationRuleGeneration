#include <stdio.h>
#include <stdlib.h> 
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
using namespace std;

int minsup;
float minconf;
vector<vector<int>> frequentItemsets;
vector<int> frequentItemsetsSupport;
int totalRules;

void frequentItemsetsGeneration(vector<int> px, vector<vector<int>> db) {
	vector<int> frequentItems;
	map<int, int> countItems;
	map<int, int>::iterator it;
	// Traverse the DB to count the support of each item.
	for (int i = 0; i < db.size(); i++) {
		for (int j = 0; j < db[i].size(); j++) {
			it = countItems.find(db[i][j]);
			if (it == countItems.end()) {
				countItems.insert(pair<int, int>(db[i][j], 1));
			}
			else {
				it->second++;
			}
		}
	}
	// Pick up items in map whose support is higher than minsup.
	it = countItems.begin();
	for (; it != countItems.end(); it++) {
		if (it->second >= minsup) {
			// Elimination
			vector<vector<int>> projectedDB;
			for (int i = 0; i < db.size(); i++) {
				for (int j = 0; j < db[i].size(); j++) {
					// Only retain items lexicographically greater than the frequent item.
					if (db[i][j] == it->first) {
						vector<int> temp;
						for (int k = j + 1; k < db[i].size(); k++) {
							temp.push_back(db[i][k]);
						}
						if (temp.size() > 0) {
							projectedDB.push_back(temp);
						}
						break;
					}
				}
			}
			// Recursion
			frequentItemsets.push_back(px);
			frequentItemsets[frequentItemsets.size() - 1].push_back(it->first);
			/*for (int i = 0; i < px.size(); i++) {
				cout << px[i] << ' ';
			}
			cout << it->first<<endl;*/
			//cout << frequentItemsets.size() << endl;
			frequentItemsetsSupport.push_back(it->second);
			frequentItemsetsGeneration(frequentItemsets[frequentItemsets.size() - 1], projectedDB);
		}
	}
}

int supportCounting(vector<int> itemset, vector<vector<int>> db) {
	int total = 0;
	vector<int>::iterator it;
	for (int i = 0; i < db.size(); i++) {
		for (int j = 0; j < itemset.size(); j++) {
			it = find(db[i].begin(), db[i].end(), itemset[j]);
			if (it == db[i].end()) {
				break;
			}
			else {
				if (j == itemset.size() - 1) {
					total++;
				}
			}
		}
	}
	return total;
}

void ruleGeneration(vector<int> itemset, vector<vector<int>> consequents, vector<vector<int>> db, int support, ofstream& output) {
	if (itemset.size() > consequents[0].size()) {
		//support = supportCounting(itemset, db);
		vector<vector<int>> infrequentConsequents;
		for (int i = 0; i < consequents.size(); i++) {
			vector<int> cause;
			vector<int>::iterator it;
			for (int j = 0; j < itemset.size(); j++) {
				it = find(consequents[i].begin(), consequents[i].end(), itemset[j]);
				if (it == consequents[i].end()) {
					cause.push_back(itemset[j]);
				}
			}
			int causeSupport = supportCounting(cause, db);
			float confidence = (float)support / causeSupport;
			if (confidence >= minconf) {
				totalRules++;
				if (minsup == 15 || minsup == 20) {
					for (int j = 0; j < itemset.size(); j++) {
						output << itemset[j] << ' ';
					}
					output << " | {} | " << support << " | -1" << "\n";
				}
				else {
					for (int j = 0; j < cause.size(); j++) {
						output << cause[j] << ' ';
					}
					output << "| ";
					for (int j = 0; j < consequents[i].size(); j++) {
						output << consequents[i][j] << ' ';
					}
					output << " | " << support << " | " << confidence << "\n";
				}
			}
			else {
				infrequentConsequents.push_back(consequents[i]);
				consequents.erase(consequents.begin() + i);
				i--;
			}
		}
		// Candidate generation
		vector<vector<int>> newConsequents;
		int size = consequents.size() - 1;
		for (int i = 0; i < size; i++) {
			for (int j = i + 1; j < consequents.size(); j++) {
				vector<int> consequent;
				// Check if the first k-1 elements are identical.
				bool pass = true;
				for (int k = 0; k < consequents[i].size() - 1; k++) {
					if (consequents[i][k] != consequents[j][k]) {
						pass = false;
						break;
					}
					else {
						consequent.push_back(consequents[i][k]);
					}
				}
				if (pass) {
					consequent.push_back(consequents[i][consequents[i].size() - 1]);
					consequent.push_back(consequents[j][consequents[i].size() - 1]);
					newConsequents.push_back(consequent);
				}
			}
		}
		// Candidate pruning
		for (int i = 0; i < newConsequents.size(); i++) {
			for (int j = 0; j < infrequentConsequents.size(); j++) {
				vector<int>::iterator it;
				int number = 0;
				for (int k = 0; k < infrequentConsequents[j].size(); k++) {
					it = find(newConsequents[i].begin(), newConsequents[i].end(), infrequentConsequents[j][k]);
					if (it == newConsequents[i].end()) {
						break;
					}
					else {
						number++;
					}
				}
				if (number == infrequentConsequents[j].size()) {
					newConsequents.erase(newConsequents.begin() + i);
					i--;
					break;
				}
			}
		}
		// Recursion
		if (newConsequents.size() > 0) {
			ruleGeneration(itemset, newConsequents, db, support, output);
		}
	}
}

void sortByFrequency(vector<vector<int>> &db, map<int, int> supportMap, int option) {
	for (int i = 0; i < db.size(); i++) {
		for (int j = 0; j < db[i].size() - 1; j++) {
			for (int k = 0; k < db[i].size() - 1; k++) {
				if (option == 2) {
					if (supportMap[db[i][k]] > supportMap[db[i][k + 1]]) {
						int temp;
						temp = db[i][k];
						db[i][k] = db[i][k + 1];
						db[i][k + 1] = temp;
					}
				}
				else {
					if (supportMap[db[i][k]] < supportMap[db[i][k + 1]]) {
						int temp;
						temp = db[i][k];
						db[i][k] = db[i][k + 1];
						db[i][k + 1] = temp;
					}
				}
			}
		}
	}
}

int main(int argc, const char* argv[]) {
	minsup = atoi(argv[1]);
	minconf = atof(argv[2]);
	string inputfile = argv[3];
	string outputfile = argv[4];
	int option = atoi(argv[5]);

	ifstream dataSet;
	dataSet.open(inputfile);
	if (!dataSet) {
		cout << "Unable to open the " << inputfile << endl;
		exit(1);
	}
	
	vector<vector<int>> db;
	vector<int> newTransaction;
	int TID, item;
	int currentLine = -1;
	while (dataSet >> TID >> item) {
		if (TID > currentLine) {
			db.push_back(newTransaction);
			currentLine = TID;
		}
		db[currentLine].push_back(item);
	}

	clock_t past, now;
	past = clock();

	// Sort items based on given option.
	if (option == 2 || option == 3) {
		map<int, int> supportMap;
		map<int, int>::iterator it;
		// Traverse the DB to count the support of each item.
		for (int i = 0; i < db.size(); i++) {
			for (int j = 0; j < db[i].size(); j++) {
				it = supportMap.find(db[i][j]);
				if (it == supportMap.end()) {
					supportMap.insert(pair<int, int>(db[i][j], 1));
				}
				else {
					it->second++;
				}
			}
		}
		sortByFrequency(db, supportMap, option);
		/*for (int i = 0; i < db.size(); i++) {
			sortByOption(db, supportMap, 0, db[i].size() - 1, option, i);
			cout << i << endl;
		}*/
	}
	
	// Frequent Itemsets Generation.
	cout << "Generating frequent itemsets ......" << endl;
	vector<int> root;
	frequentItemsetsGeneration(root, db);
	now = clock();
	cout << "Time: "<< (float)(now - past)  / CLOCKS_PER_SEC << endl;
	cout << "Number: " << frequentItemsets.size() << endl;
	past = now;

	// Rules Generation.
	cout << "Generating rules ......" << endl;
	totalRules = 0;
	ofstream output;
	output.open(outputfile);
	for (int i = 0; i < frequentItemsets.size(); i++) {
		vector<vector<int>> consequents;
		for (int j = 0; j < frequentItemsets[i].size(); j++) {
			vector<int> item1;
			item1.push_back(frequentItemsets[i][j]);
			consequents.push_back(item1);
			//cout << frequentItemsets[i][j] << ' ';
		}
		//cout << endl;
		ruleGeneration(frequentItemsets[i], consequents, db, frequentItemsetsSupport[i], output);
	}
	now = clock();
	cout << "Time: " << (float)(now - past) / CLOCKS_PER_SEC << endl;
	cout << "Number: " << totalRules << endl;
	dataSet.close();
	output.close();
}