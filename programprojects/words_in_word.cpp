#include <iostream>
#include <string>
#include <fstream>
#include <map>

using namespace std;
int main(int argc, char *argv[]) {
	ifstream myfile("enable1.txt");
	string word, str1, temp, curmax;
	map<string, int> wordlist;
	map<string, int>::iterator iter;
	
	while(myfile >> word){
		wordlist.insert(make_pair(word, 1));
	}
	
	int a,b,curmaxint = 0;

	for(iter = wordlist.begin(); iter != wordlist.end(); iter++){
		temp = iter->first;
		for(a = 0; a < temp.size()-1; a++){
			for(b = 2; b < temp.size()-1-a; b++){
				str1 = temp.substr(a,b);
				if(wordlist.find(str1) != wordlist.end()){
					iter->second ++;
					if(temp == "forethoughtfulnesses"){
						cout << str1 + " "<< a << " "<< b  << "\n";
					}
				}
			}
		}
		if(iter->second > curmaxint){
			curmax = iter->first;
			curmaxint = iter->second;
		}
	}
	
	cout << curmax << "\n" << curmaxint;
	return 0;
}