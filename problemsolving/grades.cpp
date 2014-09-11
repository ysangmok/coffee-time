#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#define GRADES 5;

using namespace std;

string getGrade(int score);

struct student {
    string firstName, lastName;
    vector<int> scores;
    int average;
};

bool compare(const student &a, const student &b) {
    return a.average > b.average;
}

int main() {
    vector<student> students;
    ifstream infile ("grades.txt");
    string scoreStr, line;
    int average, currentScore;
    while (getline(infile, line)) {
        istringstream ss(line);
        student currentStudent; 
        ss >> currentStudent.firstName;
        ss >> currentStudent.lastName >> currentStudent.lastName;
        average = 0;
        while (ss >> scoreStr) {
            currentScore = atoi(scoreStr.c_str());
            currentStudent.scores.push_back(currentScore);
            average += currentScore;
        }
        currentStudent.average = average / GRADES;
        sort(currentStudent.scores.begin(), currentStudent.scores.end());
        students.push_back(currentStudent);
    }
    sort(students.begin(), students.end(), compare);
    for (int i = 0; i < students.size(); i++) {
        cout << students[i].firstName << " " << students[i].lastName;
        cout << " (" << students[i].average << "%)" << " (" << getGrade(students[i].average) << ")";
        for (int j = 0; j < students[i].scores.size(); j++) {
            cout << " " << students[i].scores[j];
        }
        cout << endl;
    }
    return 0;
}

string getGrade(int score) {
    string tb;
    int remain;
    tb = "";
    remain = score % 10;
    if (remain >= 7) tb = "+";
    else if (remain <= 2) tb = "-";
    if (score >= 90)
        return "A" + tb;
    else if (score >= 80)
        return "B" + tb;
    else if (score >= 70)
        return "C" + tb;
    else if (score >= 60)
        return "D" + tb;
    else
        return "F" + tb;
}