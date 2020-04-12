#!/bin/bash
#create key with
# openssl genrsa -out roomy.private.pem 4096
# openssl rsa -in roomy.private.pem -pubout -out roomy.public.pem
echo "OPENSSL"
basefile=`basename $1|cut -d'.' -f1`
openssl dgst -sha256 -sign roomy.private.pem -out "$basefile".sha256 "$1"
# verify
openssl dgst -sha256 -verify roomy.public.pem -signature "$basefile".sha256 "$1"
# openssl dgst -sha256 -verify roomy.public.pem -signature room101-105.sha256 room101-105.tar
