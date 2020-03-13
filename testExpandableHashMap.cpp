#include <iostream>
#include <functional>
#include "ExpandableHashMap.h"

using namespace std;

unsigned int hasher(const string key) {
    return std::hash<std::string>(key);
}

int main() {
    ExpandableHashMap<string, double> nameToGPA(0.3);
    nameToGPA.associate("Carey", 3.5);  // Carey has a 3.5 GPA
    nameToGPA.associate("David", 3.99);
    nameToGPA.associate("Abe", 3.2);

    double* davidsGPA = nameToGPA.find("David");
    if (davidsGPA != nullptr)
        *davidsGPA = 1.5;               // after a re-grade of David’s exam
    nameToGPA.associate("Carey", 4.0);  // Carey deserves a 4.0
    double* lindasGPA = nameToGPA.find("Linda");
    if (lindasGPA == nullptr)
        cout << "Linda is not in the roster!" << endl;
    else
        cout << "Linda’s GPA is: " << *lindasGPA << endl;
}