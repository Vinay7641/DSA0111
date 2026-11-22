#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class Bill {
protected:
    string patient;
public:
    explicit Bill(string p) : patient(move(p)) {}
    virtual ~Bill() = default;
    virtual double total() const = 0;
    virtual string type() const = 0;
    virtual void print() const {
        cout << type() << " " << patient << ": " << total() << '\n';
    }
};

class OutPatientBill : public Bill {
    double consultation, tests;
public:
    OutPatientBill(string p, double c, double t)
        : Bill(move(p)), consultation(c), tests(t) {
        if (c < 0 || t < 0)
            throw invalid_argument("Negative outpatient charge");
    }
    double total() const override { return consultation + tests; }
    string type() const override { return "OP"; }
};

class InPatientBill : public Bill {
    int days;
    double rate, procedures, medicines;
public:
    InPatientBill(string p, int d, double r, double pr, double m)
        : Bill(move(p)), days(d), rate(r), procedures(pr), medicines(m) {
        if (d < 0 || r < 0 || pr < 0 || m < 0)
            throw invalid_argument("Negative inpatient charge");
    }
    double total() const override {
        return days * rate + procedures + medicines;
    }
    string type() const override { return "IP"; }
};

class DayCareBill : public Bill {
    double packagePrice;
public:
    DayCareBill(string p, double v)
        : Bill(move(p)), packagePrice(v) {
        if (v < 0)
            throw invalid_argument("Negative day-care charge");
    }
    double total() const override { return packagePrice; }
    string type() const override { return "DayCare"; }
};

class EmergencyBill : public Bill {
    double base, hourly;
    int hours;
public:
    EmergencyBill(string p, double b, double h, int n)
        : Bill(move(p)), base(b), hourly(h), hours(n) {
        if (b < 0 || h < 0 || n < 0)
            throw invalid_argument("Negative emergency charge");
    }
    double total() const override {
        return base + hourly * hours;
    }
    string type() const override { return "Emergency"; }
};

class Payer {
public:
    virtual ~Payer() = default;
    virtual string name() const = 0;
    virtual double amountCovered(const Bill&) const = 0;
};

class SelfPay : public Payer {
public:
    string name() const override { return "SelfPay"; }
    double amountCovered(const Bill& b) const override {
        return b.total();
    }
};

class Insurance : public Payer {
    double limit, copay;
public:
    Insurance(double l, double c) : limit(l), copay(c) {
        if (l < 0 || c < 0 || c > 100)
            throw invalid_argument("Invalid policy");
    }
    string name() const override { return "Insurance"; }
    double amountCovered(const Bill& b) const override {
        return min(limit, b.total() * (1.0 - copay / 100.0));
    }
};

class GovernmentScheme : public Payer {
    unordered_map<string, double> rates{
        {"OP",1500}, {"IP",45000},
        {"DayCare",20000}, {"Emergency",10000}
    };
public:
    string name() const override { return "GovernmentScheme"; }
    double amountCovered(const Bill& b) const override {
        auto it = rates.find(b.type());
        if (it == rates.end())
            throw runtime_error("Unknown package");
        return min(b.total(), it->second);
    }
};

class CorporatePayer : public Payer {
    double percent;
public:
    explicit CorporatePayer(double p) : percent(p) {
        if (p < 0 || p > 100)
            throw invalid_argument("Invalid corporate coverage");
    }
    string name() const override { return "CorporatePayer"; }
    double amountCovered(const Bill& b) const override {
        return b.total() * percent / 100.0;
    }
};

int main() {
    vector<unique_ptr<Bill>> bills;

    bills.push_back(make_unique<OutPatientBill>("Ravi", 500, 1200));
    bills.push_back(make_unique<InPatientBill>("Meena", 3, 2500, 40000, 0));
    bills.push_back(make_unique<DayCareBill>("Arun", 25000));
    bills.push_back(make_unique<EmergencyBill>("Sara", 2000, 500, 4));

    vector<unique_ptr<Payer>> payers;

    payers.push_back(make_unique<SelfPay>());
    payers.push_back(make_unique<Insurance>(50000, 20));
    payers.push_back(make_unique<GovernmentScheme>());
    payers.push_back(make_unique<CorporatePayer>(80));

    for (const auto& b : bills) {
        b->print();

        for (const auto& p : payers) {
            cout << "  " << p->name()
                 << " covers " << p->amountCovered(*b) << '\n';
        }
    }
}
