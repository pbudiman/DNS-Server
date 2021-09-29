# DNS Server
### Task
1. Write a miniature DNS server that will serve AAAA queries.
2. Accept a DNS “AAAA” query over TCP on port 8053. Forward it to a server within the university connection whose IPv4 address is the first command-line argument and whose port is the second command-line argument. 
3. Send the response back to the client who sent the request, over the same TCP connection. 
4. Log these events
5. Cache the five (5) most recent answers to queries. Answer directly if possible, instead of querying the server again.
6. You should create a Makefile that produces an executable named ’dns_svr’

### Result Log
Keep a log at ./dns_svr.log (i.e., in current directory) with messages of the following format:
- < timestamp > requested <domain_name> – when you receive a request
- < timestamp > unimplemented request – if the request type is not AAAA
- < timestamp > <domain_name> expires at <timestamp> – for each request you receive that is in your cache
- < timestamp > replacing <domain_name> by <domain_name> – for each cache eviction
- < timestamp > <domain_name> is at <IP address> – see 2.1
