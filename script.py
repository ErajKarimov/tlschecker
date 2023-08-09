import ssl
import socket
import datetime
import concurrent.futures

def get_certificate_expiry_date(domain):
    try:
        context = ssl.create_default_context()
        with socket.create_connection((domain, 443)) as sock:
            with context.wrap_socket(sock, server_hostname=domain) as ssock:
                cert = ssock.getpeercert()
                expiry_date = datetime.datetime.strptime(cert['notAfter'], "%b %d %H:%M:%S %Y %Z")
                return expiry_date
    except Exception as e:
        return None

def check_certificate_expiry(domain):
    expiry_date = get_certificate_expiry_date(domain)
    if expiry_date:
        days_until_expiry = (expiry_date - datetime.datetime.now()).days
        formatted_expiry_date = expiry_date.strftime("%d.%m.%y")  # Форматирование даты
        return f"Certificate for {domain} expires on {formatted_expiry_date} ({days_until_expiry} days left)."
    else:
        return f"Failed to retrieve certificate information for {domain}."

if __name__ == "__main__":
    input_filename = "input_domains.txt"
    output_filename = "certificate_results.txt"

    with open(input_filename, "r") as input_file:
        domain_list = input_file.read().splitlines()

    with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
        results = executor.map(check_certificate_expiry, domain_list)

    with open(output_filename, "w") as output_file:
        for result in results:
            output_file.write(result + "\n")

    print("Certificate check results written to", output_filename)
