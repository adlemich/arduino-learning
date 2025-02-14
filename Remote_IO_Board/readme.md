This is a little test setup to work with Arduino as a remote IO-Board.
It uses a Ethernet Shield so that Arduino acts as a TCP/IP Server.
This setup includes a program for the Ardunio to allow remote interaction with the following devices:
 - 3 Input Buttons
 - 1 Andon Light, using 3 LEDs
 - 1 LED Multi-Color Matrix Display
 - 1 Text Display

With the windows test client, you can test the remote functionality and use the client class as an interface to the remote IO.

A roundtrip test is also included, which runs 1000 messages to the client and back, measured on Arduino side. In my setup, over 1 Switch, a IO roundtrip takes about 1.7 ms in average.
