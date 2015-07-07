
A lot of things in this folder
==================================================
1.Design Coflow pdf
--------------------------------------------------
Introduce how to design coflow in Linux kernel.

2.GenCoflow
---------------------------------------------------
It is an executable file that can run on Linux system like Ubuntu.
It generates coflow traffic and run on the coflow client.

3.GenCoflow.c
---------------------------------------------------
This is the source code of GenCoflow. How to compile? It depends on your system and it needs myunp.h header.


4.GenCoflowMapLarge4.c
-------------------------------------------------------
It also generates coflow traffic. The difference between GenCoflow and GenCoflowMapLarge4 is to utilize the muticores CPU in multiple threads.

5.GenCoflowPerFlow.c
-----------------------------------------------------------------------
Genearte coflow traffic, but one coflow only one flow.

6.myunp.h
------------------------------------------------
compile *.c need this header.

7.pica8dscpSPconfig
------------------------------------------------------------------
How to configure PICA8 DSCP Strict Priority

8.receive
------------------------------------------------------------
It is an executable file that can run on Linux system like Ubuntu.
It runs on the coflow server. Listen 9877 ports.

9.receive.c
--------------------------------------------------------------
This is the source code of receive.
How to compile? It depends on your system and it needs myunp.h header.


