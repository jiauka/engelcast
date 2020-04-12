/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.4  */

#include "castmessage.pb.h"

#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

const CastMessage_ProtocolVersion CastMessage_protocol_version_default = CastMessage_ProtocolVersion_CASTV2_1_0;
const char CastMessage_source_id_default[128] = "sender-0";
const char CastMessage_destination_id_default[128] = "receiver-0";
const CastMessage_PayloadType CastMessage_payload_type_default = CastMessage_PayloadType_STRING;


const pb_field_t CastMessage_fields[8] = {
    PB_FIELD(  1, UENUM   , REQUIRED, STATIC  , FIRST, CastMessage, protocol_version, protocol_version, &CastMessage_protocol_version_default),
    PB_FIELD(  2, STRING  , REQUIRED, STATIC  , OTHER, CastMessage, source_id, protocol_version, &CastMessage_source_id_default),
    PB_FIELD(  3, STRING  , REQUIRED, STATIC  , OTHER, CastMessage, destination_id, source_id, &CastMessage_destination_id_default),
    PB_FIELD(  4, STRING  , REQUIRED, STATIC  , OTHER, CastMessage, namespace, destination_id, 0),
    PB_FIELD(  5, UENUM   , REQUIRED, STATIC  , OTHER, CastMessage, payload_type, namespace, &CastMessage_payload_type_default),
    PB_FIELD(  6, STRING  , OPTIONAL, STATIC  , OTHER, CastMessage, payload_utf8, payload_type, 0),
    PB_FIELD(  7, BYTES   , OPTIONAL, CALLBACK, OTHER, CastMessage, payload_binary, payload_utf8, 0),
    PB_LAST_FIELD
};

const pb_field_t AuthChallenge_fields[1] = {
    PB_LAST_FIELD
};

const pb_field_t AuthResponse_fields[4] = {
    PB_FIELD(  1, BYTES   , REQUIRED, CALLBACK, FIRST, AuthResponse, signature, signature, 0),
    PB_FIELD(  2, BYTES   , REQUIRED, CALLBACK, OTHER, AuthResponse, client_auth_certificate, signature, 0),
    PB_FIELD(  3, BYTES   , REPEATED, CALLBACK, OTHER, AuthResponse, client_ca, client_auth_certificate, 0),
    PB_LAST_FIELD
};

const pb_field_t AuthError_fields[2] = {
    PB_FIELD(  1, UENUM   , REQUIRED, STATIC  , FIRST, AuthError, error_type, error_type, 0),
    PB_LAST_FIELD
};

const pb_field_t DeviceAuthMessage_fields[4] = {
    PB_FIELD(  1, MESSAGE , OPTIONAL, STATIC  , FIRST, DeviceAuthMessage, challenge, challenge, &AuthChallenge_fields),
    PB_FIELD(  2, MESSAGE , OPTIONAL, STATIC  , OTHER, DeviceAuthMessage, response, challenge, &AuthResponse_fields),
    PB_FIELD(  3, MESSAGE , OPTIONAL, STATIC  , OTHER, DeviceAuthMessage, error, response, &AuthError_fields),
    PB_LAST_FIELD
};


/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(DeviceAuthMessage, challenge) < 65536 && pb_membersize(DeviceAuthMessage, response) < 65536 && pb_membersize(DeviceAuthMessage, error) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_CastMessage_AuthChallenge_AuthResponse_AuthError_DeviceAuthMessage)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
#error Field descriptor for CastMessage.payload_utf8 is too large. Define PB_FIELD_16BIT to fix this.
#endif


