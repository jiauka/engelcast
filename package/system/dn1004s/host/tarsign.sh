#!/bin/bash
if [ ! -d "${1}" ] ; then
    echo "$1 is NOT a directory";
	exit 1
fi
folder=`basename $1`

tar cvfz "$folder.tgz" -C $folder .
openssl=true
if [[ "$openssl" == "true" ]]; then
#create key with
# openssl genrsa -out roomy.private.pem 4096
# openssl rsa -in roomy.private.pem -pubout -out roomy.public.pem
	echo "OPENSSL"
	openssl dgst -sha256 -sign roomy.private.pem -out "$folder".sha256 "$folder.tgz"
	# verify
	openssl dgst -sha256 -verify roomy.public.pem -signature "$folder".sha256 "$folder.tgz"
	# openssl dgst -sha256 -verify roomy.public.pem -signature room101-105.sha256 room101-105.tar

	
else # gpg
	echo "GPG"
	gpg -u 47C9357C6A09CEDBBE9637E044E4A3DE2F55FCD3 --output "$folder.sig" --detach-sig "$folder.tgz"
	gpg -u 47C9357C6A09CEDBBE9637E044E4A3DE2F55FCD3 --sign "$folder.tar"
	# gpg --output doc.txt --decrypt file.txt.gpg
	# gpg --verify room101-105.tar.gpg
	# gpg --output config.tar --decrypt room101-105.tar.gpg
fi
