#include <iostream>
#include "StreetMap.cpp"
#include "PointToPointRouter.cpp"

using namespace std;

int main(int argc, char *argv[]) {
    StreetMap sm;    
    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord end("34.0425011", "-118.4967245");
    GeoCoord start("34.0425011", "-118.4967245");
    PointToPointRouter testRouter(&sm);
    list<StreetSegment> route;
    double distance = 123232;
    if (testRouter.generatePointToPointRoute(start, end, route, distance) == DELIVERY_SUCCESS)
        cout << "route found" << endl;
    for(StreetSegment s : route) {
        cout << s.start.latitudeText << " " << s.start.longitudeText << "|" << s.end.latitudeText << " " << s.end.longitudeText << "|  " << s.name << endl;
    }
    cout << "distance: " << distance << endl;
    return 0;
}