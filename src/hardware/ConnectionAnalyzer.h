#pragma once
#include "tinyxml2.h"
#include <string>
#include <vector>
#pragma once
#include "tinyxml2.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>

struct ConnectionResult {
    tinyxml2::XMLElement* connection;
    tinyxml2::XMLElement* parentModule;
    bool isImplicit;
    
    ConnectionResult(tinyxml2::XMLElement* conn, tinyxml2::XMLElement* parent, bool implicit = false)
        : connection(conn), parentModule(parent), isImplicit(implicit) {}
        
    friend std::ostream& operator<<(std::ostream& os, const ConnectionResult& result);
};

class ConnectionAnalyzer {
private:
    std::unordered_set<std::string> implicitConnectors = {"SL1", "PS1"};
public:
    ConnectionAnalyzer();
    
    std::vector<ConnectionResult> findConnections(tinyxml2::XMLElement* hardware, const char* targetModule);
    std::string traverseToEnd(tinyxml2::XMLElement* hardware, const char* startModule);
    void traverseAllConnections(tinyxml2::XMLElement* hardware, const char* startModule);
};