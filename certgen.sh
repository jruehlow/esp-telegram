#!/bin/bash

telegram_cert_gen() {
    echo "" | openssl s_client -showcerts -connect $1:443 2> /dev/null | sed -n "1,/Root/d; /BEGIN/,/END/p" | openssl x509 -outform PEM > "$2"
}

telegram_certs() {
    local DIRNAME=$(dirname "${BASH_SOURCE}")
    local CERT_DIR="${DIRNAME}/cert"
    
    mkdir "${CERT_DIR}" 2> /dev/null
    
    echo "Generating api certificate..."
    telegram_cert_gen api.telegram.org "${CERT_DIR}/api.pem"
    if [[ $? != 0 ]]; then return $?; fi
    echo "Generated."

    echo "DONE."
    return 0
}

telegram_certs
