#include <iostream>
#include "StreetMap.cpp"

using namespace std;

int main(int argc, char *argv[]) {
    StreetMap sm;    
    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }
    GeoCoord test("34.0724746", "-118.4923463");
    vector<StreetSegment> test2;
    sm.getSegmentsThatStartWith(test, test2);
    cout << test2.size();
    for (StreetSegment a : test2)
        cout << a.start.latitudeText << "_" << a.start.longitudeText << "|" << a.end.latitudeText << "_" << a.end.longitudeText << "|" << a.name << endl;
    return 0;
}