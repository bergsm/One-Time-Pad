This program simulates encryption via the one time pad encryption method. 
More info about the one time pad encryption method can be seen here: https://en.wikipedia.org/wiki/One-time_pad. 

This repo involves 5 programs. 

The first program, keygen, generates a key to be used in the encryption.

The other 4 programs are set up as encryption and decryption clients and servers. Both the encryption and decryption side work in a similar fashion.

The client program takes a key file and message file as arguments and passed to the associated daemon via sockets. 
The daemon performs the encryption/decryption and returns the message back to the client via sockets.
