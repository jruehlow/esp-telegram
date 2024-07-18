set(CERTS "")

if(NOT CMAKE_BUILD_EARLY_EXPANSION AND EXISTS config AND NOT CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY)
    list(APPEND CERTS cert/api.pem)

    if(NOT EXISTS ${COMPONENT_DIR}/cert/api.pem)
        execute_process(COMMAND ${COMPONENT_DIR}/certgen.sh RESULT_VARIABLE CERTGEN_RESULT)

        if(NOT CERTGEN_RESULT EQUAL "0")
            message(FATAL_ERROR "Failed to generate root certificates\n${CERTGEN_RESULT}")
        endif()
    endif()
endif()

idf_component_register(
    SRCS src/telegram/getMe.c
         src/telegram/_api.c
    INCLUDE_DIRS include
    REQUIRES json esp_http_client mbedtls esp-tls
    PRIV_REQUIRES app_update nvs_flash
    EMBED_TXTFILES ${CERTS}
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
