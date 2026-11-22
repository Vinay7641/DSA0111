#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
using namespace std;

struct Resource {
    string name;

    explicit Resource(string n) : name(move(n)) {
        cout << "created " << name << '\n';
    }

    ~Resource() {
        cout << "destroyed " << name << '\n';
    }
};

double readAmount(const string& s) {
    size_t used = 0;
    double x = stod(s, &used);

    if (used != s.size())
        throw invalid_argument("invalid numeric text");

    return x;
}

void checkCharge(double x) {
    if (x < 0)
        throw invalid_argument("negative charge");
}

void checkClaim(double billed, double cover) {
    if (billed > cover)
        throw runtime_error("claim exceeds cover");
}

double packageRate(const string& code) {
    if (code == "OP") return 1500;
    if (code == "IP") return 45000;
    if (code == "DAY") return 20000;

    throw runtime_error("unknown package code");
}

void allocationTest() {
    int* p = new int[numeric_limits<size_t>::max()];
    delete[] p;
}

int main() {
    try {
        Resource session("billing-session");

        try {
            checkCharge(-10);
        }
        catch (const exception& e) {
            cout << "negative test: " << e.what() << '\n';
        }

        try {
            checkClaim(60000, 50000);
        }
        catch (const exception& e) {
            cout << "claim test: " << e.what() << '\n';
        }

        try {
            cout << packageRate("UNKNOWN") << '\n';
        }
        catch (const exception& e) {
            cout << "package test: " << e.what() << '\n';
        }

        try {
            cout << readAmount("abc") << '\n';
        }
        catch (const exception& e) {
            cout << "number test: " << e.what() << '\n';
        }

        try {
            allocationTest();
        }
        catch (const bad_alloc&) {
            cout << "allocation test: bad allocation caught\n";
        }
        catch (const exception& e) {
            cout << "allocation test: " << e.what() << '\n';
        }

        throw runtime_error("top-level recovery test");
    }
    catch (const exception& e) {
        cout << "top-level caught: " << e.what() << '\n';
    }
    catch (...) {
        cout << "unknown error caught\n";
    }

    cout << "program continues safely\n";
}
