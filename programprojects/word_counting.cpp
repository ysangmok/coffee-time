#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <iomanip> 
#include <algorithm>
#include <cctype>

using namespace std;

int main(int argc, char *argv[]) {
	ifstream file("bird.txt");
	if(!file.good()){
		cout << "file bird.txt was not found\n";
	}
	map<string, int> mylist;
	map<string, int>::iterator itr;
	string word, temp, result;
	string::iterator stitr;
	while(file >> temp){
		for(stitr = temp.begin(); stitr != temp.end();){
			if(ispunct(*stitr) || isnumber(*stitr))
				stitr = temp.erase(stitr);
			else
				++stitr;
		}
		
		transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
		
		itr = mylist.find(temp);
		
			if(itr != mylist.end()){
				itr->second++;
			}
			else{
				mylist.insert(make_pair(temp,1));
			}
	}
	itr = mylist.begin();
	while(itr != mylist.end()){
		cout << setw(20) << itr->first << left << setw(10) << itr->second << "\n";
		itr++;
	}
}