#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;
int main(int argc, char *argv[]) {
	string mystr, hello;
	stringstream ss;
	int n;
	vector<string> sh;
	cout << "number of lines? \n";
	getline(cin, mystr);	
	stringstream(mystr) >> n;
	
	ifstream myfile ("input.txt");
	cout << myfile.is_open();
	if (myfile.is_open()){
		for(int i = 0; i < n; i++){
				getline(myfile, mystr);
		}
	}
	else{
		return -1;
	}
	
	while(stringstream(mystr)){
		hello << stringstream(mystr);
		cout << hello;
	}
	
	return 0;
}