# Netest

Network test/diagonostic tool

# Test Target

- ping
- ping6
- dig
- tcping

# Environment

- Mac OS X 10.6 or later
- Ruby 2.0.0p247 or later

# Install

## Download netest

```https://github.com/yuyarin/netest.git```

## Install tcping

```
cd src
gcc -m32 tcping.c
sudo cp a.out /usr/local/bin/tcping
```

## How to use

```./netest google.conf```

## Sample output

```
Ping6
  PING IPv6 2404:6800:4004:804::1011 with size 64, expect rtt<100 [FAIL]
  PING IPv6 2404:6800:4004:804::1011 with size 1232, expect rtt<100 [FAIL]
Ping
  PING IPv4 74.125.235.145 with size 64, expect rtt<100 [FAIL]
    64 bytes from 74.125.235.145: icmp_seq=0 ttl=55 time=50.626 ms
    Request timeout for icmp_seq 1
    64 bytes from 74.125.235.145: icmp_seq=2 ttl=55 time=59.462 ms
  PING IPv4 74.125.235.145 with size 1472, expect rtt<100 [FAIL]
    64 bytes from 74.125.235.145: icmp_seq=0 ttl=55 time=49.183 ms
    Request timeout for icmp_seq 1
    64 bytes from 74.125.235.145: icmp_seq=2 ttl=55 time=56.755 ms
Dig
  Dig IPv4 A www.google.com, expect status=="NOERROR" and ans =~ /[\d\.]+/ [OK]
    ; <<>> DiG 9.8.3-P1 <<>> +time=1 +tries=1 -4 A www.google.com
    ;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 5985
    ;www.google.com.			IN	A
    www.google.com.		269	IN	A	173.194.126.179
    www.google.com.		269	IN	A	173.194.126.180
    www.google.com.		269	IN	A	173.194.126.176
    www.google.com.		269	IN	A	173.194.126.177
    www.google.com.		269	IN	A	173.194.126.178
  Dig IPv4 AAAA www.google.com, expect status=="NOERROR" and ans =~ /[:0-9a-fA-F]+/ [OK]
    ; <<>> DiG 9.8.3-P1 <<>> +time=1 +tries=1 -4 AAAA www.google.com
    ;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 47196
    ;www.google.com.			IN	AAAA
    www.google.com.		122	IN	AAAA	2404:6800:4004:804::1010
Tcping
  TCP PING IPv4 www.google.com 80, expect status=="open" and rtt<100 [OK]
    www.google.com port 80 status:open rtt:31.8
  TCP PING IPv6 www.google.com 80, expect status=="open" and rtt<100 [FAIL]
    error: www.google.com port 80: No route to host
```

