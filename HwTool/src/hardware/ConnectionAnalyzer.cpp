#include "ConnectionAnalyzer.h"
#include <iostream>
#include <unordered_set>

using namespace tinyxml2;  


std::ostream& operator<<(std::ostream& os, const ConnectionResult& result) {
    const char* parentName = result.parentModule->Attribute("Name");
    const char* targetModule = result.connection->Attribute("TargetModule");
    const char* targetConnector = result.connection->Attribute("TargetConnector");
    
    os << "Connection: " << parentName << " -> " << targetModule << ":" << targetConnector;
    if (result.isImplicit) {
        os << " (implicit)";
    }
    return os;
}



ConnectionAnalyzer::ConnectionAnalyzer()
{
    implicitConnectors = {
        "SL1",    // System Link input
        "PS1",    // Power Supply input
    };
}

std::vector<ConnectionResult> ConnectionAnalyzer::findConnections(XMLElement* hardware, const char* targetModule) {
    std::vector<ConnectionResult> results;
    
    for (XMLElement* module = hardware->FirstChildElement("Module"); 
         module; module = module->NextSiblingElement("Module")) {
        
        for (XMLElement* conn = module->FirstChildElement("Connection");
             conn; conn = conn->NextSiblingElement("Connection")) {
            
            const char* target = conn->Attribute("TargetModule");
            if (target && strcmp(target, targetModule) == 0) {
                const char* targetConnector = conn->Attribute("TargetConnector");
                bool isImplicit = targetConnector && 
                                 implicitConnectors.contains(targetConnector);
                
                results.emplace_back(conn, module, isImplicit);
            }
        }
    }
    return results;
}

std::string ConnectionAnalyzer::traverseToEnd(tinyxml2::XMLElement *hardware, const char *startModule)
{
    std::unordered_set<std::string> visited;
    std::string currentModule = startModule;
    
    std::cout << "Starting traversal from: " << currentModule << std::endl;
    
    while (true) {
        if (visited.contains(currentModule)) {
            std::cout << "Cycle detected at: " << currentModule << std::endl;
            break;
        }
        
        visited.insert(currentModule);
        
        // Find modules that connect TO the current module (reverse direction)
        std::string nextModule;
        bool foundConnection = false;
        
        for (XMLElement* module = hardware->FirstChildElement("Module"); 
             module; module = module->NextSiblingElement("Module")) {
            
            for (XMLElement* conn = module->FirstChildElement("Connection");
                 conn; conn = conn->NextSiblingElement("Connection")) {
                
                const char* target = conn->Attribute("TargetModule");
                if (target && strcmp(target, currentModule.c_str()) == 0) {
                    
                    const char* targetConnector = conn->Attribute("TargetConnector");
                    bool isImplicit = targetConnector && 
                                     implicitConnectors.contains(targetConnector);
                    
                    if (!isImplicit) {  // Only follow explicit connections
                        const char* moduleName = module->Attribute("Name");
                        if (moduleName) {
                            nextModule = moduleName;
                            foundConnection = true;
                            std::cout << "Following: " << currentModule << " <- " << moduleName << std::endl;
                            break;  // Take the first explicit connection
                        }
                    }
                }
            }
            
            if (foundConnection) break;
        }
        
        if (!foundConnection) {
            std::cout << "No incoming connections to: " << currentModule << " (END)" << std::endl;
            break;
        }
        
        currentModule = nextModule;
    }
    
    return currentModule;
}