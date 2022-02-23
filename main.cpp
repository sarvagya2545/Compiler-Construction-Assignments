#include <iostream> 
#include<bits/stdc++.h>
#include <fstream>
using namespace std;

// structure of token
struct token{
	int token_no;
	string lexeme;
	int lineC;
};

static token newtk(int tk_num, string lex, int line){
	token tok;
	tok.token_no = tk_num;
	tok.lexeme = lex;
	tok.lineC = line;
	return tok;
}

vector<token> token_list;
vector<string> Keywords = {"main", "int", "float", "string", "bool", "true", "false", "print", "for", "while", "var", "if", "elsif", "else", "return"};

// stray character error
void throw_error(char ch, int lineC){
	cout<<"Lexical error: stray "<< "'" << ch << "'" << " in program at line number = "<< lineC <<endl;
}

// stray lexeme error
void throw_error(string lex, int *index, int lineC){
	cout<<"Lexical error: stray "<< "'" << lex << "'" << " in program at line number = "<< lineC <<endl;
    (*index)++;
}

// ignore comments while reading input file
void Skip_Comments(string line, int *index, int lineC){
	(*index)++;
	if(line[*index] == '%'){
		// TODO 
		while(line[*index]) (*index)++;
	}
	else{
		(*index)--;
		throw_error("%", index, lineC);
		return;
	}
}

void Arithmetic_Op(string line, int *index, int lineC){
	switch(line[*index]){
		case '+': token_list.push_back(newtk(300, "+", lineC)); break;
		case '-': token_list.push_back(newtk(301, "-", lineC)); break;
		case '*': token_list.push_back(newtk(302, "*", lineC)); break;
		case '/': token_list.push_back(newtk(303, "/", lineC)); break;
	}
	(*index)++;
}

// dfa for relational operators
void Relational_Op(string line,int *index,int lineC){
	switch(line[*index]){
		case '<':
			(*index)++;
			if(line[*index]=='='){
				token_list.push_back(newtk(600,"<=", lineC));
			}
			else{
				(*index)--;
				token_list.push_back(newtk(601,"<", lineC));
			}
			break;

		case '>':
			(*index)++;
			if(line[*index]=='='){
				token_list.push_back(newtk(602,">=",lineC));
			}
			else{
				(*index)--;
				token_list.push_back(newtk(603,">",lineC));
			}
			break;

		case '=':
			(*index)++;
		 	if(line[*index]=='='){
		   		token_list.push_back(newtk(604, "==", lineC));
			}
         	else if(line[*index]==' '){
         		throw_error("=",index,lineC);
         		return;
        	}
         	else{
         		(*index)--;
         		throw_error("=",index,lineC);
         		return;
         	}
			break;

	}
	(*index)++;
}

//dfa for assignment op
void Assignment_Op(string line, int* index, int lineC){
	(*index)++;
	if(line[*index] == '=') token_list.push_back(newtk(700, ":=", lineC));
	
    else if(line[*index] == ' '){
		throw_error(":", index, lineC);
	    return;
	}

	else{
		(*index)--;
		throw_error(":", index, lineC);
		return;
	}
	(*index)++;
}


// scanning delimiters
void Delimiter(string line, int* index, int lineC){
	switch(line[*index]){
		case '{': token_list.push_back(newtk(400, "{", lineC)); break;
		case '}': token_list.push_back(newtk(401, "}", lineC)); break;
		case '(': token_list.push_back(newtk(402, "(", lineC)); break;
		case ')': token_list.push_back(newtk(403, ")", lineC)); break;
		case ',': token_list.push_back(newtk(404, ",", lineC)); break;
		case '[': token_list.push_back(newtk(405, "[", lineC)); break;
		case ']': token_list.push_back(newtk(406, "]", lineC)); break;
		// case ';': token_list.push_back(newtk(200, ";", lineC)); break;
		// case ';': token_list.push_back(newtk(200, ";", lineC)); index++; break;

	}
	(*index)++;
}

// DFA for scanning numbers
void Number_token(string line, int* index, int lineC){
	int state = 0; //start state
	string lexeme = "";
	int neg = 0;

	if(line[*index] == '-'){
		lexeme += line[*index];
		(*index)++;
		neg = 1;
	}

	if(line[*index] < '0' || line[*index] > '9'){
		if(neg){
			(*index)--;
			Arithmetic_Op(line, index, lineC);
		}
		else{
			(*index)--;
			state = 5;
			throw_error(lexeme, index, lineC);
		}
		return;
	}

	if(state == 0){
        if(line[*index] >= '0' && line[*index] <= '9'){
            state = 2;
            lexeme += line[*index];
            (*index)++;
        }

        else if(line[*index] < '0' || line[*index] > '9'){
            state = 5;
            throw_error(lexeme, index, lineC);
            (*index)--;
            return;
        }
    }

    if(state == 1){
    	if(line[*index] == '.'){
    		state = 3;
    		lexeme += line[*index];
    		(*index)++;
    	}
    	else token_list.push_back(newtk(101, lexeme, lineC));
    }

    if(state == 2){
    	while(line[*index] >= '0' && line[*index] <= '9'){
    		lexeme += line[*index];
    		(*index)++;
    	}
    	if(line[*index] == '.'){
    		state = 3;
    		lexeme += line[*index];
    		(*index)++;
    	}
    	else
            token_list.push_back(newtk(100, lexeme, lineC));
    }

    if(state==3){
    	if(line[*index] < '0' || line[*index] > '9'){
    		state = 5;
    		throw_error(lexeme, index, lineC);
    		(*index)--;
    		return;
    	}
    	else{
    		state = 4;
    		lexeme += line[*index];
    		(*index)++;
    	}
    }

    if(state == 4){
    	while(line[*index] >= '0' && line[*index] <= '9'){
            lexeme += line[*index];
    		(*index)++;
    	}
    	token_list.push_back(newtk(101, lexeme, lineC));
    }
}

// dfa for scanning named entities
void Scan_Identifiers(string line, int* index, int lineC){
	string lexeme = "";
	while((line[*index]) && ((line[*index] >= 'a' && line[*index] <= 'z') || (line[*index] >= '0' && line[*index] <= '9') || (line[*index] >= 'A' && line[*index] <= 'Z'))){
        lexeme += line[*index];
		(*index)++;
	}

	int flg = 0;
	for(int i = 0; i < Keywords.size(); i++){
		if(lexeme == Keywords[i]){
			token_list.push_back(newtk(501 + i, lexeme, lineC));
			flg = 1;
			break;
		}
	}
	if(flg == 0) token_list.push_back(newtk(500, lexeme, lineC));
}

int main(){
	ifstream file_in; // input file stream
	ofstream file_out; // output file stream

	file_in.open("C:/Users/iamth/Documents/GitHub/Compiler-Construction-Assignments/TestCases/tc_3.txt");
    int lineC = 0; // line number count
	string line; 
	file_out.open("C:/Users/iamth/Documents/GitHub/Compiler-Construction-Assignments/TestCases/tc_3_op.txt");

	while(getline(file_in, line)){
		lineC++; 
		int index = 0;

		while(line[index]){
			if((line[index] >= '0' && line[index] <= '9') || (line[index] == '-')) Number_token(line, &index, lineC); // scan number literals
			else if((line[index] >= 'A' && line[index] <= 'Z') || (line[index] >= 'a' && line[index] <= 'z')) Scan_Identifiers(line, &index, lineC);
		
			else{
				switch(line[index]){
					// TODO - && || implementation
					case '%': Skip_Comments(line, &index, lineC); break; // ignoring comments
					case '\t':
					case ' ': index++; break; // ignore tabs and whitespaces
					case '*':
					case '/': 
					case '+':
					case '-': Arithmetic_Op(line, &index, lineC); break; // scan arithmetic operators
					case '<':
					case '>':
					case '=': Relational_Op(line, &index, lineC); break; // scan relational operators
                    case '(':
					case ')':
                    case '[':
					case ']':
					case '{':
					case '}':
					case ',': Delimiter(line, &index, lineC); break;	// got a delimiter
					case ';': token_list.push_back(newtk(200, ";", lineC)); index++;  break;
					case ':': Assignment_Op(line, &index, lineC); break; // scan for assignment operator

					default : throw_error(line[index], lineC); index++; break;
				}
			}
		}

	}

	// Writing the token list on o/p file
	for(token tok : token_list) file_out << "Token " << tok.token_no << ", string \"" << tok.lexeme << "\", line number " << tok.lineC << "\n";

    file_in.close();
    file_out.close();

    return 0;
}
