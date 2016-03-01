#include "Declaration.h"
#include "API.h"
#include "BufferManager.h"
int main() {
	API api;
	string statement;
	//test file
	api.Exec_File("SQL//test100000.txt");
	//cout << "Successful!!!!" << endl;

	while (!api.Check_Finish()) {
		getline(cin, api.inputline);
		api.dealString();
	}


}
