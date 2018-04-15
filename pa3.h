#include <iostream>
#include <string>
#include <iterator>
#include <vector>

#ifndef NODE
#define NODE
template <class T> class Node //template Node class for the Stack data structure
{
  public:
    Node(T data);
    Node<T> *next;
    T data;

};
#endif

#ifndef STACK
#define STACK
template <class T> class Stack //implementation of a Stack that can used with any necessary data type
{
    Node<T>* top;
    int length;
  public:
    Stack();
    void clear();
    ~Stack();
    void push(T data);
    void pop();
    T peak();
    bool isEmpty();
    void print();
    int getLength();
};
#endif

#ifndef COMPILER
#define COMPILER
class Compiler //Compiler class that handles parsing the input text
{
    Stack<std::string>* state; //Stack that keeps track of keywords and parentheses
    std::vector<std::string> syntaxErrors; //holds all syntax errors
    std::vector<std::string> keywords; //holds all keywords
    std::vector<std::string> delimiters; //holds delimiters
    std::vector<std::string> operators; //holds all operators
    std::vector<std::string> identifiers; //holds all identifiers
    std::vector<std::string> constants; //holds all constants
    std::string text; //this is the input text that is analyzed
    std::string::iterator current; //iterator that keeps track of the parser's status
    std::string parseKeyword(); //parses for a keyword
    std::string parseIdentifier(); //parses for identifier
    std::string parseConstant(); //parses for a constant
    std::string parseDelimiter(); //parses for delimiter
    std::string parseOp(); //parses for operator
    std::string parseParen(); //parses for a parentheses
    bool checkParse(std::string result); //checks is a parser was succesful
    bool loopCheck(); //checks for a complete loop without any syntax errors
    void whitespace(); //moves the 'current' iterator past any whitespace (effectively ignoring it)
    void bigParse(); //handles all of the parsers
    void safeAppend(std::vector<std::string>* list, std::string s); //appends a string to a vector without repeats
    int maxNestedLoop; //keeps track of the maximum nested loop
    int curMax; //keeps track of the CURRENT maximmum loop, this is compared to the maxNestedLoop variable
  public:
    Compiler(std::string text);
    ~Compiler();
    void parse();
    void print(); //prints all of the information about the program's state
    void printState(); //prints the 'state' stack, this is used primarily for debugging
    void stateCheck(); //checks the state for missing END or BEGIN keywords
};
#endif
