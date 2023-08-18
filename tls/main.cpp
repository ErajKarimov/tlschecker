#include <CkCert.h>
#include <CkDateTime.h>
#include <CkDtObj.h>
#include <CkSocket.h>

#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

std::string ChilkatSample(const std::string& sslServerHost) {
  CkSocket socket;

  bool ssl = true;
  int maxWaitMillisec = 400;
  int sslServerPort = 443;

  // Connect to the SSL server:
  std::string result;
  bool success = socket.Connect(static_cast<const char*>(sslServerHost.c_str()),
                                sslServerPort, ssl, maxWaitMillisec);
  if (success != true) {
    result = "error domain doesn't exist\r\n";
    return result;
  }

  CkCert* cert = 0;

  cert = socket.GetSslServerCert();
  std::string s1 = ".";
  std::string s2 = ":";
  if (socket.get_LastMethodSuccess() != false) {
    // Get the certificate valid-to date/time
    CkDateTime* ckdt = cert->GetValidToDt();
    CkDtObj* dt = ckdt->GetDtObj(true);

    result = std::to_string(dt->get_Day()) + s1 +
             std::to_string(dt->get_Month()) + s1 +
             std::to_string(dt->get_Year()) + "  " +
             std::to_string(dt->get_Hour()) + s2 +
             std::to_string(dt->get_Minute()) + "\r\n";
    delete dt;
    delete ckdt;
    delete cert;
  }
  socket.Close(10);
  return result;
}

void processDomains(const std::vector<std::string>& domains,
                    std::mutex& outputFileMutex, std::ofstream& outputFile,
                    size_t startIdx, size_t endIdx) {
  for (size_t i = startIdx; i < endIdx; ++i) {
    std::string result = ChilkatSample(domains[i]);

    // Accumulate the results in a buffer
    std::ostringstream buffer;
    buffer << domains[i] << " expired day: " << result;

    // Lock and write the buffer to the output file
    std::lock_guard<std::mutex> lock(outputFileMutex);
    outputFile << buffer.str();
  }
}

int main() {
  std::ifstream inputFile("domains.txt");
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

  std::ofstream outputFile("results.txt");
  if (!outputFile.is_open()) {
    std::cerr << "Cannot open output file." << std::endl;
    return 1;
  }

  std::mutex outputFileMutex;

  // Calculate number of available threads and distribute domains accordingly
  size_t numThreads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;
  size_t chunkSize = (domains.size() + numThreads - 1) / numThreads;

  for (size_t i = 0; i < numThreads; ++i) {
    size_t startIdx = i * chunkSize;
    size_t endIdx = std::min((i + 1) * chunkSize, domains.size());
    threads.emplace_back(processDomains, std::cref(domains),
                         std::ref(outputFileMutex), std::ref(outputFile),
                         startIdx, endIdx);
  }

  // Wait for all threads to finish
  for (auto& thread : threads) {
    thread.join();
  }

  outputFile.close();

  return 0;
}
