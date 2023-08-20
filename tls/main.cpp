#include <CkCert.h>
#include <CkDateTime.h>
#include <CkDtObj.h>
#include <CkSocket.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>


std::mutex outputMutex;
bool ssl = true;
int maxWaitMillisec = 400;
int sslServerPort = 443;
CkCert* cert = nullptr;
CkDateTime* ckdt = nullptr;
CkDtObj* dt = nullptr;

void processDomain(const std::string& sslServerHost, std::ofstream& outputFile) {
    CkSocket socket;
    bool success = socket.Connect(sslServerHost.c_str(), sslServerPort, ssl, maxWaitMillisec);

    if (!success) {
        std::lock_guard<std::mutex> lock(outputMutex);
        outputFile << sslServerHost << " domain doesn't exist" << std::endl;
    } else {
        {
        std::lock_guard<std::mutex> lock(outputMutex);
        cert = socket.GetSslServerCert();
        if (socket.get_LastMethodSuccess()) {   
            ckdt = cert->GetValidToDt();
            dt = ckdt->GetDtObj(true);
            outputFile << sslServerHost << " expired day: " << dt->get_Day() << "."
                       << dt->get_Month() << "."
                       << dt->get_Year() << " "
                       << dt->get_Hour() << ":"
                       << dt->get_Minute() << std::endl;
        }
            delete dt;
            delete ckdt;
            delete cert;
        }
        socket.Close(10);
    }
}

int main(int argc, char** argv) {
    std::vector<std::string> domains;
    std::string domain;
    std::ifstream inputFile(argv[1]);
    std::ofstream outputFile(argv[2]);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Cannot open input or output file." << std::endl;
        return 1;
    }

    while (std::getline(inputFile, domain)) {
        domains.push_back(domain);
    }
    inputFile.close();

    std::vector<std::thread> threads;
    for (const std::string& sslServerHost : domains) {
        threads.emplace_back(processDomain, sslServerHost, std::ref(outputFile));
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    outputFile.close();
    
    return 0;
}
