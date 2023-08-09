#!/bin/bash

input_file="domains.txt"
output_file="results.txt"

# Ensure the output file is empty before starting
> "$output_file"

# Function to extract and format expiration date for a domain
get_expiration_date() {
    domain="$1"
    expiration_date=$(openssl s_client -connect "$domain":443 -servername "$domain" 2>/dev/null | openssl x509 -noout -enddate | cut -d= -f2)
    echo "$domain: $expiration_date"
}

# Export the function to make it available to parallel
export -f get_expiration_date

# Process domains in parallel
cat "$input_file" | parallel -j$(nproc) get_expiration_date {} >> "$output_file"

echo "Done! Results written to $output_file."
