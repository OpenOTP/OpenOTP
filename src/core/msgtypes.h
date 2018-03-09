#pragma once
#include "types.h"

/* Message boolean macros */
#define SUCCESS 1
#define FAILURE 0

/* Defined context values */
const uint32_t STATESERVER_CONTEXT_WAKE_CHILDREN = 1001;

/* Msgtype limits enum */
enum MsgtypeRanges {
    // Control range
    CONTROL_MSGTYPE_MIN = 9000,
    CONTROL_MSGTYPE_MAX = 9999,
    // ClientAgent range
    CLIENTAGENT_MSGTYPE_MIN = 1000,
    CLIENTAGENT_MSGTYPE_MAX = 1999,
    // StateServer range
    STATESERVER_MSGTYPE_MIN = 2000,
    STATESERVER_MSGTYPE_MAX = 2999,
    // DatabaseServer range
    DBSERVER_MSGTYPE_MIN = 3000,
    DBSERVER_MSGTYPE_MAX = 3999,
};

/* Message type definitions */
enum MessageTypes {
    // Reserved
    RESERVED_MSG_TYPE = 0,

    // Control messages
    CONTROL_ADD_CHANNEL        = 9000,
    CONTROL_REMOVE_CHANNEL     = 9001,
    CONTROL_ADD_RANGE          = 9002,
    CONTROL_REMOVE_RANGE       = 9003,
    CONTROL_ADD_POST_REMOVE    = 9010,
    CONTROL_CLEAR_POST_REMOVES = 9011,
    CONTROL_SET_CON_NAME       = 9012,
    CONTROL_SET_CON_URL        = 9013,
    CONTROL_LOG_MESSAGE        = 9014,

    // ClientAgent messages
    CLIENTAGENT_SET_STATE                  = 1000,
    CLIENTAGENT_SET_CLIENT_ID              = 1001,
    CLIENTAGENT_SEND_DATAGRAM              = 1002,
    CLIENTAGENT_EJECT                      = 1004,
    CLIENTAGENT_DROP                       = 1005,
    CLIENTAGENT_GET_NETWORK_ADDRESS        = 1006,
    CLIENTAGENT_GET_NETWORK_ADDRESS_RESP   = 1007,
    CLIENTAGENT_DECLARE_OBJECT             = 1010,
    CLIENTAGENT_UNDECLARE_OBJECT           = 1011,
    CLIENTAGENT_ADD_SESSION_OBJECT         = 1012,
    CLIENTAGENT_REMOVE_SESSION_OBJECT      = 1013,
    CLIENTAGENT_SET_FIELDS_SENDABLE        = 1014,
    CLIENTAGENT_OPEN_CHANNEL               = 1100,
    CLIENTAGENT_CLOSE_CHANNEL              = 1101,
    CLIENTAGENT_ADD_POST_REMOVE            = 1110,
    CLIENTAGENT_CLEAR_POST_REMOVES         = 1111,
    CLIENTAGENT_ADD_INTEREST               = 1200,
    CLIENTAGENT_ADD_INTEREST_MULTIPLE      = 1201,
    CLIENTAGENT_REMOVE_INTEREST            = 1203,
    CLIENTAGENT_DONE_INTEREST_RESP         = 1204,

    // StateServer control messages
    STATESERVER_CREATE_OBJECT_WITH_REQUIRED       = 2000,
    STATESERVER_CREATE_OBJECT_WITH_REQUIRED_OTHER = 2001,
    STATESERVER_DELETE_AI_OBJECTS                 = 2009,
    // StateServer object messages
    STATESERVER_OBJECT_GET_FIELD         = 2010,
    STATESERVER_OBJECT_GET_FIELD_RESP    = 2011,
    STATESERVER_OBJECT_GET_FIELDS        = 2012,
    STATESERVER_OBJECT_GET_FIELDS_RESP   = 2013,
    STATESERVER_OBJECT_GET_ALL           = 2014,
    STATESERVER_OBJECT_GET_ALL_RESP      = 2015,
    STATESERVER_OBJECT_SET_FIELD         = 2020,
    STATESERVER_OBJECT_SET_FIELDS        = 2021,
    STATESERVER_OBJECT_DELETE_FIELD_RAM  = 2030,
    STATESERVER_OBJECT_DELETE_FIELDS_RAM = 2031,
    STATESERVER_OBJECT_DELETE_RAM        = 2032,
    // StateServer visibility messages
    STATESERVER_OBJECT_SET_LOCATION                       = 2040,
    STATESERVER_OBJECT_CHANGING_LOCATION                  = 2041,
    STATESERVER_OBJECT_ENTER_LOCATION_WITH_REQUIRED       = 2042,
    STATESERVER_OBJECT_ENTER_LOCATION_WITH_REQUIRED_OTHER = 2043,
    STATESERVER_OBJECT_GET_LOCATION                       = 2044,
    STATESERVER_OBJECT_GET_LOCATION_RESP                  = 2045,
    STATESERVER_OBJECT_LOCATION_ACK                       = 2046,
    STATESERVER_OBJECT_SET_AI                             = 2050,
    STATESERVER_OBJECT_CHANGING_AI                        = 2051,
    STATESERVER_OBJECT_ENTER_AI_WITH_REQUIRED             = 2052,
    STATESERVER_OBJECT_ENTER_AI_WITH_REQUIRED_OTHER       = 2053,
    STATESERVER_OBJECT_GET_AI                             = 2054,
    STATESERVER_OBJECT_GET_AI_RESP                        = 2055,
    STATESERVER_OBJECT_SET_OWNER                          = 2060,
    STATESERVER_OBJECT_CHANGING_OWNER                     = 2061,
    STATESERVER_OBJECT_ENTER_OWNER_WITH_REQUIRED          = 2062,
    STATESERVER_OBJECT_ENTER_OWNER_WITH_REQUIRED_OTHER    = 2063,
    STATESERVER_OBJECT_GET_OWNER                          = 2064,
    STATESERVER_OBJECT_GET_OWNER_RESP                     = 2065,
    STATESERVER_OBJECT_ENTER_INTEREST_WITH_REQUIRED       = 2066,
    STATESERVER_OBJECT_ENTER_INTEREST_WITH_REQUIRED_OTHER = 2067,
    // StateServer parent-method messages
    STATESERVER_OBJECT_GET_ZONE_OBJECTS     = 2100,
    STATESERVER_OBJECT_GET_ZONES_OBJECTS    = 2102,
    STATESERVER_OBJECT_GET_CHILDREN         = 2104,
    STATESERVER_OBJECT_GET_ZONE_COUNT       = 2110,
    STATESERVER_OBJECT_GET_ZONE_COUNT_RESP  = 2111,
    STATESERVER_OBJECT_GET_ZONES_COUNT      = 2112,
    STATESERVER_OBJECT_GET_ZONES_COUNT_RESP = 2113,
    STATESERVER_OBJECT_GET_CHILD_COUNT      = 2114,
    STATESERVER_OBJECT_GET_CHILD_COUNT_RESP = 2115,
    STATESERVER_OBJECT_DELETE_ZONE          = 2120,
    STATESERVER_OBJECT_DELETE_ZONES         = 2122,
    STATESERVER_OBJECT_DELETE_CHILDREN      = 2124,
    STATESERVER_GET_ACTIVE_ZONES            = 2125,
    STATESERVER_GET_ACTIVE_ZONES_RESP       = 2126,
    // DBSS object messages
    DBSS_OBJECT_ACTIVATE_WITH_DEFAULTS       = 2200,
    DBSS_OBJECT_ACTIVATE_WITH_DEFAULTS_OTHER = 2201,
    DBSS_OBJECT_GET_ACTIVATED                = 2207,
    DBSS_OBJECT_GET_ACTIVATED_RESP           = 2208,
    DBSS_OBJECT_DELETE_FIELD_RAM             = 2230,
    DBSS_OBJECT_DELETE_FIELDS_RAM            = 2231,
    DBSS_OBJECT_DELETE_DISK                  = 2232,
    DBSS_ADD_POST_REMOVE                     = 2233,
    DBSS_CLEAR_POST_REMOVES                  = 2234,

    // DatabaseServer messages
    DBSERVER_CREATE_OBJECT                    = 3000,
    DBSERVER_CREATE_OBJECT_RESP               = 3001,
    DBSERVER_OBJECT_GET_FIELD                 = 3010,
    DBSERVER_OBJECT_GET_FIELD_RESP            = 3011,
    DBSERVER_OBJECT_GET_FIELDS                = 3012,
    DBSERVER_OBJECT_GET_FIELDS_RESP           = 3013,
    DBSERVER_OBJECT_GET_ALL                   = 3014,
    DBSERVER_OBJECT_GET_ALL_RESP              = 3015,
    DBSERVER_OBJECT_SET_FIELD                 = 3020,
    DBSERVER_OBJECT_SET_FIELDS                = 3021,
    DBSERVER_OBJECT_SET_FIELD_IF_EQUALS       = 3022,
    DBSERVER_OBJECT_SET_FIELD_IF_EQUALS_RESP  = 3023,
    DBSERVER_OBJECT_SET_FIELDS_IF_EQUALS      = 3024,
    DBSERVER_OBJECT_SET_FIELDS_IF_EQUALS_RESP = 3025,
    DBSERVER_OBJECT_SET_FIELD_IF_EMPTY        = 3026,
    DBSERVER_OBJECT_SET_FIELD_IF_EMPTY_RESP   = 3027,
    DBSERVER_OBJECT_DELETE_FIELD              = 3030,
    DBSERVER_OBJECT_DELETE_FIELDS             = 3031,
    DBSERVER_OBJECT_DELETE                    = 3032,
};
