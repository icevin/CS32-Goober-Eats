#include <iostream>
#include <functional>
#include "ExpandableHashMap.h"

using namespace std;

unsigned int hasher(const string& key) {
    return std::hash<std::string>()(key);
}

int main() {
    ExpandableHashMap<string, double> nameToGPA(0.3);
    nameToGPA.associate("Carey", 3.5);  // Carey has a 3.5 GPA
    nameToGPA.associate("David", 3.99);
    nameToGPA.associate("Abe", 3.2);

    nameToGPA.associate("Andy", 4.0);
    nameToGPA.associate("Megan", 2.0);
    nameToGPA.associate("Jay", 3.0);
    nameToGPA.associate("Kevin", 6.0);
    nameToGPA.associate("A", 4.0);
    nameToGPA.associate("B", 4.0);
    nameToGPA.associate("Linda", 4.0);

    nameToGPA.associate("An1234dy", 4.0);
    nameToGPA.associate("Me1234gan", 2.0);
    nameToGPA.associate("Ja2134y", 3.0);
    nameToGPA.associate("Ke3412vin", 6.0);
    nameToGPA.associate("3241A", 4.0);
    nameToGPA.associate("B3241", 4.0);
    nameToGPA.associate("Lin1234da", 4.0);


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