#!/bin/bash

gcc -g -std=gnu99 keygen.c -o keygen
gcc -g -std=gnu99 otp_enc_d.c -o otp_enc_d
gcc -g -std=gnu99 otp_enc.c -o otp_enc
gcc -g -std=gnu99 otp_dec_d.c -o otp_dec_d
gcc -g -std=gnu99 otp_dec.c -o otp_dec
gcc -g -std=gnu99 otp.c -o otp

