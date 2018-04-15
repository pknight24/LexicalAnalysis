#include "pa3.h"
#include <iostream>
#include <string>
#include <iterator>
#include <fstream>

template <class T> Node<T>::Node(T data)
{
    this->data = data;
    this->next = NULL;
}

template <class T> Stack<T>::Stack()
{
   this->top = NULL; 
   this->length = 0;
}

template <class T> void  Stack<T>::push(T data)
{
    Node<T>* temp = this->top;
    this->top = new Node<T>(data);
    this->top->next = temp;
    this->length++;
}

template <class T> void Stack<T>::pop()
{
    
    Node<T>* temp = this->top->next;
    delete this->top;
    this->top = temp;
    this->length--;
}

template <class T> T Stack<T>::peak()
{
    return this->top->data;
}

template <class T> bool Stack<T>::isEmpty()
{
    return this->top == NULL;
}

template <class T> void Stack<T>::clear()
{
    while(!this->isEmpty())
    {
        this->pop();   
    }
}

template <class T> int Stack<T>::getLength()
{
    return this->length;
}

template <class T> Stack<T>::~Stack()
{
    this->clear();
}

template <class T> void Stack<T>::print()
{
    std::cout << "stack length = " << this->length << std::endl;
    if (!this->isEmpty())
    {
        Node<T>* temp = this->top;
        for (int i = 0; i <this->length; i++)
        {
            std::cout << temp->data << std::endl;
            temp = temp->next;
        } 
    }
}

Compiler::Compiler(std::string text)
{
    this->maxNestedLoop = 0;
    this->curMax = 0;
    this->text = text;
    this->state = new Stack<std::string>();
    this->current = this->text.begin();
}

Compiler::~Compiler()
{
    delete this->state;
}

std::string Compiler::parseKeyword()
{
    std::string possibleKeyword = "";
    int distance = 0;
    while (isupper(*current) && current != this->text.end())//moves the iterator through uppercase letters
    {
        possibleKeyword += *current;
        current++;
        distance++;
    }

    if (!checkParse(possibleKeyword))//moves the iterator back to where it began if the parse was unsuccesful
    {
        for (int i = 0; i < distance; i++)
        {
            current--;
        }
    }

    return possibleKeyword;
}

std::string Compiler::parseIdentifier()//same as parseKeyword
{
    std::string possibleIdentifier = "";
    int distance = 0;
    while (islower(*current) && current != this->text.end())
    {
        possibleIdentifier += *current;
        current++;
        distance++;
    }
    
    if (!checkParse(possibleIdentifier))
    {
        for (int i = 0; i < distance; i++)
        {
            current--;
        }
    }

    return possibleIdentifier;

}

std::string Compiler::parseConstant()//same as above parsers, but searches for digits
{
    std::string possibleConstant = "";
    int distance = 0;
    while (isdigit(*current) && current != this->text.end())
    {
        possibleConstant += *current;
        current++;
        distance++;
    }
    
    if (!checkParse(possibleConstant))
    {
        for (int i = 0; i < distance; i++)
        {
            current--;
        }
    }

    return possibleConstant;

}

std::string Compiler::parseDelimiter()//looks for either a comma or semicolon (only possible delimiters)
{
    std::string possibleDel = "";
    if (*current == ';'|| *current == ',')
    {
        possibleDel += *current;
        current++;
    }

    return possibleDel;
}

std::string Compiler::parseOp()//parsers for an operator
{
    std::string possibleOp = "";
    char c = *current;
    if (c == '+' || c == '-' || c == '/' || c == '*' || c == '=')
    {
        possibleOp += c;
        current++;
        if (c == '+' && *current == '+')//checks for the '++' operator
        {
            possibleOp += c;
            current++;
        }
    }

    return possibleOp;
}

std::string Compiler::parseParen() //looks for any parenthesis
{
    std::string possibleParen = "";
    if (*current == ')' || *current == '(')
    {
        possibleParen += *current;
        current++;
    }

    return possibleParen;
}

bool Compiler::checkParse(std::string result) //checks to see if the result of a parse was empty or not
{
    return !result.empty();
}

void Compiler::whitespace()
{
    while(iswspace(*current)) //moves the iterator through any whitespace
    {
        current++;
    }
}

void Compiler::bigParse() //tries each parser until one of them works
{
    std::string keyword = this->parseKeyword();
    if (checkParse(keyword))
    {
        if (keyword == "BEGIN" || keyword == "END" || keyword == "FOR") //makes sure that the keyword is one of the correct ones
        {
            this->state->push(keyword);
            this->safeAppend(&this->keywords, keyword);
            if (keyword == "END")
            {
                if (loopCheck()) curMax++;//when an 'END' is reached, checks to see if the loop is complete
            }
            else if (keyword == "FOR") curMax = 0; //resets the current max when a new loop begins

            if (curMax > maxNestedLoop) maxNestedLoop = curMax;
        } 
        else
        {
            this->syntaxErrors.push_back(keyword);//pushes any misspellings to the errors vector
        }
        return; //ends the method
    }

    std::string identifier = this->parseIdentifier();
    if (checkParse(identifier))
    {
        this->safeAppend(&this->identifiers, identifier);
        return;
    }

    std::string delimiter = this->parseDelimiter();
    if (checkParse(delimiter))
    {
        this->safeAppend(&this->delimiters, delimiter);
        return;
    }

    std::string paren = this->parseParen();
    if (checkParse(paren))
    {
        if (paren =="(") this->state->push(paren);//open parentheses are pushed to the stack
        else if(paren == ")")
        {
            if (this->state->peak() == "(") this->state->pop(); //if the parentheses match, pop from the stack
            else this->state->push(paren);//if not, it will be detected as an error later
        }
        return;
    }

    std::string op = this->parseOp();
    if (checkParse(op))
    {
        this->safeAppend(&this->operators, op);
        return;
    }

    std::string constant = this->parseConstant();
    if(checkParse(constant))
    {
        this->safeAppend(&this->constants, constant);
        return;
    }
}

void Compiler::parse()
{
    while(current != this->text.end()) //creates a loop that parses through the entire file while ignoring whitespace
    {
        this->bigParse();
        this->whitespace();
    }
}

void Compiler::print()
{
    std::cout << "\nThe depth of nested loop(s) is " << this->maxNestedLoop << std::endl;
    std::cout << "Keywords: ";
    for (std::string &s : this->keywords)
    {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Identifiers: ";
    for (std::string &s : this->identifiers)
    {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    std::cout << "Constants: ";
    for (std::string &s : this->constants)
    {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Operators: ";
    for (std::string &s : this->operators)
    {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Delimiters: ";
    for (std::string &s : this->delimiters)
    {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    std::cout << "Syntax Error(s): ";
    if (this->syntaxErrors.empty()) std::cout << "NA";
    else
    {
        for (std::string &s : this->syntaxErrors)
        {
            std::cout << s << " ";
        }
    }
    
    std::cout << std::endl;


}

void Compiler::safeAppend(std::vector<std::string>* list, std::string s)
{
    bool present = false;
    for (std::string &in : *list)
    {
        if (in == s) present = true;
    }

    if (!present) list->push_back(s);
}

bool Compiler::loopCheck()//a loop is only complete when it contains FOR BEGIN END without ANY syntax errors, including parentheses
{
    if (this->state->isEmpty()) return false; 
    this->state->pop(); //pops the END from the stack (this method is only called when an END keyword is reached)
    if (!this->state->isEmpty() && this->state->peak() == "BEGIN")
    {
        this->state->pop(); //pops the BEGIN
        if (this->state->getLength() > 0 && this->state->peak() == "FOR") //makes sure that the next item in the stack is FOR
        {
            this->state->pop();
            return true;
        }
        else 
        {
            while (!this->state->isEmpty() && this->state->peak() != "FOR") 
            {
              this->syntaxErrors.push_back(this->state->peak());//adds unmatched parentheses to the errors vector
              this->state->pop();

            }
            if (!this->state->isEmpty()) this->state->pop(); //clears the rest of the improper loop from the stack so that it doesn't interfer with any later loops
            return false;
        }

    }
    else if(this->state->peak() == "FOR") //this checks for a missing BEGIN 
    {
        this->state->pop();
        return false;
    }
    return false;
    
}

void Compiler::stateCheck()
{
    if (this->state->isEmpty()) return; //this means that all loops were done correctly
    if (this->state->peak() == "BEGIN") //checks for missing END
    {
      this->syntaxErrors.push_back("END");
      this->state->pop();
      while (this->state->getLength() > 0 && this->state->peak() != "FOR") //checks for any leftover parentheses that were not balanced
      {
          this->syntaxErrors.push_back(this->state->peak());
          this->state->pop();
      }
    }
    else if (this->state->peak() == "FOR") //checks for missing BEGIN and END
    {
        this->syntaxErrors.push_back("BEGIN");
        this->syntaxErrors.push_back("END");
    }
    else
    {
        while(!this->state->isEmpty()) //handles the odd case where no loops was created and parentheses are left over
        {
            this->syntaxErrors.push_back(this->state->peak());
            this->state->pop();
        }
    }
}

void Compiler::printState() //used for debugging
{
    std::cout << "Print State" << std::endl;
    this->state->print();
    std::cout << "\ndone printing\n";
}

std::string getInput() //gets the input file while checking for errors
{
    std::cout << "Please enter the name of the input file: \n";

    std::string fileName;
    std::cin >> fileName;
    std::ifstream source(fileName);

    std::string fileContents;

    if (source.is_open())
    {
        std::string str;
        while (std::getline(source, str))
        {
            fileContents += str;
            fileContents += '\n';    
        }
        return fileContents;
    }
    else
    {
        std::cout << "Error: unable to open file. Please try again.\n\n";
        return getInput();
    }

}

int main()
{
    std::string fileContents = getInput(); 
    Compiler c(fileContents);
    c.parse();
    c.stateCheck();
    c.print();
    return 0;
}
