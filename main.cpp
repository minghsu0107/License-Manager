#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include "get_computer_info.h"
#include "license_generator.h"
#include "license_validation.h"
#include "json.hpp"
using namespace std;
using json=nlohmann::json;

struct var {
	string key;
	string projector_file;
	string version;
	string output_file;
	string email_dir;
	string service_dir;
};

void display(string output_file) {
	ifstream t(output_file);
	stringstream buffer;
	buffer << t.rdbuf();
	json j = json::parse(string(buffer.str()));

	cout.fill('.');
	j = j["service_status"];
	for (auto it = j.begin(); it != j.end(); ++it) {
		cout << left << setw(25) << it.key();
		if (it.value()) cout << "on" << endl;
		else cout << "off" << endl;
	}
}

int main() {
	var v = {"1234567890abcdef", "projector\\id.txt", "01.00", \
		     "output\\out.txt", "email_info", "service_status"};

	Extract e(v.key);
	int emes = e.load(v.projector_file);
	if (emes == 101)
		cout << "fail to access pc information." << endl;
	else if (emes == 102)
		cout << "fail to access projector information" << endl;
	else if (!emes) {
		Generator g(v.key, v.version);
		int gmes = g.generate(v.email_dir, v.service_dir);
		if (!gmes) {
			Validation vv(v.key);
			int vmes = vv.validate(v.projector_file);

			if (!vmes) {
				cout << "validation succeed, welcome!" << endl;
				cout << "activate following functions..." << endl;
				display(v.output_file);
			}
			else if (vmes == 101)
				cout << "fail to access pc information." <<endl;
			else if (vmes == 102)
				cout << "fail to access projector information" << endl;
			else if (vmes == 1001)
				cout << "wrong license key." << endl;
			else if (vmes == 1002)
				cout << "license expired." << endl;
			else if (vmes == 1003)
				cout << "invalid pc." << endl;
		}
		else if (gmes == 1011)
			cout << "fail to open user information file." << endl;
		else if (gmes == 1012)
			cout << "user information is damaged." << endl;
		else if (gmes == 1013)
			cout << "service status file does not exist." << endl;
	}
	return 0;
}