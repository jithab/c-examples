#!/usr/bin/bsh
set -e

rm -rf build; mkdir build

# Generate CA private key and certificate
openssl genpkey -algorithm RSA -out build/ca.key -pkeyopt rsa_keygen_bits:2048
openssl req -x509 -new -nodes -key build/ca.key -sha256 -days 365 -out build/ca.crt -subj "/C=US/ST=California/L=San Francisco/O=MyCompany/CN=MyRootCA"

# Generate server private key
openssl genpkey -algorithm RSA -out build/server.key -pkeyopt rsa_keygen_bits:2048
# Generate certificate signing request (CSR) for server
openssl req -new -key build/server.key -out build/server.csr -subj "/C=US/ST=California/L=San Francisco/O=MyCompany/CN=server.mydomain.com"
# Sign server CSR with CA
openssl x509 -req -in build/server.csr -CA build/ca.crt -CAkey build/ca.key -CAcreateserial -out build/server.crt -days 365 -sha256

# Generate client private key
openssl genpkey -algorithm RSA -out build/client.key -pkeyopt rsa_keygen_bits:2048
# Generate certificate signing request (CSR) for server
openssl req -new -key build/client.key -out build/client.csr -subj "/C=US/ST=California/L=San Francisco/O=MyCompany/CN=client.mydomain.com"
# Sign server CSR with CA
openssl x509 -req -in build/client.csr -CA build/ca.crt -CAkey build/ca.key -CAcreateserial -out build/client.crt -days 365 -sha256


# Verifying server certificate with ca
openssl verify -CAfile build/ca.crt build/server.crt
# Verifying client certificate with ca
openssl verify -CAfile build/ca.crt build/client.crt

# Compile server
gcc server.c -o build/server -lssl -lcrypto
# Compile client 
gcc client.c -o build/client -lssl -lcrypto

# Run the client
echo -e "\n\n\nRun client in another shell: build/client"

# Run the server
build/server
