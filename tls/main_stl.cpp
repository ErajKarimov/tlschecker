#include <CkCert.h>
#include <CkDateTime.h>
#include <CkDtObj.h>
#include <CkSocket.h>

#include <algorithm>
#include <execution>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
  std::vector<std::string> domains;
  std::string domain;
  std::ifstream inputFile(argv[1]);
  std::ofstream outputFile(argv[2]);
  if (!inputFile.is_open()) {
    std::cerr << "Cannot open input file." << std::endl;
    return 1;
  }
  
  if (!outputFile) {
      std::cerr << "Could not open the file for writing." << std::endl;
      return 1;
  }
  
  while (std::getline(inputFile, domain)) {
    domains.push_back(domain);
  }
  inputFile.close();
  
  bool ssl = true;
  int maxWaitMillisec = 400;
  int sslServerPort = 443; 
  bool success;
  CkCert* cert = nullptr;
  CkDateTime* ckdt = nullptr;
  CkDtObj* dt = nullptr;
  std::for_each(std::execution::par, domains.begin(), domains.end(), [&](const std::string& sslServerHost) {
    CkSocket socket;

    success = socket.Connect(sslServerHost.c_str(), sslServerPort, ssl,
                             maxWaitMillisec);
    if (!success) {
      outputFile << sslServerHost <<" domain doesn't exist" << std::endl;
    } else {
      cert = socket.GetSslServerCert();
      if (socket.get_LastMethodSuccess()) {
        ckdt = cert->GetValidToDt();
        dt = ckdt->GetDtObj(true);

        outputFile << sslServerHost
                  << " expired day: " << dt->get_Day() << "."
                  << dt->get_Month() << "."
                  << dt->get_Year() << " "
                  << dt->get_Hour() << ":"
                  << dt->get_Minute() << std::endl;

        delete dt;
        delete ckdt;
        delete cert;
      }
      socket.Close(10);
    }
    });
    outputFile.close();
    return 0;
}
