#include <iostream>
#include <string>
#include <sstream>
#include <map>
using namespace std;

int main(int argc, char *argv[]) {
	map<string, string> mymap;
	mymap["a"]= "apple";
	//mymap.insert(pair<string,string>('a','apple'));
	string mystr;
	cout << "type what you want substringed.\n";
	getline(cin, mystr);
	stringstream iss(mystr);
	cout << sizeof(iss);
	while(iss){
		string out;
		iss >> out;
		cout << "substring: " << out << endl;
	}
	cout << mymap["a"] << "\n";
	mymap["b"] = "battle";
	mymap["c"] = "cat";
	mymap["d"] = "dog";
	mymap["e"] = "eeep";
	map<string, string>::iterator it = mymap.begin();
	for(it = mymap.begin(); it!=mymap.end(); it++){
		cout << it->first << " == " << it->second << "\n";
	}
}