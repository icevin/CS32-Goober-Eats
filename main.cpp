#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

int main(int argc, char *argv[])
{
    #ifdef BONNIE
    StreetMap sm;
	sm.load("mapdata.txt");

	GeoCoord g("34.0625329", "-118.4470263");
	GeoCoord g2("34.0636671", "-118.4461842");
	PointToPointRouter p(&sm);
	list<StreetSegment> route;
	double dist = 0;

	if (p.generatePointToPointRoute(g, g2, route, dist) == DELIVERY_SUCCESS)
		cout << "yay" << endl;
	else if (p.generatePointToPointRoute(g, g2, route, dist) == NO_ROUTE)
		cout << "no" << endl;
	else if (p.generatePointToPointRoute(g, g2, route, dist) == BAD_COORD)
		cout << "last" << endl;
	//string lat = "34.0547000";
	//string lon = "-118.4794734";
	//GeoCoord g(lat, lon);
	//vector<StreetSegment> segs;
	//if (sm.getSegmentsThatStartWith(g, segs))
	//	cout << "yay" << endl;
        
//    //if (!sm.load(argv[1]))
//    //{
//    //    cout << "Unable to load map data file " << argv[1] << endl;
//    //    return 1;
//    //}
//	sm.load("mapdata.txt");
	GeoCoord temp1("34.0625329", "-118.4470263"); // should return broxton and weyburn twice each
	GeoCoord temp2("34.0732851", "-118.4931016"); // should return 2 glenmere ways
	////GeoCoord temp("34.0724746", "-118.4923463"); // should return glenmere way
	//vector<StreetSegment> segs;
	//bool success = sm.getSegmentsThatStartWith(temp1, segs);
	//if (success)
	//	cout << "Yay" << endl;
	//else
	//	cout << "Keep trying.";
	//for (int i = 0; i < segs.size(); i++)
	//{
	//	cout << segs[i].name << endl;
	//}
	PointToPointRouter ptpr(&sm);
	GeoCoord sproulLanding("34.0712323", "-118.4505969");
	GeoCoord E6("34.0687443", "-118.4449195");
	GeoCoord btp("34.0685657", "-118.4489289");
	//double dist;
	//list<StreetSegment> route ;
	//cout << ptpr.generatePointToPointRoute(sproulLanding, btp, route, dist) << endl;
	//for (auto it = route.begin(); it != route.end(); it++)
	//{
	//	cout << (*it).name << ":" << endl;
	//	cout << "Starting GeoCoord: " << (*it).start.latitude << ", " << (*it).start.longitude << endl;
	//	cout << "Ending GeoCoord: " << (*it).end.latitude << ", " << (*it).start.longitude << endl;
	//	cout << endl;
	//}
	//cout << endl;
	//cout << "Total distance travelled: " << dist << endl;
    GeoCoord depot = sproulLanding;
    vector<DeliveryRequest> deliveries;
    //if (!loadDeliveryRequests(argv[2], depot, deliveries))
    //{
    //    cout << "Unable to load delivery request file " << argv[2] << endl;
    //    return 1;
    //}
	DeliveryRequest a("apples", E6);
	DeliveryRequest b("bananas", btp);
	DeliveryRequest c("cantaloupes", temp1);
	DeliveryRequest d("durianes", temp2);
	deliveries.push_back(a);
	deliveries.push_back(b);
	deliveries.push_back(c);
	deliveries.push_back(d);
    cout << "Generating route...\n\n";

    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;
    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl;

    #endif
    #ifndef BONNIE
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " mapdata.txt deliveries.txt" << endl;
        return 1;
    }

    StreetMap sm;
        
    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord depot;
    vector<DeliveryRequest> deliveries;
    if (!loadDeliveryRequests(argv[2], depot, deliveries))
    {
        cout << "Unable to load delivery request file " << argv[2] << endl;
        return 1;
    }

    cout << "Generating route...\n\n";

    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;
    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl; 
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
    #endif
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}
