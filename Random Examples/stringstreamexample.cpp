#include <iostream> // std::cin & std::cout
#include <sstream>  // std::istringstream
#include <string>   // std::string & std::getline
#include <stack>    // std::stack

int main()
{
  std::string line, word;
  std::stack<std::string> words;// std::stack<T> is hwo 

  std::getline(std::cin, line); // Get a full line of text

  std::istringstream iss(line); // Put the line in a custom stream
  while ( iss.good() )          // while the stream is still valid
  {
    iss >> word;                // extract another word
    words.push(word);           // puts it on the top of the stack
  }

  while ( !words.empty() )           // self-explanitory
  {
    std::cout << words.top() << ' '; // get the top word
    words.pop();                     // removes top word
  }
}