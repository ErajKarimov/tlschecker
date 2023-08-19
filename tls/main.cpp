#include <CkCert.h>
#include <CkDateTime.h>
#include <CkDtObj.h>
#include <CkSocket.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <thread>

void ChilkatSample(const std::string& sslServerHost) {
    CkSocket socket;

    bool ssl = true;
    int maxWaitMillisec = 400;
    int sslServerPort = 443;

    // Connect to the SSL server:
    bool success = socket.Connect(sslServerHost.c_str(),
                                  sslServerPort, ssl, maxWaitMillisec);
    if (!success) {
        std::cout << sslServerHost <<" domain doesn't exist\r\n";
        return;
    }

    CkCert* cert = socket.GetSslServerCert();
    if (socket.get_LastMethodSuccess()) {
        // Get the certificate valid-to date/time
        CkDateTime* ckdt = cert->GetValidToDt();
        CkDtObj* dt = ckdt->GetDtObj(true);

        std::cout << sslServerHost << " expired day: "
                  << std::to_string(dt->get_Day()) << "."
                  << std::to_string(dt->get_Month()) << "."
                  << std::to_string(dt->get_Year()) << " "
                  << std::to_string(dt->get_Hour()) << ":"
                  << std::to_string(dt->get_Minute()) << "\r\n";

        delete dt;
        delete ckdt;
    }
    socket.Close(10);
}

int main(int argc, char** argv) {
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Cannot open input file." << std::endl;
        return 1;
    }

    std::vector<std::string> domains;
    std::string domain;
    while (std::getline(inputFile, domain)) {
        domains.push_back(domain);
    }
    inputFile.close();

    // Parallel processing of domains
    std::vector<std::thread> threads;
    for (const std::string& domain : domains) {
        threads.emplace_back([&]() {
            ChilkatSample(domain);
        });
    }

    // Wait for all threads to finish
    for (std::thread& thread : threads) {
        thread.join();
    }

    return 0;
}
