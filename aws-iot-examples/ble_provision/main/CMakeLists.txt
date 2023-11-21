
# Register all the required components
idf_component_register(
                        SRC_DIRS "."
                        INCLUDE_DIRS "."
                        PRIV_REQUIRES bs_esp32_platform
)

# Embed the AWS certificate in the binary
target_add_binary_data(${COMPONENT_TARGET} "../../_Certificates/claimCerts/aws-root-ca.pem" TEXT)

