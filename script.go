package main

import (
    "bufio"
    "crypto/tls"
    "fmt"
    "os"
    "sync"
)

func main() {
    inputFilePath := "input_domains.txt"
    outputFilePath := "output.txt"

    domains := readDomainsFromFile(inputFilePath)

    // Number of worker goroutines
    numWorkers := 10

    // Create a channel to receive results
    results := make(chan string, len(domains))

    // Create a wait group to wait for all workers to finish
    var wg sync.WaitGroup

    // Divide the domains among workers
    domainsPerWorker := len(domains) / numWorkers

    // Create worker goroutines
    for i := 0; i < numWorkers; i++ {
        wg.Add(1)
        go func(startIndex, endIndex int) {
            defer wg.Done()
            processDomains(domains[startIndex:endIndex], results)
        }(i*domainsPerWorker, (i+1)*domainsPerWorker)
    }

    // Close the results channel when all workers are done
    go func() {
        wg.Wait()
        close(results)
    }()

    // Collect results and write them to the output file
    writeResultsToFile(outputFilePath, results)
}

func readDomainsFromFile(filePath string) []string {
    file, err := os.Open(filePath)
    if err != nil {
        panic(err)
    }
    defer file.Close()

    var domains []string
    scanner := bufio.NewScanner(file)
    for scanner.Scan() {
        domains = append(domains, scanner.Text())
    }
    if err := scanner.Err(); err != nil {
        panic(err)
    }

    return domains
}

func processDomains(domains []string, results chan<- string) {
    for _, domain := range domains {
        expirationDate := getCertificateExpiration(domain)
        results <- fmt.Sprintf("%s - %s\n", domain, expirationDate)
    }
}

func getCertificateExpiration(domain string) string {
    conn, err := tls.Dial("tcp", domain+":443", nil)
    if err != nil {
        return "Error"
    }
    defer conn.Close()

    cert := conn.ConnectionState().PeerCertificates[0]
    return cert.NotAfter.Format("02.01.06")
}

func writeResultsToFile(filePath string, results <-chan string) {
    file, err := os.Create(filePath)
    if err != nil {
        panic(err)
    }
    defer file.Close()

    for result := range results {
        _, err := file.WriteString(result)
        if err != nil {
            panic(err)
        }
    }
}
