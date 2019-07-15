#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <direct.h>
#include <dirent.h>
#include <sys/stat.h>
#include "xxtea.h"
#include "license_generator.h"
#include "json.hpp"
using namespace std;
using json=nlohmann::json;

Generator::Generator(string key, string version): _key(key), _version(version){}

string Generator::get_key() { return _key; }

string Generator::get_version() { return _version; }

pair<json, json> Generator::get_variables(string func_file) {
    ifstream in(func_file);
    json tmp_j1, tmp_j2;
    in >> tmp_j1 >> tmp_j2;
    in.close();
    return make_pair(tmp_j1, tmp_j2);
}

static vector<string> get_all_user_file(string dir_path) {
    DIR *dir;
    struct dirent *ent;
    vector<string> v;
    dir_path += "\\";
    assert((dir = opendir(dir_path.c_str())) != NULL);
    
    while ((ent = readdir(dir)) != NULL) {
        string tmp = string(ent->d_name);
        if (tmp != "." && tmp != "..") {
            v.push_back(tmp);
        }
    }
    closedir(dir);
    return v;
}

string Generator::my_encryption(json j) {
    XXTEA en;
    return en.xxtea_encrypt(j.dump(), get_key());
}

string Generator::my_decryption(string str) {
    XXTEA en;
    return en.xxtea_decrypt(str, get_key());
}

void Generator::update(json &j, string decryp, string func_dir, string file) {
    string user = file.substr(0, file.length() - 4);
    string func_file = func_dir + "\\" + user + ".json";

    struct stat buf;
    if (stat(func_file.c_str(), &buf) == -1)
        throw 1013;

    auto p = get_variables(func_file);
    json tmp = json::parse(decryp);
    tmp["service_status"] = p.first;
    tmp["expiration"] = p.second;
    j[user] = tmp;
}

void Generator::set_version(string &res) { 
    res += "version=" + get_version(); 
}

static bool IsRightFormat(string strData) {
    if (strData[0] != '{')
        return false;
 
    int num = 0;
    for (int i = 0; i < strData.length(); ++i) {
        if (strData[i] == '{')
            ++num;
        else if (strData[i] == '}')
            --num;
    }
    
    if (!num) return true;
    return false;
}

static void write_file(string res) {
    mkdir(".\\my_license"); 
    ofstream out("my_license\\license.txt");
    out << res;
    out.close();
}

int Generator::generate(string source_dir, string func_dir) {
    vector<string> v = get_all_user_file(source_dir);
    ifstream in;
    string encryp, decryp;
    json j;
    for (auto &file: v) {
    	in.open(source_dir + "\\" + file);
        if (!in) return 1011;

        getline(in, encryp);
        decryp = my_decryption(encryp);
        if (!IsRightFormat(decryp)) return 1012;

        try {
            update(j, decryp, func_dir, file);
        }
        catch (int error_code) {
            return error_code;
        }
        in.close();
    }
#ifdef DEBUG
    ofstream out("result.json");
    out << j.dump(4);
    out.close();
#endif
    string res = my_encryption(j);
    set_version(res);
    write_file(res);
    return 0;
}
