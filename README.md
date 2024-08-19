# The codeing three assignment of Introduction of Computer Network (11120EECS302001)
# Depenadancy
* gcc, make
# Build
* make
## Lab 1 - Introduction to Wireshark
* Capture the icmp packets by Wireshark.
* Curl the Server and Post the Student_ID as data.
```
ping 8.8.8.8
curl -X POST --data "input=111065540" http://140.114.79.144/idnex.php
```
## Lab 2 - TCP Socket Programming
Start from the [pack](./lab2/Pack_student)

* Connect server and client with TCP socket and successfully send a
 message.
* Create TCP header (without checksum) using l4info.
* Complete the header (checksum).
## Lab 3
