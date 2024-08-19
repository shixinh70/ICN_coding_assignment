# The codeing three assignment of Introduction of Computer Network (11120EECS302001)
# Depenadancy
* gcc, make
# Build
* make
## Lab 1 - Introduction to Wireshark
* Task1 - Capture the icmp packets by Wireshark.
* Task2 - the Server and Post the Student_ID as data.
```
ping 8.8.8.8
curl -X POST --data "input=111065540" http://140.114.79.144/idnex.php
```
## Lab 2 - TCP Socket Programming
Start from the [Pack_Student](./lab2/Pack_student)
* Task1 - Connect server and client with TCP socket and successfully send a
 message.
* Task2 - Receive the plaint text from the server, and convert it into l4info.
* Task3 - Complete the l4info (checksum).

The server will tell you your score and the result.
![Screenshot from 2024-08-19 13-24-33](https://github.com/user-attachments/assets/c8f935e8-347c-418d-bef9-04d254634e8d)

## Lab3 – TCP Reliable Data Transfer
Start from the [Pack_Student](./lab3/pack_student)
* Task1 - Finishing the Three way handshake of our costom TCP.
* Task2 - Receive the picture sent from the Server (packets might be corrupted).
* Task3 - Trigger the retransmission of corrupted packet and get the complete picture.

