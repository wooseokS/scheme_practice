#include<iostream>
#include<string>
#include<istream>

#define MEMORY_LENGTH 90
#define HASH_LENGTH 997

using namespace std;


class Memory {
public:
	int index;
	int rchild;
	int lchild;
	Memory() {
		index = NULL;
		rchild = 0;
		lchild = 0;
	}
	Memory(int index) {
		this->index = index;
		rchild = 0;
		lchild = 0;
	}
	int getIndex() {
		return index;
	}
	int getrchild() {
		return rchild;
	}
	int getlchild() {
		return lchild;
	}
};
Memory* memory_array[MEMORY_LENGTH];
string input;
class Hash {
public:
	int index;
	string symbol;
	int pointer;
	Hash() {
		index = NULL;
		symbol = "";
		pointer = NULL;
	}
	Hash(int index, string symbol) {
		this->index = index;
		this->symbol = symbol;
	}
};
Hash hash_array[HASH_LENGTH];
int freelist = 1;

string doLower(string word) {
	string result = "";
	int length = word.length();
	for (int i = 0; i < length; i++) {
		if (word.at(0) <= 'Z' && word.at(0) >= 'A') {
			result.push_back((char)((int)word.at(0) + 'z' - 'Z'));
		}
		else {
			result.push_back(word.at(0));
		}
		word.erase(0, 1);
	}
	return result;
}

int hashCal(string word) { //calculate hash value
	int hash_value = 0;
	while (word.length() > 0) {
		hash_value += (int)word.at(0);
		word.erase(0, 1);
	}
	hash_value %= HASH_LENGTH;
	return hash_value;
}

string getVal(int hashvalue) {  // hashvalue > 0
	return hash_array[abs(hashvalue)].symbol;
}

int getHashValue(string word) { //return hash value > 0
	int hash_value = hashCal(word);
	if (hash_array[hash_value].symbol.length() != 0) {
		int i = hash_value;
		while (hash_array[i].symbol.length() != 0) {
			if (hash_array[i].symbol == word) return i;
			i += 1;
			i %= HASH_LENGTH;
		}
		hash_array[i] = Hash(i, word);
		return i;
	}
	else {
		hash_array[hash_value] = Hash(hash_value, word);
		return hash_value;
	}
}

string getNextToken(string &word) { //get by reperence
	string token = "";
	if (word.length() > 0) {
		while (word.at(0) == ' ') {
			word.erase(0, 1);
		}
	}
	else {
		return "";
	}
	switch (word.at(0)) {
	case '\'': {
		token = "'";
		word.erase(0, 1);
		return token;
	}
	case ')': {
		token = ")";
		word.erase(0, 1);
		return token;
	}
	case '(': {
		token = "(";
		word.erase(0, 1);
		return token;
	}
	default: {
		while (word.length() > 0 and word.at(0) != ' ') {
			if (word.at(0) == '(') return token;
			if (word.at(0) == ')') return token;
			token.push_back(word.at(0));
			word.erase(0, 1);
		}
		while (word.length() > 0 and word.at(0) == ' ') {
			word.erase(0, 1);
		}
		return token;
	}
	}
	return token;
}

int alloc() {
	int alloc_index = freelist;
	memory_array[freelist]->lchild = 0;
	freelist = memory_array[freelist]->getrchild();
	return alloc_index;
}

void dealloc(int index) {
	int dealloc_index = index;
	memory_array[index]->rchild = freelist;
	freelist = index;
}

void pushBack(string &word) {
	word = "(" + word;
}

int read() {
	//node array 만들기 & hash table 채우기
	int root = 0;
	bool first = true;
	int token_hash = getHashValue(getNextToken(input));
	if (token_hash == getHashValue("(")) {
		token_hash = getHashValue(getNextToken(input));
		int temp;
		while (token_hash != getHashValue(")")) {
			if (first) {
				temp = alloc();
				root = temp;
				first = false;
			}
			else {
				memory_array[temp]->rchild = alloc();
				temp = memory_array[temp]->rchild;
			}
			if (token_hash == getHashValue("(")) {
				pushBack(input);
				memory_array[temp]->lchild = read();
			}
			else {
				memory_array[temp]->lchild = -token_hash;
			}
			memory_array[temp]->rchild = 0;
			token_hash = getHashValue(getNextToken(input));
		}
		return root;
	}
	else return -token_hash;
}

string concatenate(string body, string token) {
	return body +" "+ token+" ";
}

string preprocessing() {
	string new_command = "";
	string token = getNextToken(input);
	while (token.length() > 0) {
		if (token == "define") {
			new_command = concatenate(new_command, "define");
			token = getNextToken(input);
			if (token == "(") {
				token = getNextToken(input);
				new_command = concatenate(new_command, token);
				new_command = concatenate(new_command, "( lambda (");
				new_command = concatenate(new_command, preprocessing());
				new_command = concatenate(new_command, ")");
			}
		}
		else if (token == "'") {
			new_command = concatenate(new_command, "(quote ");
			int number_of_nextparen = 0;
			do {
				token = getNextToken(input);
				new_command = concatenate(new_command, token);
				if (token == "(") number_of_nextparen += 1;
				else if (token == ")") number_of_nextparen -= 1;
			} while (number_of_nextparen > 0);
			new_command = concatenate(new_command, ")");
		}
		else new_command = concatenate(new_command, token);

		token = getNextToken(input);
	}
	return new_command;
}
bool isnumeric(string st) {
	int len = st.length(), ascii_code, decimal_count = -1, negative_count = -1;
	for (int i = 0; i < len; i++) {
		ascii_code = int(st[i]);
		switch (ascii_code) {
		case 45: // Allow a negative sign.
			negative_count++;
			if (negative_count || i != 0) {
				return false;
			}
			break;
		case 46: // Allow a decimal point.
			decimal_count++;
			if (decimal_count) {
				return false;
			}
			break;
		default:
			if (ascii_code < 48 || ascii_code > 57) {
				return false;
			}
			break;
		}
	}
}

int eval(int node_root) { //token_index > 0 , return : hash < 0 , index > 0
	if (node_root < 0) {
		return node_root;
	}
	int token_index = -memory_array[node_root]->lchild;

	if (token_index == getHashValue("user defined func")) {
		return 0;
	}
	else if (token_index == getHashValue("+")) {
		return -getHashValue(to_string(stof(getVal(eval(memory_array[memory_array[node_root]->rchild]->lchild))) + stof(getVal(eval(memory_array[memory_array[memory_array[node_root]->rchild]->rchild]->lchild)))));
	}
	else if (token_index == getHashValue("-")) {
		return -getHashValue(to_string(stof(getVal(eval(memory_array[memory_array[node_root]->rchild]->lchild))) - stof(getVal(eval(memory_array[memory_array[memory_array[node_root]->rchild]->rchild]->lchild)))));
	}
	else if (token_index == getHashValue("*")) {
		return -getHashValue(to_string(stof(getVal(eval(memory_array[memory_array[node_root]->rchild]->lchild))) * stof(getVal(eval(memory_array[memory_array[memory_array[node_root]->rchild]->rchild]->lchild)))));
	}
	else if (token_index == getHashValue("number?")) {
		if (isnumeric(getVal(eval(memory_array[memory_array[node_root]->rchild]->lchild)))) {
			return -getHashValue("#t");
		}
		else return -getHashValue("#f");
	}
	else if (token_index == getHashValue("symbol?")) {
		if (!isnumeric(getVal(eval(memory_array[memory_array[node_root]->rchild]->lchild))))
			return -getHashValue("#t");
		else return -getHashValue("#f");
	}
	else if (token_index == getHashValue("null?")) {
		if (memory_array[node_root]->rchild == 0 || eval(memory_array[memory_array[node_root]->rchild]->lchild) == 0 || memory_array[node_root]->lchild == 0)
			return -getHashValue("#t");
		else return -getHashValue("#f");
	}
	else if (token_index == getHashValue("cons")) {
		int new_memory = alloc();
		memory_array[new_memory]->lchild = eval(memory_array[memory_array[node_root]->rchild]->lchild);
		memory_array[new_memory]->rchild = eval(memory_array[memory_array[memory_array[node_root]->rchild]->rchild]->lchild);
		return new_memory;
	}
	else if (token_index == getHashValue("cond")) {
		while (memory_array[memory_array[node_root]->rchild]->rchild != 0) {
			node_root = memory_array[node_root]->rchild;
			if (eval(memory_array[memory_array[node_root]->lchild]->lchild) == -getHashValue("#t")) {
				return eval(memory_array[memory_array[memory_array[node_root]->lchild]->rchild]->lchild);
			}
		}
		if (memory_array[memory_array[memory_array[node_root]->rchild]->lchild]->lchild != -getHashValue("else")) {
			cout << "error \n";
			return 0;
		}
		return eval(memory_array[memory_array[memory_array[memory_array[node_root]->rchild]->lchild]->rchild]->lchild);

	}
	else if (token_index == getHashValue("car")) {
		return memory_array[eval(memory_array[memory_array[node_root]->rchild]->lchild)]->lchild;
	}
	else if (token_index == getHashValue("cdr")) {
		return memory_array[eval(memory_array[memory_array[node_root]->rchild]->lchild)]->rchild;
	}
	else if (token_index == getHashValue("define")) {
		if (memory_array[memory_array[memory_array[memory_array[node_root]->rchild]->rchild]->lchild]->lchild == -getHashValue("lambda")) {
			return -getHashValue("wow");
		}
		else {
			return -getHashValue("nope");
		}
	}
	else if (token_index == getHashValue("quote")) {
		return memory_array[memory_array[node_root]->rchild]->lchild;
	}
	else {
		return -token_index;
	}
}

/*
int int_width(int num)
{
	int ret;
	if (num < 0) num = -num;
	for (ret = 0; num > 0; num /= 10, ret++);
	return ret;
}
void outputBlank(int num) {
	for (int i = 0; i < num; i++) {
		cout << " ";
	}
}
void outputMemory(int node_root) {
	
	//node array만 출력하게 하는 함수
	if (node_root <= 0) return;
	cout << memory_array[node_root]->getIndex();
	int blank_value;
	(memory_array[node_root]->getlchild() > 0) ? blank_value = 1 : blank_value = 0;
	outputBlank(blank_value + 8 - int_width(memory_array[node_root]->getIndex()));
	cout << memory_array[node_root]->getlchild();
	outputBlank(12 - int_width(memory_array[node_root]->getlchild()));
	cout << memory_array[node_root]->getrchild() << '\n';
	if (memory_array[node_root]->lchild > 0) outputMemory(memory_array[node_root]->lchild);
	if (memory_array[node_root]->rchild != 0) outputMemory(memory_array[node_root]->rchild);
	
	//memory array 모두 출력
	for (int i = 0; i < MEMORY_LENGTH; i++) {
		cout << memory_array[i]->getIndex();
		int blank_value;
		(memory_array[i]->getlchild() >= 0) ? blank_value = 1 : blank_value = 0;
		outputBlank(blank_value + 8 - int_width(memory_array[i]->getIndex()));
		cout << memory_array[i]->getlchild();
		outputBlank(12 - int_width(memory_array[i]->getlchild()));
		cout << memory_array[i]->getrchild() << '\n';
	}
}
void outputHash() {
	for (int i = 0; i < HASH_LENGTH; i++) {
		if (hash_array[i].length() != 0) {
			cout << i;
			outputBlank(8 - int_width(i));
			cout << hash_array[i] << '\n';
		}
	}
}
*/

void outputString(int node_root, bool leftParam) {
	if (node_root < 0) cout << hash_array[-node_root].symbol;
	else if (node_root == 0) cout << "()";
	else {
		int lchild = memory_array[node_root]->lchild;
		int rchild = memory_array[node_root]->rchild;
		if (leftParam) cout << "(";
		if (lchild < 0) {
			cout << hash_array[-lchild].symbol << " ";
		}
		else {
			outputString(lchild, 1);
		}
		if (rchild == 0) cout << ") ";
		else outputString(rchild, 0);
	}
}
void output(int node_root) {
	cout << '\n';
	outputString(node_root, 1);

	cout << '\n';
}
//proj1의 출력함수

int main() {
	//hash array, memory array(30) making
	hash_array[0] = Hash(0, "");

	for (int i = 0; i < MEMORY_LENGTH; i++) {
		memory_array[i] = new Memory(i);
		memory_array[i]->rchild = i + 1;
	}

	while (1) {
		int read_root = 0;
		getline(cin, input);
		input = doLower(input);

		input = preprocessing();

		read_root = read();

		output(read_root);
		int result = 0;
		result = eval(read_root);
		output(result);
		//printresult(result, true);
	}

	return 0;
}