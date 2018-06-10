#!/bin/bash

gcc -g -std=gnu99 keygen.c -o keygen
gcc -g -std=gnu99 otp_enc_d.c -o otp_enc_d
gcc -g -std=gnu99 otp_enc.c -o otp_enc

