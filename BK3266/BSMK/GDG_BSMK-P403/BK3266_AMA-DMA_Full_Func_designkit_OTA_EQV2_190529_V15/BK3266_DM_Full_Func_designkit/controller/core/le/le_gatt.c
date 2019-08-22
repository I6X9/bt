/***********************************************************************
 *
 * MODULE NAME:    le_gatt.c
 * PROJECT CODE:   CEVA Low Energy Single Mode
 * DESCRIPTION:    Implementation of the ATT Protocol and GATT.
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:  Janurary 2012
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 2011-2012 Ceva Inc.
 *     All rights reserved.
 *
 ***********************************************************************/
#include "config.h"
#ifdef BT_DUALMODE
#include "sys_config.h"
#include "sys_types.h"
#include "sys_features.h"
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
#include "tra_queue.h"
#include "le_const.h"
#include "le_config.h"
#include "le_connection.h"
#include "le_link_layer.h"
#include "le_security.h"
#include "lmp_utils.h"
#include "sys_rand_num_gen.h"
#include "le_att.h"
#include "LE_gatt_server.h"
#include "hw_lc.h"
#include "hw_le_lc.h"
#include "bt_timer.h"
#include "le_smp.h"
#include "le_security_db.h"
#include "tc_event_gen.h"
#include "le_att.h"
#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
#include "le_gap.h"
#endif
#include "sys_mmi.h"

#include "le_cts_app.h"

#if 1 //(PRH_BS_CFG_SYS_GATT_DELIVER_EVENTS_VIA_HCI == 1) // GF 29 Aug

typedef struct t_gatt_event_size
{
    u_int8 event_id;
    u_int8 payload_offset;
    u_int8 length_offset;
    u_int8 type;
}t_gatt_event_size;

t_gatt_event_size gatt_event_len[13] =
    {{ GATT_DISCOVER_SERVICES_BY_UUID_EVENT, 5, 6, 2 },
     { GATT_DISCOVER_CHARACTERISTICS_EVENT,  6, 6, 2},
     { GATT_DISCOVER_SERVICES_EVENT, 6, 6 , 2},
     { GATT_READ_BY_TYPE_RESPONSE_EVENT, 6, 6, 2},
     { GATT_DISCOVER_INCLUDED_SERVICES_EVENT, 6, 6, 2 },
     { GATT_DISCOVER_DESCRIPTORS_EVENT, 6, 6, 2 },
     { GATT_READ_RESPONSE_EVENT, 5, 5, 1 },
     { GATT_VALUE_NOTIFICATION_EVENT, 5, 6, 1},
     { GATT_VALUE_INDICATION_EVENT, 5, 6, 1 },
     { GATT_WRITE_COMPLETE_EVENT, 0, 4, 0 },
     { GATT_VALUE_CONFIRMATION_EVENT, 0, 4, 0 },
     { GATT_WRITE_INDICATION_EVENT, 0, 4, 0 },
     { GATT_INFORMATION_READ_COMPLETE_EVENT, 0, 4, 0 }};
#endif

#define _Get_Uint16(p_pdu)   ((u_int16)p_pdu[0] + ((u_int16) p_pdu[1] << 8))
extern void _Insert_Uint16(t_p_pdu p_buffer, u_int16 value_16_bit);
extern void _Insert_Uint32(t_p_pdu p_buffer, u_int32 value_32_bit);
extern void _Insert_Uint24(t_p_pdu p_buffer,u_int32 val24);

t_ATT_config ATT_config;

#define GATT_MAX_NUM_PROTOCOL_USERS  2

void* GATT_Event_Callback_Array[GATT_MAX_NUM_PROTOCOL_USERS];
void* GATT_Command_Complete_Callback_Array[GATT_MAX_NUM_PROTOCOL_USERS];


//u_int8 GATT_Write_Complete_Event(u_int16 handle,u_int8 status);
u_int8 _GATT_Max_MTU_Event(u_int16 handle,u_int16 max_mtu,u_int8 status);
u_int8 _GATT_Find_Characteric_UUID_In_Payload( u_int8* p_pdu, u_int8* p_attrib_val_array, u_int8 attrib_len, u_int8 mtu_len, u_int8* p_UUID,u_int8 uuid_len);
void _GATT_Clear_Timer(t_LE_Connection* p_connection);
void _GATT_ReSet_GATT_Timer(t_LE_Connection* p_connection);

void _Set_Attribute_Value(u_int8* p_pdu, u_int8* p_value, u_int16 val_len);
void _Set_Attribute_Signature(u_int8* p_pdu, u_int8* p_signature );
u_int16 _Att_Get_Last_Handle(u_int8 opcode,u_int8* p_data,u_int16 mtu_len,u_int8 attrib_len);
u_int8 _GATT_Compare_UUID(u_int8 UUID_len,u_int8* p_UUID1,u_int8* p_UUID2 );
extern u_int16 GATTserver_Handle_in_Error;

void Gatt_Set_MTU_Size(u_int16 mtu_size)
{
    ATT_config.MTU_LEN = mtu_size;
    ATT_config.protocol = 0;
}



/**************************************************************************
 * FUNCTION :- GATT_init
 *
 * DESCRIPTION
 * This function is responsible for initialising the GATT protocol and layer.
 *
 *************************************************************************/

void GATT_init()
{
    /* Only the Event and Callback arrays are initialised */
    u_int8 i;

    for(i=0;i< GATT_MAX_NUM_PROTOCOL_USERS;i++)
    {
        GATT_Event_Callback_Array[i] = 0x00;
        GATT_Command_Complete_Callback_Array[i] = 0x00;
    }
}

/**************************************************************************
 * FUNCTION :- GATT_Register_Callbacks
 *
 * DESCRIPTION
 * This function is responsible for registering callbacks functions the GATT_Event
 * and GATT_Command_Complete.
 *
 *************************************************************************/

void GATT_Register_Callbacks(void* event_funct_ptr, void* command_complete_funct_ptr)
{
    GATT_Event_Callback_Array[1] = event_funct_ptr;
    GATT_Command_Complete_Callback_Array[1] = command_complete_funct_ptr;
}


/**************************************************************************
 * FUNCTION :- GATT_Register_Callbacks
 *
 * DESCRIPTION
 * This function is responsible for registering GAP callbacks functions the GATT_Event
 * and GATT_Command_Complete.
 *
 *************************************************************************/

void GATT_Register_GAP_Callbacks(void* event_funct_ptr, void* command_complete_funct_ptr)
{
    /* Only the Event and Callback arrays are initialised at index 0 for GAP */

    GATT_Event_Callback_Array[0] = event_funct_ptr;
    GATT_Command_Complete_Callback_Array[0] = command_complete_funct_ptr;
}



/**************************************************************************
 * FUNCTION :- GATT_DeRegister_Callbacks
 *
 * DESCRIPTION
 * This function is responsible for De-registering callbacks functions the GATT_Event
 * and GATT_Command_Complete.
 *
 *
 *************************************************************************/

void GATT_DeRegister_Callbacks(void)
{
    GATT_Event_Callback_Array[1] = 0x00;
    GATT_Command_Complete_Callback_Array[1] = 0x00;
}

/**************************************************************************
 * FUNCTION :- GATT_DeRegister_GAP_Callbacks
 *
 * DESCRIPTION
 * This function is responsible for De-registering callbacks functions the GATT_Event
 * and GATT_Command_Complete.
 *
 *
 *************************************************************************/

void GATT_DeRegister_GAP_Callbacks(void)
{
    GATT_Event_Callback_Array[0] = 0x00;
    GATT_Command_Complete_Callback_Array[0] = 0x00;
}
void GATT_Discover_DEVICE_NAME(void)/* TP/IDLE/NAMP/BV-01-C  GAP layer */
{
	 t_Att_pdu_params  pdu_params;
   t_Att_pdu_params* p_pdu_params = &pdu_params;
	 t_LE_Connection* p_connection;
	 u_int16 connection_handle ;
	/* TP/IDLE/NAMP/BV-01-C GAP layer */
	p_connection = LEconnection_Find_Link_Entry(0);
	connection_handle =  LEconnection_Determine_Connection_Handle(p_connection);
	//opcode = ATT_READ_BY_TYPE_REQUEST;
	pdu_params.start_handle = 1;
	pdu_params.end_handle = 0xFFFF;
	p_pdu_params->attribute_type16 = GATT_ATT_TYPE_DEVICE_NAME;
	p_pdu_params->UUID_len = 2;//ATT_config.UUID_len;
	p_pdu_params->p_UUID = NULL;//ATT_config.UUID;
	LEatt_Encode_PDU(connection_handle,ATT_READ_BY_TYPE_REQUEST,p_pdu_params);		
}
/**************************************************************************
 * FUNCTION :- LEatt_Encode_PDU
 *
 * DESCRIPTION
 * This function is responsible for the encoding of ATT PDUs.
 *
 * GATT Server needs to support encode of the following PDUs
 *    ATT_ERROR_RESPONSE
 *    ATT_EXCHANGE_MTU_RESPONSE
 *    ATT_FIND_INFORMATION_RESPONSE
 *      ATT_READ_BY_GROUP_TYPE_RESPONSE
 *      ATT_READ_BY_TYPE_RESPONSE
 *      ATT_READ_BLOB_RESPONSE
 *      ATT_FIND_BY_TYPE_VALUE_RESPONSE
 *      ATT_READ_MULTIPLE_RESPONSE
 *      ATT_READ_RESPONSE
 *      ATT_PREPARE_WRITE_RESPONSE
 *      ATT_WRITE_RESPONSE
 *      ATT_EXECUTE_WRITE_RESPONSE
 *      ATT_HANDLE_VALUE_INDICATION
 *      ATT_HANDLE_VALUE_NOTIFICATION
 *
 * GATT Client needs to support encode of the following PDUs
 *    ATT_ERROR_REQUEST
 *    ATT_EXCHANGE_MTU_REQUEST
 *    ATT_FIND_INFORMATION_REQUEST
 *      ATT_READ_BY_GROUP_TYPE_REQUEST
 *      ATT_READ_BY_TYPE_REQUEST
 *      ATT_READ_BLOB_REQUEST
 *      ATT_FIND_BY_TYPE_VALUE_REQUEST
 *      ATT_READ_MULTIPLE_REQUEST
 *      ATT_READ_REQUEST
 *      ATT_PREPARE_WRITE_REQUEST
 *      ATT_WRITE_REQUEST
 *      ATT_EXECUTE_WRITE_REQUEST
 *    ATT_HANDLE_VALUE_CONFIRMATION
 **************************************************************************/

void LEatt_Encode_PDU(u_int16 connection_handle, u_int8 opcode,t_Att_pdu_params* p_pdu_params)
{
    u_int8 payload[530];
    u_int8* p_payload = payload;
    u_int8* p_pdu = payload;
    u_int16 length = 0;
    u_int16 i = 0;
    u_int8 pdu_length = 0,frame_length = 0;
    u_int8 att_data_llid = 0;
    t_q_descr* qd;
#if (PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED==1)
    u_int32 tx_pkt_count;
#endif

    p_payload+=4;
    p_payload[0] = opcode;


    // Only Set the Pending opcode for Requests
    if (((opcode & 0x01) == 0) || (opcode == ATT_HANDLE_VALUE_INDICATION ))
    {
        ATT_config.pending_ATT_opcode = opcode;
    }

    switch(opcode)
    {
#if (PRH_BS_CFG_SYS_LE_GATT_SERVER_SUPPORTED==1)
    case ATT_ERROR_RESPONSE :
        p_payload[1] = p_pdu_params->error_opcode;
        LMutils_Set_Uint16((p_payload+2),p_pdu_params->attribute_handle);
        p_payload[4] = p_pdu_params->error_code;
        length = 5;
        break;
#endif

    case ATT_EXCHANGE_MTU_REQUEST :
    case ATT_EXCHANGE_MTU_RESPONSE :
        LMutils_Set_Uint16((p_payload+1),p_pdu_params->mtu);
        length = 3;
        break;

#if (PRH_BS_CFG_SYS_LE_GATT_CLIENT_SUPPORTED==1)
    case ATT_FIND_INFORMATION_REQUEST :
    case ATT_FIND_BY_TYPE_VALUE_REQUEST :
    case ATT_READ_BY_GROUP_TYPE_REQUEST :
    case ATT_READ_BY_TYPE_REQUEST :
        LMutils_Set_Uint16((p_payload+1),p_pdu_params->start_handle);
        LMutils_Set_Uint16((p_payload+3),p_pdu_params->end_handle);
        if (opcode == ATT_FIND_INFORMATION_REQUEST)
        {
            length = 5;
        }
        else if (opcode == ATT_FIND_BY_TYPE_VALUE_REQUEST)
        {
            LMutils_Set_Uint16((p_payload+5),p_pdu_params->attribute_type16);
            length = 7 + p_pdu_params->attribute_value_len;
            /* The byte order of the Attribute Value is dictated by the Application */

            LMutils_Array_Copy(p_pdu_params->attribute_value_len,p_payload+7,p_pdu_params->attribute.p_value);

        }
        else
        {
            if (p_pdu_params->UUID_len == 0x02)
            {
                LMutils_Set_Uint16((p_payload+5),p_pdu_params->attribute_type16);
                length = 7;
            }
            else if (p_pdu_params->UUID_len == 16)
            {
                LMutils_Array_Copy(16,p_payload+5,p_pdu_params->p_UUID);
                length = 21;
            }
        }
        break;

#endif

#if (PRH_BS_CFG_SYS_LE_GATT_SERVER_SUPPORTED==1)
    case ATT_READ_BY_GROUP_TYPE_RESPONSE :
    case ATT_READ_BY_TYPE_RESPONSE :
    case ATT_FIND_INFORMATION_RESPONSE :

        if (opcode == ATT_FIND_INFORMATION_RESPONSE)
            p_payload[1] = p_pdu_params->format;
        else
            p_payload[1] = p_pdu_params->attribute_value_len; /* Handle+Value length */

        LMutils_Array_Copy(p_pdu_params->length,p_payload+2,p_pdu_params->p_result);

        length = 2 + p_pdu_params->length;
        break;

    case ATT_READ_BLOB_RESPONSE :
    case ATT_FIND_BY_TYPE_VALUE_RESPONSE :
    case ATT_READ_MULTIPLE_RESPONSE :
    case ATT_READ_RESPONSE :

        LMutils_Array_Copy(p_pdu_params->length,p_payload+1,p_pdu_params->p_result);

        length = 1 + p_pdu_params->length;
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_GATT_CLIENT_SUPPORTED==1)

    case ATT_READ_REQUEST :
        LMutils_Set_Uint16((p_payload+1),p_pdu_params->attribute.handle);
        length = 3;
        break;

    case ATT_READ_BLOB_REQUEST :
        LMutils_Set_Uint16((p_payload+1),p_pdu_params->attribute.handle);
        LMutils_Set_Uint16((p_payload+3),p_pdu_params->value_offset);
        length = 5;
        break;

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_READ_MULTIPLE==1)
    case ATT_READ_MULTIPLE_REQUEST :
        for (i=0;i<p_pdu_params->num_handles;i++)
            LMutils_Set_Uint16((p_payload+1+(i*2)),p_pdu_params->handle[i]);
        length = 1 + (p_pdu_params->num_handles*2);
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_PREPARE_WRITE==1)
    case ATT_EXECUTE_WRITE_REQUEST :
        p_payload[1] = p_pdu_params->flags;
        length = 2;
        break;


    case ATT_PREPARE_WRITE_REQUEST :
        LMutils_Set_Uint16((p_payload+1),p_pdu_params->attribute.handle);
        if ((ATT_config.sub_procedure == GATT_WRITE_LONG_CHARACTERISTIC) ||
            (ATT_config.sub_procedure == GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE_LONG) ||
            (ATT_config.sub_procedure == GATT_CHARACTERISTIC_VALUE_RELIABLE_WRITE))
        {
            LMutils_Set_Uint16((p_payload+3),p_pdu_params->value_offset);
            // GF 25 June 2012 -- Bypass fix for GAW_CL_BI27 - To invoke test it requires functionality
            // which would not normally be in a product..
#if (PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5 ==1)
            for (i=0;i<p_pdu_params->attribute.value_len;i++)
                p_payload[5+i] =  p_pdu_params->attribute.p_value[i];
#else
            for (i=0;i<p_pdu_params->attribute.value_len;i++)
                p_payload[5+i] =  ATT_config.value[ATT_config.value_offset+i];
#endif
            ATT_config.value_offset += p_pdu_params->attribute.value_len;
        }

        length = 5 + p_pdu_params->attribute.value_len;
        break;
#endif
#endif

#if (PRH_BS_CFG_SYS_LE_GATT_SERVER_SUPPORTED==1)
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_PREPARE_WRITE==1)
    case ATT_PREPARE_WRITE_RESPONSE :
        LMutils_Set_Uint16((p_payload+1),p_pdu_params->attribute.handle);
        LMutils_Set_Uint16((p_payload+3),p_pdu_params->value_offset);
        _Set_Attribute_Value((p_payload+5),p_pdu_params->attribute.p_value,p_pdu_params->attribute.value_len);
        length = 5 + p_pdu_params->attribute.value_len;
        break;
#endif
#endif

#if (PRH_BS_CFG_SYS_LE_GATT_SERVER_SUPPORTED==1)
    case ATT_WRITE_RESPONSE :
    case ATT_EXECUTE_WRITE_RESPONSE :
    case ATT_HANDLE_VALUE_CONFIRMATION :
        length = 1;
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_GATT_CLIENT_SUPPORTED==1)
    case ATT_WRITE_REQUEST :
    case ATT_HANDLE_VALUE_INDICATION :
    case ATT_HANDLE_VALUE_NOTIFICATION :
    case ATT_WRITE_COMMAND :
    case ATT_SIGNED_WRITE_COMMAND :
        LMutils_Set_Uint16((p_payload+1),p_pdu_params->attribute.handle);
        _Set_Attribute_Value((p_payload+3),p_pdu_params->attribute.p_value,p_pdu_params->attribute.value_len);
        length = 3 + p_pdu_params->attribute.value_len;
#if (PRH_BS_CFG_SYS_LE_SMP_DATA_SIGNING==1)
        if (opcode == ATT_SIGNED_WRITE_COMMAND)
        {
            // Create a Signature !
            LEsmp_Create_Signature(payload,length,p_pdu_params->signature);
            _Set_Attribute_Signature((payload+3+p_pdu_params->attribute.value_len),p_pdu_params->signature);
            length += 12;
        }
        else
#endif
        {
            ATT_config.start_handle = p_pdu_params->attribute.handle;
        }
        break;
#endif
    }

    if (length > 0 )
    {
        // Find the relevant device index
        t_LE_Connection* p_connection = LEconnection_Find_P_Connection(connection_handle);
        u_int8 device_index = p_connection->device_index;

        // Note :- Should only set the GATT timer for Requests NOT Responses
        // The lsb of the opcode = 0 for Requests. 1 for responses
        // Also should not set the timer for an ATT_WRITE_COMMAND

        if ( (((opcode & 0x01) == 0x0) || (opcode == ATT_HANDLE_VALUE_INDICATION)) &&
             (opcode != ATT_WRITE_COMMAND))
            _GATT_ReSet_GATT_Timer(p_connection);

        // L2CAP Length Field
        LMutils_Set_Uint16(p_pdu,length);

        // GATT CID Field
        LMutils_Set_Uint16((p_pdu+2),0x0004);

#if (PRH_BS_CFG_SYS_TRACE_ATT_VIA_HCI_SUPPORTED==1)
        TCeg_Send_LE_GATT_Event(p_payload, (u_int8)length, 0 /*Transmitted*/);
#endif
        length+=4;

        while(length > 0 )
        {
            uint8 frame_p_pdu[31];
            if(length > 27)
            {
                frame_length = 27;
                length -= frame_length;
            }
            else
            {
                frame_length = length;
                length = 0;
            }

            pdu_length = frame_length;
            LMutils_Array_Copy(frame_length, frame_p_pdu, p_pdu);

#if (PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED==1)
            tx_pkt_count = p_connection->enc_tx_pkt_counter;
            if (LEconnection_Encrypt_Payload(p_connection,frame_p_pdu,pdu_length,(att_data_llid==1)?(LCH_continue):(LCH_start),tx_pkt_count))
            {
                pdu_length += 4;
            }
#endif
            qd = BTq_Enqueue(L2CAP_OUT_Q, device_index, (t_length) pdu_length);
            if(qd)
            {
                if(att_data_llid==1)
                    qd->message_type = LCH_continue;
                else
                    qd->message_type = LCH_start;
#if (PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED==1)
                if(p_connection->current_security_state & LE_ENCRYPTION_ACTIVE)
                {
                    qd->encrypt_status = LE_DATA_ENCRYPTED;
                    qd->enc_pkt_count = tx_pkt_count;
                    p_connection->enc_tx_pkt_counter++;
                }
#endif
                /*
                 * Presented with fully encoded PDU then encode directly
                 */
                LMutils_Array_Copy(pdu_length, qd->data,frame_p_pdu);
                BTq_Commit(L2CAP_OUT_Q, device_index);
            }
            p_pdu += frame_length; 
            if(length)
            {
                att_data_llid = 1;
            }
        }
    }
}


/**************************************************************************
 * FUNCTION :- LEatt_Decode_PDU
 *
 * DESCRIPTION
 * This function is responsible for the decoding of ATT PDUs.
 *
 * GATT Client needs to support decode of the following PDUs
 *    ATT_ERROR_RESPONSE
 *    ATT_EXCHANGE_MTU_RESPONSE
 *    ATT_FIND_INFORMATION_RESPONSE
 *      ATT_READ_BY_GROUP_TYPE_RESPONSE
 *      ATT_READ_BY_TYPE_RESPONSE
 *      ATT_READ_BLOB_RESPONSE
 *      ATT_FIND_BY_TYPE_VALUE_RESPONSE
 *      ATT_READ_MULTIPLE_RESPONSE
 *      ATT_READ_RESPONSE
 *      ATT_PREPARE_WRITE_RESPONSE
 *      ATT_WRITE_RESPONSE
 *      ATT_EXECUTE_WRITE_RESPONSE
 *      ATT_HANDLE_VALUE_INDICATION
 *      ATT_HANDLE_VALUE_NOTIFICATION
 *
 * GATT Server needs to support decode of the following PDUs
 *    ATT_ERROR_REQUEST
 *    ATT_EXCHANGE_MTU_REQUEST
 *    ATT_FIND_INFORMATION_REQUEST
 *      ATT_READ_BY_GROUP_TYPE_REQUEST
 *      ATT_READ_BY_TYPE_REQUEST
 *      ATT_READ_BLOB_REQUEST
 *      ATT_FIND_BY_TYPE_VALUE_REQUEST
 *      ATT_READ_MULTIPLE_REQUEST
 *      ATT_READ_REQUEST
 *      ATT_PREPARE_WRITE_REQUEST
 *      ATT_WRITE_REQUEST
 *      ATT_EXECUTE_WRITE_REQUEST
 *    ATT_HANDLE_VALUE_CONFIRMATION
 **************************************************************************/

void LEatt_Decode_PDU(t_LE_Connection* p_connection ,u_int16 mtu_length,u_int8* p_pdu)
{
    u_int8 opcode = p_pdu[0]; // & 0x3F;
    t_Att_pdu_params  pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 att_status=0;
    u_int8 result[530];
    u_int8 result_len=0;
#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_PREPARE_WRITE==1)
    u_int8 flags;
#endif
#if ((PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_PREPARE_WRITE==1) || (PRH_BS_CFG_SYS_LE_SERVER_READ_BLOB_SUPPORTED==1))
    u_int16 offset;
#endif
    u_int16 max_mtu;
    u_int16 connection_handle;
    u_int16 start_handle = HCIparam_Get_Uint16(&p_pdu[1]);
    u_int16 end_handle = HCIparam_Get_Uint16(&p_pdu[3]);
    u_int16 attrib_type = HCIparam_Get_Uint16(&p_pdu[5]);


    connection_handle = LEconnection_Determine_Connection_Handle(p_connection);

    ATT_config.connection_handle = connection_handle;


#if (PRH_BS_CFG_SYS_TRACE_ATT_VIA_HCI_SUPPORTED==1)
    /* LOG at this point before Encryption.
     * Log this message over the TCI if this feature is active
     */
    TCeg_Send_LE_GATT_Event(p_pdu, (u_int8)(mtu_length-4), 1 /*Transmitted*/);
#endif

    if ((opcode & 0x01) && // This is a response PDU
        ((opcode-1) == ATT_config.pending_ATT_opcode))
    {
        p_connection->GATT_timer = BT_TIMER_OFF;
        ATT_config.pending_ATT_opcode = 0x00;
    }
#if 0
	DEBUG_MSG(0x3A);
	DEBUG_MSG(opcode);
    //os_printf("LEatt_Decode_PDU opcode:0x%.2x,mtu_length:%x\r\n",opcode,mtu_length);
	if(p_connection->connInterval_org == p_connection->connInterval)
	{
		//LE_App_Set_L2cap_Para_Update_Timer(HW_Get_Native_Clk_Avoid_Race() + 3200);
		DEBUG_MSG(0x3D);
	}
#endif

    switch(opcode)
    {
    case ATT_ERROR_RESPONSE :
        att_status = p_pdu[4];
        p_connection->GATT_timer = BT_TIMER_OFF;
        switch(p_pdu[1])
        {
        case ATT_FIND_BY_TYPE_VALUE_REQUEST:
            if (att_status == 0x0A /* Attribute Not Found */)
                att_status = NO_ERROR;
            if (ATT_config.sub_procedure == GATT_DISCOVER_SPECIFIC_SERVICE)
                GATT_Command_Complete_Event(connection_handle,(u_int8)LE_GATT_CLIENT_PRIMARY_SERVICE_DISCOVERY,att_status);
            break;

        case ATT_READ_BY_TYPE_REQUEST :
            if (ATT_config.sub_procedure == GATT_FIND_INCLUDED_SERVICES)
            {
                if (att_status == 0x0A /* Attribute Not Found */)
                    att_status = NO_ERROR;
                GATT_Command_Complete_Event(connection_handle,(u_int8)LE_GATT_CLIENT_RELATIONSHIP_DISCOVERY,att_status);
            }
            else if ((ATT_config.sub_procedure == GATT_DIS_ALL_CHARS_OF_SERVICE) ||
                     (ATT_config.sub_procedure == GATT_DISCOVER_CHARACTERISTICS_BY_UUID))
            {
                if (att_status == 0x0A /* Attribute Not Found */)
                    att_status = NO_ERROR;

                GATT_Command_Complete_Event(connection_handle,(u_int8)LE_GATT_CLIENT_CHARACTERISTIC_DISCOVERY,att_status);
            }
            else if (ATT_config.sub_procedure == GATT_READ_USING_CHARACTERISTIC_UUID )
            {
                // Important that this returns "Attribute Not Found" (0x0A) in the event.
                // See Conformance test - GAR_CL_BI07
                GATT_Command_Complete_Event(connection_handle,(u_int8)LE_GATT_CLIENT_CHARACTERISTIC_VALUE_READ,att_status);
            }

            break;

        case ATT_FIND_INFORMATION_REQUEST :
        case ATT_READ_BY_GROUP_TYPE_REQUEST :
            {
                u_int16 event_id = 0;

                if (att_status == 0x0A /* Attribute Not Found */)
                    att_status = NO_ERROR;

                if(ATT_config.sub_procedure == GATT_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS)
                {
                    event_id = LE_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY;
                }
                else if (ATT_config.sub_procedure == GATT_DISCOVER_ALL_SERVICES)
                {
                    event_id = LE_GATT_CLIENT_PRIMARY_SERVICE_DISCOVERY;
                }

                GATT_Command_Complete_Event(connection_handle,event_id,att_status);
            }
            break;

        case ATT_READ_BLOB_REQUEST :
        case ATT_READ_MULTIPLE_REQUEST :
        case ATT_READ_REQUEST :
            GATT_Event(GATT_READ_RESPONSE_EVENT,connection_handle, 0x06 /* Dummy L2CAP Header */, 0x00,0x00,att_status);
            break;

        case ATT_EXCHANGE_MTU_REQUEST :
            GATT_Command_Complete_Event(connection_handle,(u_int8)LE_GATT_CLIENT_EXCHANGE_MTU,att_status);
            break;

        case ATT_WRITE_REQUEST :
        case ATT_PREPARE_WRITE_REQUEST :
        case ATT_EXECUTE_WRITE_REQUEST :
            GATT_Event(GATT_WRITE_COMPLETE_EVENT,connection_handle,0x00,0x00,0x00, att_status);
            break;

        }///switch(p_pdu[1]) for ATT_ERROR_RESPONSE case 
        break;

    case ATT_EXCHANGE_MTU_REQUEST :
		#ifdef LE_DEBUG_ENABLE 
		os_printf("ATT_EXCHANGE_MTU_REQUEST:%d\r\n",connection_handle);
		#endif
        // This is only applicable on the Server side.
        // We Extract the Clients - MTU
        max_mtu = HCIparam_Get_Uint16(&p_pdu[1]);
        // Encode our Max MTU and send to peer.

        Gatt_Set_MTU_Size(160);  //max:512    can modified by user
        p_pdu_params->mtu = ATT_config.MTU_LEN;
        LEatt_Encode_PDU(connection_handle,ATT_EXCHANGE_MTU_RESPONSE,p_pdu_params);

        if (max_mtu < ATT_config.MTU_LEN)
        {
            ATT_config.MTU_LEN = max_mtu;
        }
        break;

    case ATT_EXCHANGE_MTU_RESPONSE :
        if (ATT_config.pending_ATT_opcode==ATT_EXCHANGE_MTU_REQUEST)
        {
            max_mtu = _Get_Uint16((p_pdu+1));
            if (max_mtu < ATT_config.MTU_LEN)
            {
                ATT_config.MTU_LEN = max_mtu;
            }
            // Raise an event !!
            GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_EXCHANGE_MTU&0xFF),att_status);
        }
        break;

    case ATT_FIND_INFORMATION_REQUEST :
        {
            p_pdu_params->format = 0;

            if((start_handle <= end_handle) && (start_handle != 0x0000))
            {
                att_status = GATTserver_Find_Descriptor_Information(ATT_config.MTU_LEN-2, start_handle, end_handle, result, &result_len, &p_pdu_params->format);
                if (att_status == 0)
                {
                    /* Build up the response PDU */
                    // Format is a parameter of the GATTserver_Find_Descriptor_Information
                    //p_pdu_params->format = 0x01;
                    p_pdu_params->length = result_len;
                    p_pdu_params->p_result = result;
                    LEatt_Encode_PDU(connection_handle,ATT_FIND_INFORMATION_RESPONSE,p_pdu_params);
                }
            }
            else
            {
                att_status = LE_ATT_INVALID_HANDLE;
            }

            if (att_status != 0)
            {
                /* Send Error Response with the «Invalid Handle?error code. The Attribute Handle In
                 * Error parameter shall be set to the Starting Handle parameter.
                 */
                p_pdu_params->error_opcode = ATT_FIND_INFORMATION_REQUEST;
                p_pdu_params->attribute_handle = start_handle;
                p_pdu_params->error_code = att_status;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
            }

            if(end_handle == Get_GATTserver_DB_Last_Handle())
            {
                GATT_Event(GATT_INFORMATION_READ_COMPLETE_EVENT,connection_handle, mtu_length, 0x0, 0x0,NO_ERROR);
            }
        }
        break;

    case ATT_FIND_INFORMATION_RESPONSE :
        {
            u_int8 attrib_len;
            u_int8  format = p_pdu[1];
            u_int16 last_handle;
            t_Att_pdu_params pdu_params;
            t_Att_pdu_params* p_pdu_params = &pdu_params;
            u_int8 opcode;

            if (format == 0x01)
                attrib_len = 4;
            else if (format == 0x02)
                attrib_len = 18;
            else
                return; // Add error handling here for wrong format info.

            last_handle = _Att_Get_Last_Handle(ATT_FIND_INFORMATION_RESPONSE,&p_pdu[2],mtu_length,attrib_len);

            GATT_Event(GATT_DISCOVER_DESCRIPTORS_EVENT,connection_handle, mtu_length,  attrib_len, &p_pdu[2],NO_ERROR);

            if (last_handle != ATT_config.end_handle)
            {
                opcode = ATT_FIND_INFORMATION_REQUEST;
                p_pdu_params->start_handle = last_handle+1;
                p_pdu_params->end_handle = ATT_config.end_handle;
                LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
            }
            else
            {
                if (last_handle < ATT_config.start_handle )
                {
                    att_status = GATT_ERROR_INVALID_PDU;
                }
                GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERY & 0xFF),att_status);
            }
        }
        break;


    case ATT_FIND_BY_TYPE_VALUE_REQUEST :
        {
            u_int16 attrib_value = HCIparam_Get_Uint16(&p_pdu[7]);

            if((start_handle > end_handle) || (start_handle==0x0000))
            {
                att_status = LE_ATT_INVALID_HANDLE;
            }
            else
            {
                att_status = GATTserver_Find_By_Type_And_Value((ATT_config.MTU_LEN-1),start_handle,end_handle,
                                                               attrib_type,attrib_value,result,&result_len);

                if (att_status == 0)
                {
                    p_pdu_params->length = result_len;
                    p_pdu_params->p_result = result;
                    LEatt_Encode_PDU(connection_handle,ATT_FIND_BY_TYPE_VALUE_RESPONSE,p_pdu_params);
                }
            }
            if (att_status != 0)
            {
                p_pdu_params->error_opcode = ATT_FIND_BY_TYPE_VALUE_REQUEST;
                p_pdu_params->attribute_handle = start_handle;
                p_pdu_params->error_code = att_status;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
            }
        }
        break;

    case ATT_FIND_BY_TYPE_VALUE_RESPONSE :

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_DISCOVERY_BY_UUID==1)
        if (ATT_config.sub_procedure == GATT_DISCOVER_SPECIFIC_SERVICE)
        {
            u_int8* p_handle_info_ist = p_pdu +1;
            u_int16 last_handle = _Att_Get_Last_Handle(ATT_FIND_BY_TYPE_VALUE_RESPONSE,&p_pdu[1],mtu_length,0x04);
            GATT_Event(GATT_DISCOVER_SERVICES_BY_UUID_EVENT, connection_handle,mtu_length,0x04,p_handle_info_ist,NO_ERROR);

            /* If the attribute type in the Find By Type Value Request is not a grouping attribute
             * as defined by a higher layer specification, the Group End Handle shall be equal to the
             * Found Attribute Handle. If no other attributes with the same attribute type exist after
             *  the Found Attribute Handle, the End Found Handle shall be set to 0xFFFF.
             */

            if (last_handle != 0xFFFF)
            {
                /*
                 * Raise an event. With the data from peer. And construct a new
                 * request for the peed
                 */

                opcode = ATT_FIND_BY_TYPE_VALUE_REQUEST;
                pdu_params.start_handle = last_handle+1;
                pdu_params.end_handle = 0xFFFF;
                pdu_params.attribute_type16 = GATT_ATT_TYPE_PRIMARY_SERVICE;

                // Need to consider 16 Byte UUIDs

                pdu_params.attribute_value_len = ATT_config.UUID_len;
                pdu_params.attribute.p_value = ATT_config.UUID;
                LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
            }
            else
            {
                GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_PRIMARY_SERVICE_DISCOVERY &0xFF),NO_ERROR);
            }
        }
#endif
        break;

    case ATT_READ_BY_TYPE_REQUEST :
        {
            u_int8 attrib_len = 0;

            if((start_handle > end_handle) || (start_handle==0x0000))
            {
                att_status = LE_ATT_INVALID_HANDLE;
            }
            else
            {
                if (attrib_type == GATT_ATT_TYPE_INCLUDE)
                {
                    att_status =  GATTserver_Find_Included_Services((ATT_config.MTU_LEN-2), start_handle, end_handle, result,&result_len);
                    attrib_len = 8;
                }
                else if (attrib_type == GATT_ATT_TYPE_CHARACTERISTIC)
                {
                    att_status =  GATTserver_Discover_All_Characteristics_Of_A_Service((ATT_config.MTU_LEN-2), start_handle, end_handle, result,&result_len,&attrib_len);
                }
                else  /* Read by Type with a Characteristic UUID */
                {
                    t_attrib_UUID attrib_UUID;

                    if (mtu_length == 0x0B)
                    {
                        attrib_UUID.len = 0x02;
                        attrib_UUID.UUID_val = attrib_type;
                    }
                    else if (mtu_length == 0x19)
                    {
                        attrib_UUID.len = 0x10;
                        attrib_UUID.p_UUID = &p_pdu[5];
                    }

                    att_status = GATTserver_Read_Characteristic_Value_By_UUID((ATT_config.MTU_LEN-2), start_handle, end_handle,&attrib_UUID, result,&result_len,&attrib_len);
                }
                if (att_status == 0)
                {
                    p_pdu_params->p_result = result;
                    p_pdu_params->length = result_len;
                    p_pdu_params->attribute_value_len = attrib_len;
                    LEatt_Encode_PDU(connection_handle,ATT_READ_BY_TYPE_RESPONSE,p_pdu_params);
                }
            }
            if (att_status != 0)
            {
                p_pdu_params->error_opcode = ATT_READ_BY_TYPE_REQUEST;
                p_pdu_params->attribute_handle = GATTserver_Handle_in_Error;
                p_pdu_params->error_code = att_status;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
            }
        }
        break;

    case ATT_READ_BY_TYPE_RESPONSE :
        {
            u_int8 attrib_len = p_pdu[1];

            u_int16 last_handle = _Att_Get_Last_Handle(ATT_READ_BY_TYPE_RESPONSE,&p_pdu[2],mtu_length,attrib_len);

            if (ATT_config.sub_procedure == GATT_FIND_INCLUDED_SERVICES)
            {
                GATT_Event(GATT_DISCOVER_INCLUDED_SERVICES_EVENT,connection_handle, mtu_length, attrib_len, &(p_pdu[2]),NO_ERROR);
                if (last_handle != ATT_config.end_handle)
                {

                    /*
                     * Raise an event. With the data from peer. And construct a new
                     * request for the peer
                     */

                    opcode = ATT_READ_BY_TYPE_REQUEST;
                    pdu_params.start_handle = last_handle+1;
                    pdu_params.end_handle = ATT_config.end_handle;
                    pdu_params.attribute_type16 = GATT_ATT_TYPE_INCLUDE;
                    pdu_params.UUID_len = 2;
                    LEatt_Encode_PDU(connection_handle,ATT_READ_BY_TYPE_REQUEST,p_pdu_params);
                }
                else
                {
                    GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_RELATIONSHIP_DISCOVERY&0xFF),NO_ERROR);
                }
            }
            else if (ATT_config.sub_procedure == GATT_READ_USING_CHARACTERISTIC_UUID)
            {
                GATT_Event(GATT_READ_BY_TYPE_RESPONSE_EVENT, connection_handle,mtu_length, attrib_len, &(p_pdu[2]),NO_ERROR);

                if (last_handle != ATT_config.end_handle)
                {
                    opcode = ATT_READ_BY_TYPE_REQUEST;
                    pdu_params.start_handle = last_handle+1;
                    pdu_params.end_handle = ATT_config.end_handle;
                    p_pdu_params->p_UUID =  ATT_config.UUID;
                    p_pdu_params->UUID_len = ATT_config.UUID_len;
                    if (ATT_config.UUID_len==0x02)
                        p_pdu_params->attribute_type16 = ATT_config.characteristic_16bit_UUID;
                    LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
                }
                else
                {
                    GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_CHARACTERISTIC_VALUE_READ&0xFF),NO_ERROR);
                }

            }
            else if ((ATT_config.sub_procedure == GATT_DIS_ALL_CHARS_OF_SERVICE) ||
                     (ATT_config.sub_procedure == GATT_DISCOVER_CHARACTERISTICS_BY_UUID))
            {
#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_DISCOVERY_BY_UUID==1)
                if  (ATT_config.sub_procedure == GATT_DISCOVER_CHARACTERISTICS_BY_UUID)
                {
                    u_int8 attrib_val_array[30];
                    u_int8 num_chars_found;

                    // This function searchs the incoming PDU contents for a match to the requested Characterisic UUID
                    // if a match is found it adds it to the attrib_val_array[..].

                    //*************************************
                    // TO DO -- 05 December
                    // This does not support 16 Byte UUIDs
                    // Check ATT_config.UUID_len
                    //*************************************
                    num_chars_found = _GATT_Find_Characteric_UUID_In_Payload( &(p_pdu[2]),attrib_val_array, attrib_len, mtu_length,  ATT_config.UUID, ATT_config.UUID_len);

                    if (num_chars_found)
                    {
                        u_int8 length = (attrib_len * num_chars_found) + 6;

                        GATT_Event(GATT_DISCOVER_CHARACTERISTICS_EVENT,connection_handle, length, attrib_len, attrib_val_array,NO_ERROR);
                    }
                }
                else
#endif
                {
                    GATT_Event(GATT_DISCOVER_CHARACTERISTICS_EVENT,connection_handle, mtu_length, attrib_len, &(p_pdu[2]),NO_ERROR);
                }

                if (last_handle != ATT_config.end_handle)
                {
                    opcode = ATT_READ_BY_TYPE_REQUEST;
                    pdu_params.start_handle = last_handle+1;
                    pdu_params.end_handle = ATT_config.end_handle;
                    p_pdu_params->attribute_type16 = GATT_ATT_TYPE_CHARACTERISTIC;
                    p_pdu_params->UUID_len = ATT_config.UUID_len;
                    p_pdu_params->p_UUID = ATT_config.UUID;
                    LEatt_Encode_PDU(connection_handle,ATT_READ_BY_TYPE_REQUEST,p_pdu_params);
                }
                else
                {
                    GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_CHARACTERISTIC_DISCOVERY&0xFF),NO_ERROR);
                }
            }
        }
        break;

    case ATT_READ_REQUEST :
        {
            att_status = GATTserver_Read_Characteristic_Value((ATT_config.MTU_LEN-1),start_handle,result, &result_len);
	        #ifdef LE_DEBUG_ENABLE
            	//os_printf("ATT_READ_REQUEST  att_status:0x%.2x\r\n",att_status);	
            #endif

            if (att_status != 0)
            {
                // Insufficient Authorization OR Insufficient Authentication OR Insufficient Encr Key Size
                // Insufficient Encryption OR Invalid Handle OR Read Not Permitted
                
                p_pdu_params->error_opcode = ATT_READ_REQUEST;
                p_pdu_params->attribute_handle = start_handle;
                p_pdu_params->error_code = att_status;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);

            }
            else
            {
                p_pdu_params->length = result_len;
                p_pdu_params->p_result = result;
                LEatt_Encode_PDU(connection_handle,ATT_READ_RESPONSE,p_pdu_params);
            }
        }
        break;

    case ATT_READ_RESPONSE :

        if ((ATT_config.sub_procedure == GATT_READ_CHARACTERISTIC_VALUE) ||
            (ATT_config.sub_procedure == GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ_SHORT))

        {
            if ((mtu_length-4) != ATT_config.MTU_LEN)
            {
                GATT_Event(GATT_READ_RESPONSE_EVENT, connection_handle, mtu_length, 0x00,&(p_pdu[1]),NO_ERROR);
            }
            else
            {
                u_int8* p_result = (p_pdu+1);
                // Copy the attribute data into the ATT_config and update the offset.
                if (ATT_config.sub_procedure == GATT_READ_CHARACTERISTIC_VALUE)
                    ATT_config.sub_procedure = GATT_READ_LONG_CHARACTERISTIC_VALUES;
                else if (ATT_config.sub_procedure == GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ_SHORT)
                    ATT_config.sub_procedure = GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ_LONG;
                ATT_config.value_offset = 0;

                LMutils_Array_Copy((mtu_length-5),ATT_config.value+ATT_config.value_offset,p_result);

                ATT_config.value_offset += (mtu_length-5);

                {
                    // Need to send more BLOB Requests
                    opcode = ATT_READ_BLOB_REQUEST;
                    p_pdu_params->attribute.handle = ATT_config.characteristic_Handle;
                    p_pdu_params->value_offset = ATT_config.value_offset;
                    LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
                }

            }

        }
        break;
#if (PRH_BS_CFG_SYS_LE_SERVER_READ_BLOB_SUPPORTED==1)
    case ATT_READ_BLOB_REQUEST :
        {

            offset = HCIparam_Get_Uint16(&p_pdu[3]);

            att_status = GATTserver_Read_Long_Characteristic_Value_Offset( (ATT_config.MTU_LEN-1), start_handle, offset, result, &result_len);

            if (att_status != 0x00)
            {
                // Insufficient Authorization OR Insufficient Authentication OR Insufficient Encr Key Size
                // Insufficient Encryption OR Invalid Handle OR Read Not Permitted
                p_pdu_params->error_opcode = ATT_READ_BLOB_REQUEST;
                p_pdu_params->attribute_handle = start_handle;
                p_pdu_params->error_code = att_status;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
            }
            else
            {
                //
                // Set the Characteristic Value in the ATT_config structure so that it does not have
                // to be retrieved again.

                p_pdu_params->length = result_len;
                p_pdu_params->p_result = (u_int8*)result;
                LEatt_Encode_PDU(connection_handle,ATT_READ_BLOB_RESPONSE,p_pdu_params);

            }
        }
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_CLIENT_READ_BLOB_SUPPORTED==1)
    case ATT_READ_BLOB_RESPONSE :
        if ((ATT_config.sub_procedure == GATT_READ_LONG_CHARACTERISTIC_VALUES) ||
            (ATT_config.sub_procedure == GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ_LONG))
        {
            u_int8* p_result = (p_pdu+1);
            // Copy the attribute data into the ATT_config and update the offset.

            LMutils_Array_Copy((mtu_length-5),ATT_config.value+ATT_config.value_offset,p_result);

            ATT_config.value_offset += (mtu_length-5);

            if ((mtu_length-5) != (ATT_config.MTU_LEN-1))
            {
                // Looks like there was BUG here -- need to set param 3 = ATT_config.value_offset +    6/5
                //                GATT_Event(GATT_READ_LONG_RESPONSE_EVENT, ATT_config.characteristic_Handle, (u_int8)ATT_config.value_offset, 0x00,ATT_config.value,NO_ERROR);
                GATT_Event(GATT_READ_RESPONSE_EVENT, connection_handle, (u_int8)ATT_config.value_offset+5, 0x00,ATT_config.value,NO_ERROR);

            }
            else
            {
                // Need to send more BLOB Requests
                opcode = ATT_READ_BLOB_REQUEST;
                p_pdu_params->attribute.handle = ATT_config.characteristic_Handle;
                p_pdu_params->value_offset = ATT_config.value_offset;
                LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
            }
        }
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_READ_MULTIPLE==1)
    case ATT_READ_MULTIPLE_REQUEST :
        {
            u_int8 response[23]; // Proper size of Response yet to be determined
            u_int16 handle = 0xFFFF;
            u_int8* p_result = response;
            u_int8 total_result_len=0;
            u_int16 max_result_len = (ATT_config.MTU_LEN-1);
            u_int8 i=0;
            u_int8 status = 0;
            p_pdu++;

            while ((i < (mtu_length-5)) && (status == 0))
            {
                handle = HCIparam_Get_Uint16(&p_pdu[i]);

                // Is the following function really needed --- can a more generic functio be useed.
                status = GATTserver_Read_Characteristic_Value(max_result_len, handle, p_result, &result_len);

                if (status == 0x00)
                {
                    p_result += result_len;
                    max_result_len -= result_len;
                    total_result_len += result_len;
                }
                i+=2;
            }
            if (status == 0x00)
            {
                // Encode an ATT_READ_MULTIPLE_RESPONSE
                p_pdu_params->p_result = response;
                p_pdu_params->length = total_result_len;
                LEatt_Encode_PDU(connection_handle,ATT_READ_MULTIPLE_RESPONSE,p_pdu_params);
            }
            else
            {
                p_pdu_params->error_opcode = ATT_READ_MULTIPLE_REQUEST;
                p_pdu_params->attribute_handle = handle;
                p_pdu_params->error_code = status;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
            }
        }
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_READ_MULTIPLE==1)
    case ATT_READ_MULTIPLE_RESPONSE :
        if (ATT_config.sub_procedure == GATT_READ_MULTIPLE_CHARACTERISTIC_VALUES)
        {
            GATT_Event(GATT_READ_RESPONSE_EVENT, connection_handle, mtu_length, 0x00,&(p_pdu[1]),NO_ERROR);
        }
        break;
#endif

    case ATT_READ_BY_GROUP_TYPE_REQUEST :
        /* The starting handle shall be less than or equal to the ending handle. If a server
         * receives a Read By Group Type Request with the Starting Handle parameter
         * greater than the Ending Handle parameter or the Starting Handle parameter is
         * 0x0000, an Error Response shall be sent with the «Invalid Handle?error code;
         * The Attribute Handle In Error parameter shall be set to the Starting Handle
         * parameter.
         *
         * If the Attribute Group Type is not a supported grouping attribute as defined by a
         * higher layer specification then an Error Response shall be sent with the error
         * code «Unsupported Group Type? The Attribute Handle In Error parameter
         * shall be set to the Starting Handle.
         *
         * If no attribute with the given type exists within the handle range, then no attribute
         * handle and value will be returned, and an Error Response shall be sent
         * with the error code «Attribute Not Found? The Attribute Handle In Error
         * parameter shall be set to the starting handle.
         */

        {
            if  ((start_handle > end_handle) || (start_handle==0x0000))
            {
                /* Send Error Response with the «Invalid Handle?error code. The Attribute Handle In
                 * Error parameter shall be set to the Starting Handle parameter.
                 */
                att_status =  LE_ATT_INVALID_HANDLE;
            }
            else
            {
                u_int8 pair_len = 0;
                att_status = GATTserver_Read_By_Group_Type((ATT_config.MTU_LEN-2), start_handle, end_handle,attrib_type ,
                                                           result, &result_len, &pair_len);
                
                if (att_status == 0)
                {
                  
                    if(pair_len == 0x10)//for 128 bits UUID modify
					{
						p_pdu_params->attribute_value_len = 20;
                    }
                    else
                    {
                        p_pdu_params->attribute_value_len = 6;
                    }
                    p_pdu_params->length = result_len;
                    p_pdu_params->p_result = result;
                    LEatt_Encode_PDU(connection_handle,ATT_READ_BY_GROUP_TYPE_RESPONSE,p_pdu_params);
                }
            }

            if (att_status != 0)
            {
               
                p_pdu_params->error_opcode = ATT_READ_BY_GROUP_TYPE_REQUEST;
                p_pdu_params->attribute_handle = start_handle;
                p_pdu_params->error_code = att_status;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
            }
        }
        break;

    case ATT_READ_BY_GROUP_TYPE_RESPONSE :

        if (ATT_config.sub_procedure == GATT_DISCOVER_ALL_SERVICES)
        {
            u_int8 attrib_len;
            u_int16 last_handle;

            /*
             * Need to add error handle code to ensure not End Handle is less than the Start handle
             */

            attrib_len = p_pdu[1];

            last_handle = _Att_Get_Last_Handle(ATT_READ_BY_GROUP_TYPE_RESPONSE,&p_pdu[2],mtu_length,attrib_len);

            GATT_Event(GATT_DISCOVER_SERVICES_EVENT, connection_handle,mtu_length,attrib_len,&p_pdu[2],NO_ERROR);

            if (last_handle != 0xFFFF)
            {
                /*
                 * Raise an event. With the data from peer. And construct a new
                 * request for the peed
                 */

                opcode = ATT_READ_BY_GROUP_TYPE_REQUEST;
                p_pdu_params->start_handle = last_handle+1;
                p_pdu_params->end_handle = 0xFFFF;
                p_pdu_params->UUID_len = 0x02;
                p_pdu_params->attribute_type16 = GATT_ATT_TYPE_PRIMARY_SERVICE;
                LEatt_Encode_PDU(16 /*connection_handle*/,ATT_READ_BY_GROUP_TYPE_REQUEST,p_pdu_params);

            }
            else
            {
                GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_PRIMARY_SERVICE_DISCOVERY&0xFF),NO_ERROR);
            }
        }
        break;

    case ATT_WRITE_REQUEST :
        {
            u_int8* p_attrib_value = (p_pdu+3);

            att_status = GATTserver_Write_Command(start_handle,  p_attrib_value,(mtu_length-7));

					
            if(att_status == GATT_SERVER_HANDLE_WRITE_RESP /* Special Exception for write to Control points  */)
            {
                // No Nothing let the Server handle the Write Response...
                // Otherwise the ATT_Indication will be sent before the Write_Response

            }
            else if (att_status != 0)
            {
                p_pdu_params->error_opcode = ATT_WRITE_REQUEST;
                p_pdu_params->attribute_handle = start_handle;
                p_pdu_params->error_code = att_status;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
            }
            else
            {
                LEatt_Encode_PDU(connection_handle,ATT_WRITE_RESPONSE,p_pdu_params);
            }
        }
        break;

    case ATT_WRITE_RESPONSE :

        if ((ATT_config.sub_procedure == GATT_WRITE_CHARACTERISTIC) ||
            (ATT_config.sub_procedure == GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE_SHORT))
        {
            GATT_Event(GATT_WRITE_COMPLETE_EVENT, connection_handle, 0x00, 0x00, 0x00, NO_ERROR);
        }
        break;
#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_PREPARE_WRITE==1)
    case ATT_PREPARE_WRITE_REQUEST :
        {
            u_int8* p_data = (p_pdu+5);
            u_int8 status;
            u_int8 i;

            offset = HCIparam_Get_Uint16(&p_pdu[3]);
            status = 0x00;

            if (offset == 0x00)
                ATT_config.characteristic_Handle = start_handle;

            else if (start_handle != ATT_config.characteristic_Handle)
            {
                // Error - change on Characteristic Handle before
                // write is complete.
            }
            {
                u_int8 max_write_len = GATTserver_Get_Value_Length(start_handle);

                //
                // First Find the Max Length of the Characteristic Value identified by the handle.

                status = GATTserver_Can_I_Write(start_handle);

                if ((status==0x00) &&((mtu_length-9+offset) > max_write_len))
                {
                    //status = GATT_ERROR_INVALID_OFFSET;/* TP/GAW/SR/BI-33 GATT Layer remove this line*/
									  
                }
            }

            if (status)
            {
                p_pdu_params->error_opcode = ATT_PREPARE_WRITE_REQUEST;
                p_pdu_params->attribute_handle = start_handle;
                p_pdu_params->error_code = status;
                ATT_config.last_prepare_write_error = status;
                ATT_config.characteristic_Handle = start_handle;
                ATT_config.prepared_write_pending = 0x00;
                LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
            }
            else
            {
                // Check If the offset is too large !!
                // TO DO TODO !!!!!!
                //*****************************************

                // Copy the data to the ATT_config.
                for(i=0; i< (mtu_length-9); i++)
                {
                    ATT_config.value[i+offset/*+ATT_config.value_offset*/] = p_data[i];
                }

                ATT_config.value_offset = (mtu_length-9)+offset;
                ATT_config.prepared_write_pending = 0x01;
                p_pdu_params->attribute.handle = start_handle;
                p_pdu_params->value_offset = offset;
                p_pdu_params->attribute.p_value = p_data;
                p_pdu_params->attribute.value_len = mtu_length - 9;
                LEatt_Encode_PDU(connection_handle,ATT_PREPARE_WRITE_RESPONSE,p_pdu_params);
            }
        }
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_PREPARE_WRITE==1)
    case ATT_PREPARE_WRITE_RESPONSE :

        if ((ATT_config.sub_procedure == GATT_WRITE_LONG_CHARACTERISTIC) ||
            (ATT_config.sub_procedure == GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE_LONG) ||
            (ATT_config.sub_procedure == GATT_CHARACTERISTIC_VALUE_RELIABLE_WRITE))
        {

            // NOTE !! TO DO CHECK the CONTENTS OF PREPARED WRITE RESPONSE Against the PREPARE_WRITE_REQUEST
            // First check - that the Prepare_Write_Response matches the last Prepare Write Request
            //
            // NOTE - no requirement to check the contents of the Prepare_Write_Response if it is used
            // in the WRITE_LONG_CHARACTERISTIC procedure.

            // Check is this is the response to the last segment the Long Write
            // 26 June 2012 -- GF changed for support of reliable Writes
            //if ((ATT_config.value_offset+1) == ATT_config.value_len)
            if (ATT_config.value_offset == ATT_config.value_len)
            {
                // Last Segment Transmitted - Send Execute
                if (ATT_config.sub_procedure != GATT_CHARACTERISTIC_VALUE_RELIABLE_WRITE)
                {
                    p_pdu_params->flags = 0x01;
                    LEatt_Encode_PDU(connection_handle,ATT_EXECUTE_WRITE_REQUEST,p_pdu_params);
                }
                else
                {
                    // Generate Event to indicate that prepared Write is complete.
                    //GATT_Write_Complete_Event(connection_handle,NO_ERROR);
                    GATT_Event(GATT_WRITE_COMPLETE_EVENT, connection_handle, 0x00, 0x00, 0x00, NO_ERROR);

                }

            }
            else
            {
                // Determine if this is the last segment to be transmitted

                if ((ATT_config.value_offset+1+(ATT_config.MTU_LEN - 5)) <=  ATT_config.value_len)
                {
                    p_pdu_params->attribute.value_len = ATT_config.MTU_LEN - 5;
                }
                else
                {
                    p_pdu_params->attribute.value_len = (ATT_config.value_len - ATT_config.value_offset);
                }
                // Move on to the next segment of the Characteristic
                p_pdu_params->attribute.handle = ATT_config.characteristic_Handle;
                p_pdu_params->value_offset = ATT_config.value_offset;
                p_pdu_params->attribute.p_value = &ATT_config.value[ATT_config.value_offset];
                LEatt_Encode_PDU(connection_handle,ATT_PREPARE_WRITE_REQUEST,p_pdu_params);
            }

        }
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_SERVER_SUPPORTS_PREPARE_WRITE==1)
    case ATT_EXECUTE_WRITE_REQUEST :
        flags = *(p_pdu+1);

        if (ATT_config.prepared_write_pending)
        {
            if (flags == 0x01)
            {
                att_status = GATTserver_Write_Command(ATT_config.characteristic_Handle,ATT_config.value,ATT_config.value_offset);
            }
            ATT_config.prepared_write_pending = 0x00;

        }
        else
        {
            att_status = ATT_config.last_prepare_write_error;
        }
        if (att_status==0x00)
        {
            LEatt_Encode_PDU(connection_handle,ATT_EXECUTE_WRITE_RESPONSE,p_pdu_params);
        }
        else
        {
            p_pdu_params->error_opcode = ATT_EXECUTE_WRITE_REQUEST;
            p_pdu_params->attribute_handle = ATT_config.characteristic_Handle;
            p_pdu_params->error_code = att_status;
            ATT_config.last_prepare_write_error = 0x00;
            LEatt_Encode_PDU(connection_handle,ATT_ERROR_RESPONSE,p_pdu_params);
        }
        break;
#endif

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_PREPARE_WRITE==1)
    case ATT_EXECUTE_WRITE_RESPONSE :

        if ((ATT_config.sub_procedure == GATT_WRITE_LONG_CHARACTERISTIC) ||
            (ATT_config.sub_procedure ==GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE_LONG) ||
            (ATT_config.sub_procedure == GATT_CHARACTERISTIC_VALUE_RELIABLE_WRITE))
        {
            GATT_Event(GATT_WRITE_COMPLETE_EVENT, connection_handle, 0x00, 0x00, 0x00, NO_ERROR);
        }
        break;
#endif

    case ATT_HANDLE_VALUE_NOTIFICATION :
        GATT_Event(GATT_VALUE_NOTIFICATION_EVENT,connection_handle,mtu_length,0x00,(p_pdu+1),NO_ERROR);
        break;

    case ATT_HANDLE_VALUE_INDICATION :
        GATT_Event(GATT_VALUE_INDICATION_EVENT,connection_handle,mtu_length,0x00,(p_pdu+1),NO_ERROR);
        // Need to construct a Handle_Value_Confirmation ATT PDU
        LEatt_Encode_PDU(connection_handle,ATT_HANDLE_VALUE_CONFIRMATION,p_pdu_params);
        break;

    case ATT_HANDLE_VALUE_CONFIRMATION :
        p_connection->GATT_timer = BT_TIMER_OFF;
        if (ATT_config.sub_procedure == GATT_CHARACTERISTIC_VALUE_INDICATION)
        {
            GATT_Event(GATT_VALUE_CONFIRMATION_EVENT,connection_handle,0x00,0x00,0x00,NO_ERROR);
        }
        break;

    case ATT_WRITE_COMMAND :
        att_status = GATTserver_Write_Command(start_handle,(p_pdu+3),(mtu_length-7));//mtu_length LL layer payload length set in the Data channel PDU header
        break;
#if (PRH_BS_CFG_SYS_LE_SMP_DATA_SIGNING==1)
    case ATT_SIGNED_WRITE_COMMAND :
        {
#if ((PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1) || (PRH_BS_CFG_SYS_LE_SMP_PERIPHERAL_SUPPORTS_KEY_HIERARCHY!=1))
            //handle = _Get_Uint16((u_int8*)(p_pdu+1));
            u_int8* p_attrib_value = (p_pdu+3);
            u_int8  CSRK[16];
            // Get the CSRK -
            if (LEsecdb_Search_DB(LEconnection_Get_Peer_Addr_Type(p_connection),p_connection->peer_address.bytes,MASTER,0x05,0x00,CSRK))
            {

                if (LEsmp_Verify_Signature(p_pdu,mtu_length-4,CSRK))
                {

                    att_status =  GATTserver_Write_Command(start_handle, p_attrib_value,(mtu_length-15));
                }
            }
#else
            // TO DO = To be completed

#endif
        }
        break;

#endif
    }
}


/********************************************************************************************
 *  GATT API
 *******************************************************************************************/

#if (PRH_BS_CFG_SYS_LE_GATT_CLIENT_SUPPORTED==1)
/*************************************************************************************************
 * Function :- GATT_Discover_Primary_Service
 *
 * This function initiates the Primary Service discovery procedure. It supports both the subprocedures
 * for 1/ Discovering all services of a device and 2/ Discovering a specific service on a device.
 *
 * Both 16 Bit and 128 Bit UUIDs are supported.
 *
 * INPUTs
 * u_int16 connection_handle :-  Connection handle of the ACL link
 * u_int8  UUID_type         :-  Indicates if the service UUID is 16bit or 128bit.
 *                               ( UUID_type = 0 --> 16 Bit UUID
 *                               ( UUID_type = 1 --> 128 Bit UUID )
 * u_int8*  p_UUID           :-  a pointer to a byte array storing the UUID of the service (LSO first)
 *                               ( If the UUID is zero then a discover all services subprocedure is performed)
 *
 * u_int8 protocol_id        :- Identifies the invoker, allowing the response event to be sent to the correct
 *                              layer.
 *                                hci_layer   :- 0x00
 *                                gap_layer   :- 0x01
 *                                application :- 0x02
 *
 **************************************************************************************************/
t_error GATT_Discover_Primary_Service(u_int16 connection_handle,u_int8 UUID_type, u_int8* p_UUID, u_int8 protocol_id)
{
    u_int8 opcode=0;
    u_int8 sub_procedure;
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int16 primary_service_UUID;
    u_int8 i;

    ATT_config.protocol = protocol_id;

    pdu_params.start_handle = 0x0001;
    pdu_params.end_handle = 0xFFFF;
    pdu_params.attribute_type16 = GATT_ATT_TYPE_PRIMARY_SERVICE;

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_128_BIT_UUIDS==1)
    if (UUID_type == 0x01)
    {
        // 128 Bit UUID

        LMutils_Array_Copy(16,ATT_config.UUID,p_UUID);
        ATT_config.UUID_len = 16;
        p_pdu_params->UUID_len = 16;

        primary_service_UUID = (p_UUID[3]<<8)+p_UUID[2];
    }
    else
#endif
    {
        primary_service_UUID = (p_UUID[1]<<8)+p_UUID[0];

        for (i=0;i<2;i++)
            ATT_config.UUID[i] = p_UUID[i];

        //ATT_config.characteristic_16bit_UUID = characteristic_UUID;
        ATT_config.UUID_len = 2;
        p_pdu_params->UUID_len = 2;
    }


    if (primary_service_UUID == 0x0000)
    {
        sub_procedure = GATT_DISCOVER_ALL_SERVICES;
        opcode = ATT_READ_BY_GROUP_TYPE_REQUEST;
    }
#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_DISCOVERY_BY_UUID==1)
    else
    {
        sub_procedure = GATT_DISCOVER_SPECIFIC_SERVICE;
        opcode = ATT_FIND_BY_TYPE_VALUE_REQUEST;
        pdu_params.attribute_value_len = ATT_config.UUID_len;
        pdu_params.attribute.p_value = ATT_config.UUID;
        ATT_config.primary_service_UUID = primary_service_UUID;
    }
#endif
    /*
     * Need to store comfiguration information as the Discover Primary Service
     * can span multip ATT rep/resp exchanges.
     */

    ATT_config.sub_procedure = sub_procedure;
    ATT_config.start_handle = 0x0001;
    ATT_config.end_handle = 0xFFFF;
    ATT_config.connection_handle = connection_handle;
    LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);

    return NO_ERROR;
}

/*************************************************************************************************
 * Function :- GATT_Relationship_Discovery
 *
 * This sub-procedure is used by a client to find include service declarations within a service
 * definition on a server. The service specified is identified by the service handle range.
 *
 * INPUTs
 * u_int16 connection_handle :-  Connection handle of the ACL link
 * u_int16  start_handle     :-  Start of the handle range.
 * u_int16  end_handle       :-  End if the handle range.
 *
 **************************************************************************************************/

t_error GATT_Relationship_Discovery(u_int16 connection_handle, u_int16 start_handle, u_int16 end_handle )
{
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 opcode = ATT_READ_BY_TYPE_REQUEST;

    pdu_params.start_handle = start_handle;
    pdu_params.end_handle = end_handle;
    pdu_params.attribute_type16 = GATT_ATT_TYPE_INCLUDE;
    p_pdu_params->UUID_len = 0x02;

    ATT_config.sub_procedure = GATT_FIND_INCLUDED_SERVICES;
    ATT_config.start_handle = start_handle;
    ATT_config.end_handle = end_handle;
    ATT_config.connection_handle = connection_handle;
    LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
    return NO_ERROR;
}

/*************************************************************************************************
 * Function :- GATT_Characteristic_Discovery
 *
 * This procedure is used by a client to discover service characteristics on a server.
 *
 * Two sub-procedures can be used for characteristic discovery: Discover All Characteristics of a
 * Service and Discover Characteristics by UUID. Both are supported in this function.
 *
 * Sub-Procedure :
 * Discover all Characteristics
 *         For this API the start/end handle range is provided and the requested UUID is set to 0x0000.
 *
 * Discover Characteristics by UUID
 *         For this API the requested Characteristic UUID is provided in addition to the start/end handle range
 *
 * INPUTs
 * u_int16 connection_handle :-  Connection handle of the ACL link
 *
 * u_int16 starting_handle   :- The Starting Handle shall be set to starting handle of the specified service
 * u_int16 ending_handle     :- the Ending Handle shall be set to the ending handle of the specified service.
 * u_int8  uuid_type         :-  Indicates if the service UUID is 16bit or 128bit.
 *                               ( uuid_type = 0 --> 16 Bit UUID
 *                               ( uuid_type = 1 --> 128 Bit UUID )
 *
 * u_int8*  p_UUID           :-  a pointer to a byte array storing the UUID of the service (LSO first)
 *                               ( If the UUID is zero then a discover all services subprocedure is performed)
 * u_int8  protocol_id       :- Identifies the invoking protocol
 *
 **************************************************************************************************/


t_error GATT_Characteristic_Discovery(u_int16 connection_handle, u_int8 uuid_type, u_int8* p_UUID, u_int16 start_handle, u_int16 end_handle, u_int8 protocol_id )
{
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 opcode;
    u_int8 sub_procedure;
    u_int16 characteristic_UUID;
    u_int8 uuid_len;

    if (p_UUID == 0x00)
        return UNSPECIFIED_ERROR;

    ATT_config.protocol = protocol_id;

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_128_BIT_UUIDS==1)
    if (uuid_type == 0x01)
    {
        uuid_len = 16;
        // 128 Bit UUID

        characteristic_UUID = (p_UUID[3]<<8)+p_UUID[2];
    }
    else
#endif
    {
        uuid_len = 2;
        characteristic_UUID = (p_UUID[1]<<8)+p_UUID[0];
    }

    LMutils_Array_Copy(uuid_len,ATT_config.UUID,p_UUID);


    ATT_config.UUID_len = uuid_len;
    p_pdu_params->UUID_len = uuid_len;
    p_pdu_params->p_UUID = ATT_config.UUID;

    opcode = ATT_READ_BY_TYPE_REQUEST;
    p_pdu_params->attribute_type16 = GATT_ATT_TYPE_CHARACTERISTIC;
    p_pdu_params->start_handle = start_handle;
    p_pdu_params->end_handle = end_handle;

    if (characteristic_UUID == 0x0000)
    {
        sub_procedure = GATT_DIS_ALL_CHARS_OF_SERVICE;
    }
#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_DISCOVERY_BY_UUID==1)
    else
    {
        sub_procedure = GATT_DISCOVER_CHARACTERISTICS_BY_UUID;
    }
#endif
    ATT_config.sub_procedure = sub_procedure;
    ATT_config.start_handle = start_handle;
    ATT_config.end_handle = end_handle;
    ATT_config.connection_handle = connection_handle;
    ATT_config.characteristic_16bit_UUID = characteristic_UUID;
    LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);

    return NO_ERROR;
}


/*************************************************************************************************
 * Function :- GATT_Characteristic_Descriptor_Discovery
 *
 * This is used by a client to discover characteristic descriptors of a characteristic.
 * Find's all the characteristic descriptor’s Attribute Handles and Attribute Types within
 * a characteristic definition
 *
 * INPUTs
 * u_int16 connection_handle :-  Connection handle of the ACL link
 *
 * u_int16 starting_handle   :- The Starting Handle set to the handle of the specified characteristic value + 1
 * u_int16 ending_handle     :- The Ending Handle set to the ending handle of the specified characteristic.
 *
 * ???? u_int8  protocol_id       :- Identifies the invoking protocol
 *
 **************************************************************************************************/

t_GATT_error GATT_Characteristic_Descriptor_Discovery(u_int16 connection_handle,u_int16 start_handle, u_int16 end_handle )
{
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 opcode;
    u_int8 sub_procedure = 0;

    if (start_handle > end_handle)
		return GATT_ERROR_INVALID_HANDLE;

    sub_procedure =  GATT_DISCOVER_ALL_CHARACTERISTIC_DESCRIPTORS;

    opcode = ATT_FIND_INFORMATION_REQUEST;
    p_pdu_params->start_handle = start_handle;
    p_pdu_params->end_handle = end_handle;

    ATT_config.sub_procedure = sub_procedure;
    ATT_config.start_handle = start_handle;
    ATT_config.end_handle = end_handle;
    ATT_config.connection_handle = connection_handle;
    LEatt_Encode_PDU(connection_handle, opcode,p_pdu_params);

	return GATT_NO_ERROR;
}

/*************************************************************************************************
 * Function :- GATT_Characteristic_Value_Read
 *
 * This function is used to initiate a read a Characteristic Value from a server. There are four sub-procedures
 * that can be used to read a Characteristic Value:  three are supported by this function :
 *         Read Characteristic Value,
 *         Read Using Characteristic UUID,
 *         Read Long Characteristic Values,
 *
 * INPUTs
 * u_int16 connection_handle :-  Connection handle of the ACL link
 *
 * u_int8  uuid_type         :-  Indicates if the service UUID is 16bit or 128bit.
 *                               ( uuid_type = 0 --> 16 Bit UUID
 *                               ( uuid_type = 1 --> 128 Bit UUID )
 *
 * u_int8   offset           :-  This is the offset of the first byte into the attribute value which should be read.
 *                                  This parameter is only required to support PTS testing.
 *
 * u_int8*  p_UUID           :-  a pointer to a byte array storing the UUID of the service (LSO first)
 *                               ( If the UUID is zero then a discover all services subprocedure is performed)
 * u_int16 starting_handle   :- If a UUID is zero, then this is the handle of the value to be read. If the UUID is
 *                                 not zero then this is the starting handle for the range over which the read will be
 *                                   performed.
 *
 * u_int16 ending_handle     :- the Ending Handle shall be set to the ending handle of the range over which the read
 *                                 will be performed. This parameter is only relevant if the UUID is non-zero.
 * u_int8  protocol_id       :- Identifies the invoking protocol
 *
 **************************************************************************************************/

t_error GATT_Characteristic_Value_Read(u_int16 connection_handle, u_int8 uuid_type, u_int8 offset, u_int8* p_UUID,
                                       u_int16 start_handle, u_int16 end_handle, u_int8 protocol_id )
{
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 opcode;
    u_int16 characteristic_UUID=0;
    u_int8 sub_procedure=0;
    u_int8 uuid_len;
    u_int8 i;

    ATT_config.protocol = protocol_id;
    ATT_config.characteristic_Handle = start_handle;

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_128_BIT_UUIDS==1)
    if (uuid_type & 0x01)
    {
        // 128 Bit UUID
        uuid_len = 16;
        characteristic_UUID = (p_UUID[3]<<8)+p_UUID[2];
    }
    else
#endif
    {
        uuid_len = 2;
        characteristic_UUID = (p_UUID[1]<<8)+p_UUID[0];
    }

#if 1
    for (i=0;i<uuid_len;i++)
        ATT_config.UUID[i] = p_UUID[i];
#else
    LMutils_Array_Copy(uuid_len,ATT_config.UUID,p_UUID);
#endif
    ATT_config.UUID_len = uuid_len;
    p_pdu_params->UUID_len = uuid_len;
    p_pdu_params->p_UUID = ATT_config.UUID;
    ATT_config.characteristic_16bit_UUID = characteristic_UUID;


#if (PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5 ==1) // Conformance ONLY --- This is only required to allow GAR_CL_BI13
    if (offset!= 0x00)
    {
        ATT_config.sub_procedure =  GATT_READ_LONG_CHARACTERISTIC_VALUES;
        ATT_config.value_offset = offset;

        // Need to send more BLOB Requests
        opcode = ATT_READ_BLOB_REQUEST;
        p_pdu_params->attribute.handle = start_handle;
        p_pdu_params->value_offset = ATT_config.value_offset;
        LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
    }
    else
#endif
        if (characteristic_UUID == 0)
        {
            p_pdu_params->attribute.handle = start_handle;

            sub_procedure =  GATT_READ_CHARACTERISTIC_VALUE;
            opcode = ATT_READ_REQUEST;
            LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);

        }
        else
        {
            sub_procedure =  GATT_READ_USING_CHARACTERISTIC_UUID;
            opcode = ATT_READ_BY_TYPE_REQUEST;
            p_pdu_params->start_handle = start_handle;
            p_pdu_params->end_handle = end_handle;
            p_pdu_params->attribute_type16 = characteristic_UUID;
            LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
        }

    ATT_config.sub_procedure = sub_procedure;
    ATT_config.start_handle = start_handle;
    ATT_config.end_handle = end_handle;
    ATT_config.connection_handle = connection_handle;

    return NO_ERROR;
}


#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_READ_MULTIPLE==1)

t_error GATT_Characteristic_Value_Read_Multiple(u_int16 connection_handle,u_int8* p_handle)
{
    u_int8 opcode = ATT_READ_MULTIPLE_REQUEST;
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 i;
    u_int16 handle;

    p_pdu_params->num_handles=0;
    i=0;

    for(i=0;i<5;i++)
    {
        handle = HCIparam_Get_Uint16(p_handle);
        if (handle)
        {
            p_pdu_params->handle[i] = handle;
            p_pdu_params->num_handles++;
        }
        p_handle+=2;
    }

    if (p_pdu_params->num_handles > 0)
    {
        LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
        ATT_config.sub_procedure = GATT_READ_MULTIPLE_CHARACTERISTIC_VALUES;
        ATT_config.connection_handle = connection_handle;
    }
    else
    {
        return UNSPECIFIED_ERROR;
    }

    return NO_ERROR;
}

#endif
/*************************************************************************************************
 * Function :- GATT_Characteristic_Value_Write
 *
 * This procedure is used to write a Characteristic Value to a server.
 * All of the five subprocedures are supported by this function :
 *
 *         Write Without Response
 *         Write Characteristic Value
 *         Write Long Characteristic Values
 *         Reliable Writes
 *         Signed Writes
 *
 * INPUTs
 * u_int16 connection_handle :-  Connection handle of the ACL link
 *
 * u_int8  write_instruct    :-  Determines which of the sub-procedures are used.
 *                                     write_instruct == 1  --> Ack Required ( Write_With_Response)
 *                                     write_instruct == 2  --> Signed Write
 *                                     write_instruct == 4  --> Prepare_Write
 * u_int8  offset            :-  This is the offset of the first byte into the attribute value which should be written.
 *                                  This parameter is only required to support PTS testing.
 *
 * u_int16 value_handle      :- If a handle of the characteristic to be written.
 * u_int8  value_len         :- The length of data to be written.
 *
 * u_int8* p_value             :- A pointer to the data to be written to the peer server.
 * u_int8  protocol_id       :- Identifies the invoking protocol
 *
 **************************************************************************************************/

t_error GATT_Characteristic_Value_Write( u_int16 connection_handle, u_int8 write_instruct, u_int8 offset, u_int16 value_handle, u_int16 value_len, u_int8* p_value,u_int8 protocol_id )
{
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 opcode=0;
    u_int8 sub_procedure=0;

    ATT_config.protocol = protocol_id;
    p_pdu_params->attribute.handle = value_handle;
#if (PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5 ==1)// Required for conformance only -- GAW_CL_BI27
    if (offset != 0)
    {
        u_int8 i=0;

        sub_procedure = GATT_WRITE_LONG_CHARACTERISTIC;
        opcode = ATT_PREPARE_WRITE_REQUEST;
        ATT_config.value_len = value_len;

        for (i=0;i<ATT_config.value_len;i++)
            ATT_config.value[i] = p_value[i];

        ATT_config.characteristic_Handle = value_handle;
        ATT_config.value_offset = 0;

        p_pdu_params->attribute.p_value = &ATT_config.value[0];
        p_pdu_params->value_offset = offset;
        p_pdu_params->attribute.value_len = ATT_config.MTU_LEN - 5;
    }
    else
#endif
        if ((value_len > (ATT_config.MTU_LEN - 5)) || (write_instruct & 0x04) /* Explicit Prepare Write */)
        {
            u_int8 i=0;

            if  (write_instruct & 0x04)
                sub_procedure = GATT_CHARACTERISTIC_VALUE_RELIABLE_WRITE;
            else
                sub_procedure = GATT_WRITE_LONG_CHARACTERISTIC;

            opcode = ATT_PREPARE_WRITE_REQUEST;
            ATT_config.value_len = value_len;

            for (i=0;i<ATT_config.value_len;i++)
                ATT_config.value[i] = p_value[i];

            ATT_config.characteristic_Handle = value_handle;
            ATT_config.value_offset = 0;

            p_pdu_params->attribute.p_value = &ATT_config.value[0];
            p_pdu_params->value_offset = 0;
            if (value_len > (ATT_config.MTU_LEN - 5))
                p_pdu_params->attribute.value_len = ATT_config.MTU_LEN - 5;
            else
                p_pdu_params->attribute.value_len = value_len;
        }
        else
        {

            p_pdu_params->attribute.p_value = p_value;
            p_pdu_params->attribute.value_len = value_len;

            if ( write_instruct == 0x00)
            {
                sub_procedure =  GATT_WRITE_WITHOUT_RESPONSE;
                opcode = ATT_WRITE_COMMAND;
            }
#if (PRH_BS_CFG_SYS_LE_SMP_DATA_SIGNING==1)
            else if (write_instruct == 0x02)
            {
                sub_procedure = GATT_SIGNED_WRITE_WITHOUT_RESPONSE;
                opcode = ATT_SIGNED_WRITE_COMMAND;

                LEsmp_Get_Signature(p_pdu_params->attribute.p_value,p_pdu_params->signature);

            }
#endif
            else if (write_instruct == 0x01) // Ack Needed..
            {
                opcode = ATT_WRITE_REQUEST;
                sub_procedure = GATT_WRITE_CHARACTERISTIC;
            }
        }

    ATT_config.sub_procedure = sub_procedure;
    ATT_config.start_handle = value_handle;
    ATT_config.connection_handle = connection_handle;
    LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
    return NO_ERROR;
}

/*************************************************************************************************
 * Function :- GATT_Client_Charactaristic_Descriptor_Write
 *
 * This sub-procedure is used to write a characteristic descriptor value to a server when the client knows
 * the characteristic descriptor handle.
 *
 * INPUTs
 * u_int16 connection_handle :-  Connection handle of the ACL link
 *
 * u_int16 value_handle      :- If a handle of the characteristic descriptor to be written.
 * u_int8  value_len         :- The length of data to be written.
 *
 * u_int8* p_value             :- A pointer to the data to be written to the peer server.
 * ????u_int8  protocol_id       :- Identifies the invoking protocol
 *
 **************************************************************************************************/

t_error GATT_Client_Charactaristic_Descriptor_Write(u_int16 connection_handle,  u_int16 value_handle, u_int16 value_len, u_int8* p_value )
{
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 opcode;
    u_int8 sub_procedure;

    p_pdu_params->attribute.handle = value_handle;
    p_pdu_params->attribute.p_value = p_value;

    if (value_len > (ATT_config.MTU_LEN - 5))
    {
        u_int16 i;

        sub_procedure = GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE_LONG;

        opcode = ATT_PREPARE_WRITE_REQUEST;

        p_pdu_params->value_offset = 0;

        p_pdu_params->attribute.value_len = ATT_config.MTU_LEN - 5;
        ATT_config.characteristic_Handle = value_handle;
        ATT_config.value_offset = 0;
        ATT_config.value_len = value_len;
        for (i=0;i<value_len;i++)
            ATT_config.value[i] = p_value[i];

    }
    else
    {
        opcode = ATT_WRITE_REQUEST;
        sub_procedure = GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_WRITE_SHORT;

        p_pdu_params->attribute.value_len = value_len;

    }

    ATT_config.sub_procedure = sub_procedure;
    ATT_config.start_handle = value_handle;
    ATT_config.connection_handle = connection_handle;
    LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
    return NO_ERROR;
}

#if (PRH_BS_CFG_SYS_LE_CLIENT_SUPPORTS_PREPARE_WRITE==1)
/*************************************************************************************************
 * Function :- GATT_Client_Execute_Write
 *
 * This is used by the client to commit or rollback a series of prepared writes which have already been
 * sent to the server.
 *
 * INPUTs
 * u_int16 handle :- Connection handle of the ACL link
 *
 * u_int8  flags  :-  0x00 ?Cancel all prepared writes.
 *                      0x01 ?Immediately write all pending prepared values
 *
 **************************************************************************************************/

t_error GATT_Client_Execute_Write(u_int16 handle,u_int8 flags)
{
    if (ATT_config.sub_procedure == GATT_CHARACTERISTIC_VALUE_RELIABLE_WRITE)
    {
        t_Att_pdu_params pdu_params;

        pdu_params.flags = flags;
        LEatt_Encode_PDU(handle,ATT_EXECUTE_WRITE_REQUEST,&pdu_params);
    }
    else
    {
        return UNSPECIFIED_ERROR;
    }
    return NO_ERROR;
}
#endif

#endif


/*************************************************************************************************
 * Function :- GATT_Client_Charactaristic_Descriptor_Read
 *
 * This function is used to read a characteristic descriptor from a server when the client knows the
 * characteristic descriptor declaration’s Attribute handle
 *
 * INPUTs
 * u_int16 connection_handle :-  Connection handle of the ACL link
 *
 * < Deprecated >
 * u_int8 long_val           :- Indicates if the descriptor value is a 'long value' or a normal 'short value'
 *                                     0x00  -- Normal 'short' descriptor value
 *                                     0x01  -- Long descriptor value
 *
 * u_int16 desc_char_handle :- Handle for the descriptor
 **************************************************************************************************/

t_error GATT_Client_Charactaristic_Descriptor_Read(u_int16 connection_handle , /*u_int8 long_val, */u_int16 char_desc_handle)
{
    t_Att_pdu_params pdu_params;
    t_Att_pdu_params* p_pdu_params = &pdu_params;
    u_int8 opcode;

    p_pdu_params->attribute.handle = char_desc_handle;
#if 0
    if (long_val==0x01) // Long Read
    {
        ATT_config.sub_procedure = GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ_LONG;
        opcode = ATT_READ_BLOB_REQUEST;
        p_pdu_params->value_offset = 0;
        ATT_config.characteristic_Handle = char_desc_handle;
        ATT_config.value_offset = 0;
        LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
    }
    else
#endif
    {
        opcode = ATT_READ_REQUEST;
        LEatt_Encode_PDU(connection_handle,opcode,p_pdu_params);
        ATT_config.sub_procedure = GATT_CHARACTERISTIC_DESCRIPTOR_VALUE_READ_SHORT;
    }
    ATT_config.connection_handle = connection_handle;

    return NO_ERROR;
}

/*************************************************************************************************
 * Function :- _Set_Attribute_Value
 *
 * Simple function to copy a block of data. Used to write an Attribute value into a PDU.
 *************************************************************************************************/

void _Set_Attribute_Value(u_int8* p_pdu, u_int8* p_value, u_int16 val_len)
{
    u_int16 i;

    for(i=0;i<val_len;i++)
        p_pdu[i] = p_value[i];
}

/*************************************************************************************************
 * Function :- _Set_Attribute_Signature
 *
 * Simple function to copy a block of data. Used to write an Signature into a PDU.
 *************************************************************************************************/

void _Set_Attribute_Signature(u_int8* p_pdu, u_int8* p_signature )
{
    u_int8 i;

    for(i=0;i<12;i++)
        p_pdu[i] = p_signature[i];
}

/*************************************************************************************************
 * Function :- _Att_Get_Last_Handle
 *
 * This funtion finds the last/end handle in payload of a pdu.
 *
 * INPUTS
 * u_int8   opcode  :- The opcode of the PDU to which the data field belongs. This indicates how to interpret the data
 * u_int8*  p_data  :- A pointer to the data payload section of a PDU.
 * u_int8   mtu_len :- The length of the MTU in which the data payload was recieved.
 * u_int8   attrib_len :- The attribute length field of the data in the payload
 *
 *************************************************************************************************/

u_int16 _Att_Get_Last_Handle(u_int8 opcode,u_int8* p_data,u_int16 mtu_len,u_int8 attrib_len)
{
    u_int8 len_of_all_attrib_data;
    //u_int8 num_attribs;
    u_int16 last_handle = 0;
    u_int8 handle_offset;

    len_of_all_attrib_data = mtu_len - 6 /*l2cap header + ATT header*/;

    if (opcode == ATT_FIND_BY_TYPE_VALUE_RESPONSE)
        len_of_all_attrib_data += 1;

    handle_offset = (((len_of_all_attrib_data/attrib_len)-1) * attrib_len);
    if ((opcode == ATT_READ_BY_GROUP_TYPE_RESPONSE) || (opcode == ATT_FIND_BY_TYPE_VALUE_RESPONSE))
        handle_offset+=2;

    last_handle = HCIparam_Get_Uint16(&p_data[handle_offset]);

    return last_handle;
}
/*********************************************************************************
 * GATT Server Interface
 *********************************************************************************/
/*************************************************************************************************
 * Function :- GATT_Command_Complete_Event
 *
 * This function generates a command complete for a GATT procedure.
 *
 * INPUTS
 * u_int16  connection_handle  :-  The handle for the ACL link on which the procedure was run.
 *
 * u_int8   operation_id       :- The operarator id mapping to the GATT command for which the connection complete
 *                                   corresponds to :
 *
 *                                     0x01   Primary Service Discovery
 *                                    0x02   Relationship Discovery
 *                                    0x03   Characteristic Discovery
 *                                    0x04   Characteristic Descriptor Discovery
 *                                    0x05   Read Characteristic
 *                                    0x11   Exchange MTU
 *                                    0x10   Write Local Permissions
 *                                    0x0F   Write Local Value
 *
 * u_int8   status             :- Indicates success/failure of the GATT procedure
 *************************************************************************************************/

u_int8 GATT_Command_Complete_Event(u_int16 connection_handle,u_int8 operation_id,u_int8 status)
{
#if (PRH_BS_CFG_SYS_GATT_DELIVER_EVENTS_VIA_HCI==1)
    t_length metaEventParaLen=0;
    t_length queue_length;
    t_p_pdu p_buffer =0x0;
    struct q_desc* qd;

    metaEventParaLen = 5;

    if (operation_id == (u_int8)(LE_GATT_CLIENT_EXCHANGE_MTU&0xFF)) // Command Complete for Exchange MTU
        metaEventParaLen+=2;

    queue_length = metaEventParaLen + 2;

    qd = BTq_Enqueue(HCI_EVENT_Q, 0, queue_length);
    if(qd && qd->data)
    {
        p_buffer=qd->data;
    }
    else
    {
        return MEMORY_FULL;
    }

    *(p_buffer)= LE_META_EVENT;
    *(p_buffer+1)= metaEventParaLen;
    *(p_buffer+2)= GATT_COMMAND_COMPLETE_EVENT;
    _Insert_Uint16((qd->data+3), connection_handle);
    p_buffer[5] = operation_id;
    p_buffer[6] = status;
    if (operation_id == 0x11)
        _Insert_Uint16((qd->data+7),ATT_config.MTU_LEN);

    BTq_Commit(HCI_EVENT_Q,0);

#endif

#if (PRH_BS_CFG_SYS_EMBEDDED_PROFILES==1)

    {
        t_gatt_command_complete_params gatt_command_complete_params;
        u_int8 i;

        gatt_command_complete_params.operation_id = operation_id;
        gatt_command_complete_params.handle = connection_handle;
        gatt_command_complete_params.status = status;
        gatt_command_complete_params.mtu_len = ATT_config.MTU_LEN;

        for (i=0;i<GATT_MAX_NUM_PROTOCOL_USERS;i++)
        {
            if (GATT_Command_Complete_Callback_Array[i] /* HCI Callback */)
            {
                ((void(*)(u_int8,t_gatt_command_complete_params*))(GATT_Command_Complete_Callback_Array[i]))(operation_id,&gatt_command_complete_params);
            }
        }
    }
#endif
    return NO_ERROR;
}

/*************************************************************************************************
 * Function :- GATT_Event
 *
 * Description
 * This Function is responsible for generating Events to the HCI or user application.
 * The following Event opcodes are supported
 *
 *     GATT_READ_RESPONSE_EVENT
 *  GATT_DISCOVER_DESCRIPTORS_EVENT
 *     GATT_DISCOVER_SERVICES_BY_UUID_EVENT
 *  GATT_DISCOVER_INCLUDED_SERVICES_EVENT
 *  GATT_READ_BY_TYPE_RESPONSE_EVENT
 *  GATT_DISCOVER_CHARACTERISTICS_EVENT
 *  GATT_DISCOVER_SERVICES_EVENT
 *  GATT_VALUE_NOTIFICATION_EVENT
 *  GATT_VALUE_INDICATION_EVENT
 *  GATT_VALUE_CONFIRMATION_EVENT
 *
 *************************************************************************************************/

u_int8 GATT_Event(u_int8 event_opcode, u_int16 handle, u_int16 payload_len, u_int8 attrib_len, u_int8* p_pdu,u_int8 status)
{
    u_int8 i=0;
#if (PRH_BS_CFG_SYS_EMBEDDED_PROFILES==1)
    t_gatt_event_params gatt_event_params;
#endif
    //os_printf("GATT_Event eventCode:0x%.2x,0x%.2x,0x%.2x,0x%.2x,0x%.2x\r\n",event_opcode,handle,payload_len,attrib_len,status);	

#if (PRH_BS_CFG_SYS_GATT_DELIVER_EVENTS_VIA_HCI == 1) // GF 29 Aug
    t_length metaEventParaLen=0;
    t_p_pdu p_buffer =0x0;
    struct q_desc* qd;
    u_int8 value_payload_len=0;

    /*2
     * Length Field -
     * GATT_DISCOVER_SERVICES_EVENT
     *     MTU payload - 4 (l2cap header) - 1 Opcode - 1 Attrib_len
     *     Add on : +1 overall len, +1 Attrib_len
     *     Add on : +1 Event Id, +2 handle, + 1 Status
     *
     *  GATT_DISCOVER_SERVICES_BY_UUID_EVENT
     *     MTU payload - 4 (l2cap header) - 1 Opcode
     *     Add on : +1 MetaEvent +1 overall len, +1 Opcode +2 handle  +1 Status
     *     Add on : +1 Payload Len + 1 Attribute Len
     */

    while((i<13) && (gatt_event_len[i].event_id != event_opcode))
        i++;

    if(i == 13)
    {
        return ILLEGAL_COMMAND;
    }
    if(gatt_event_len[i].event_id == event_opcode)
    {
        value_payload_len = payload_len - gatt_event_len[i].payload_offset;
        metaEventParaLen = value_payload_len + gatt_event_len[i].length_offset;
    }

    qd = BTq_Enqueue(HCI_EVENT_Q, 0, metaEventParaLen+2);
    if((qd) && (qd->data))
    {
        p_buffer=qd->data;
    }
    else
    {
        return MEMORY_FULL;
    }


    *(p_buffer)= LE_META_EVENT;
    *(p_buffer+1)= metaEventParaLen;
    *(p_buffer+2)= event_opcode;
    _Insert_Uint16((p_buffer+3), handle);
    p_buffer[5] = status;

    if(gatt_event_len[i].event_id == event_opcode)
    {

        if (gatt_event_len[i].type != 0)
        {
            p_buffer[6] = value_payload_len;
            if (gatt_event_len[i].type == 2)
            {
                p_buffer[7] = attrib_len;
                p_buffer+=8;
            }
            else
            {
                p_buffer+=7;
            }
            for (i=0;i<value_payload_len;i++)
                p_buffer[i] = p_pdu[i];
        }
    }

    BTq_Commit(HCI_EVENT_Q,0);
#endif

#if (PRH_BS_CFG_SYS_EMBEDDED_PROFILES==1)
 //   return NO_ERROR;  /*  revision 335 change */
    /*
     * Determine the Payload Length
     */
    //os_printf("GATT_Event : %x\r\n",event_opcode);
    gatt_event_params.payload_length = 0;
    gatt_event_params.payload_length = payload_len - gatt_event_len[i].payload_offset;

    gatt_event_params.event_opcode = event_opcode;
    gatt_event_params.handle = handle;
    gatt_event_params.status = status;
    gatt_event_params.payload = p_pdu;
    if (event_opcode == GATT_WRITE_INDICATION_EVENT)
    {
        gatt_event_params.att_handle = handle;
        gatt_event_params.handle = 0x00;
    }
    else
    {
        gatt_event_params.att_handle = 0x00;
    }
    gatt_event_params.att_len = attrib_len;

    for (i=0;i<GATT_MAX_NUM_PROTOCOL_USERS;i++)
    {
        if (GATT_Event_Callback_Array[i] /* HCI Callback */)
        {
            ((void(*)(u_int8,t_gatt_event_params*))(GATT_Event_Callback_Array[i]))(event_opcode,&gatt_event_params);/* LE_APP_GATT_Command_Complete */
        }
    }
#endif
    return NO_ERROR;
}

/*************************************************************************************************
 * Function :- _GATT_Find_Characteric_UUID_In_Payload
 *
 * This function searchs the incoming PDU contents for a match to the requested Characterisic UUID
 * if a match is found it adds it to the attrib_val_array[..].
 *
 * u_int8*   p_pdu            :- A pointer to the payload of a pdu to be searched
 * u_int8* p_attrib_val_array :- A pointer to the output of this function which is an sequence of attribute values.
 * u_int8  attrib_len         :- The length of each attribute.
 * u_int8  mtu_len            :- The length of the overall MTU in which the corresponding PDU was recieved
 * u_int8* p_UUID             :- A pointer to the UUID which is being searched for.
 *
 **************************************************************************************************/

u_int8 _GATT_Find_Characteric_UUID_In_Payload( u_int8* p_pdu, u_int8* p_attrib_val_array, u_int8 attrib_len, u_int8 mtu_len, u_int8* p_UUID, u_int8 UUID_len)
{
    // The format of the ATTRIBUTE Data is a set of Handle-Value Pairs
    //  Byte 0-1   Attribute Handle
    //  Byte 2-6   Characteristic Definition
    //       Byte 2 - Properties
    //       Byte 3-4 Value Handle
    //       Byte 5-6 Characteristic UUID

    // So the Characteristic UUID is at Byte 5-6
    u_int8 num_attribs_found=0;
    u_int8 i;
    u_int8 num_attribs = ((mtu_len - 6 /*l2cap header  + ATT header*/)/attrib_len);

    for (i=0;i<num_attribs;i++)
    {
        if (_GATT_Compare_UUID(UUID_len,p_UUID,&p_pdu[5]))
        {
            u_int8 j;

            for (j=0;j<attrib_len;j++)
                p_attrib_val_array[j] = p_pdu[j];

            num_attribs_found++;
        }
        p_pdu+=attrib_len;
    }
    return num_attribs_found;
}

/*************************************************************************************************
 * Function :- GATT_Exchange_MTU
 *
 * This is used by the client to set the ATT_MTU to the maximum possible value that can be supported by
 * both devices when the client supports a value greater than the default ATT_MTU for the Attribute
 * Protocol. This subprocedure shall only be initiated once during a connection
 *
 * INPUTS
 * u_int16 connection_handle :- Identifier for ACL link
 **************************************************************************************************/

u_int8 GATT_Exchange_MTU(u_int16 connection_handle)
{
    t_Att_pdu_params  pdu_params;
    t_LE_Connection* p_connection = LEconnection_Find_P_Connection(connection_handle);

    if (LEconnection_Get_Role(p_connection) == MASTER)
    {
        // Initiate an Exchange Mtu
        pdu_params.mtu = ATT_config.MTU_LEN;
        LEatt_Encode_PDU(connection_handle,ATT_EXCHANGE_MTU_REQUEST,&pdu_params);
    }
    return NO_ERROR;
}

/*****************************************************************************
 * GATT Timer Handle Functions
 *
 * A 30 Second GATT timer is started for each request sent on an ACL link.
 * The timer is cleared on receipt of the Response corresponding to the request
 ****************************************************************************/

/*****************************************************************************
 * Function :- _GATT_ReSet_GATT_Timer
 *
 * Sets a 30 second GATT timer for an ACL link
 *****************************************************************************/

void _GATT_ReSet_GATT_Timer(t_LE_Connection* p_connection)
{
    p_connection->GATT_timer  = (HW_Get_Native_Clk_Avoid_Race() + (30 * 3200))  & BT_CLOCK_MAX_TICKS;
}

/*****************************************************************************
 * Function :- GATT_Check_GATT_Timers
 *
 * This function is used to check if any GATT timers have expired and takes the
 * appropiate action if one or more have expired.
 *
 * It cycles through all ACL links - checking if the GATT timer has expired. If a
 * timer has expired, we use the 'pending_ATT_opcode' to determine the Event which
 * should be invoked.
 *****************************************************************************/

void GATT_Check_GATT_Timers(void)
{
    t_LE_Connection* p_connection;
    u_int8 i;
    u_int32 current_clk = HW_Get_Native_Clk_Avoid_Race();

    for (i=0;i<DEFAULT_MAX_NUM_LE_LINKS;i++)
    {
        p_connection = LEconnection_Find_Link_Entry(i);

        //if((LEconnection_Is_Active(p_connection)) && (p_connection->GATT_timer != BT_CLOCK_MAX_TIMEOUT))
		if((LEconnection_Is_Active(p_connection)) && (p_connection->GATT_timer != BT_TIMER_OFF))
        {
            if (((p_connection->GATT_timer - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT)
            {
                u_int16 connection_handle =  LEconnection_Determine_Connection_Handle(p_connection);
                u_int8 status = GATT_ERROR_TIMEOUT;
                u_int8 dummy;

                p_connection->GATT_timer  = BT_TIMER_OFF;
                switch(ATT_config.pending_ATT_opcode)
                {
                case ATT_EXCHANGE_MTU_REQUEST :
                    GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_EXCHANGE_MTU&0xFF),status);
                    break;

                case ATT_FIND_INFORMATION_REQUEST :
                    GATT_Event(GATT_DISCOVER_DESCRIPTORS_EVENT,connection_handle, 0,  0, &dummy,status);
                    break;

                case ATT_FIND_BY_TYPE_VALUE_REQUEST :
                    GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_PRIMARY_SERVICE_DISCOVERY&0xFF),status);
                    break;

                case ATT_READ_BY_TYPE_REQUEST :
                    if (ATT_config.sub_procedure == GATT_FIND_INCLUDED_SERVICES)
                    {
                        GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_RELATIONSHIP_DISCOVERY&0xFF),status);
                    }
                    else if ((ATT_config.sub_procedure == GATT_DIS_ALL_CHARS_OF_SERVICE) ||
                             (ATT_config.sub_procedure == GATT_DISCOVER_CHARACTERISTICS_BY_UUID))
                    {
                        GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_CHARACTERISTIC_DISCOVERY&0xFF),status);
                    }
                    else if (ATT_config.sub_procedure == GATT_READ_USING_CHARACTERISTIC_UUID )
                    {
                        GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_CHARACTERISTIC_VALUE_READ&0xFF),status);
                    }
                    break;

                case ATT_READ_REQUEST :
                case ATT_READ_BLOB_REQUEST :
                case ATT_READ_MULTIPLE_REQUEST :
                    GATT_Event(GATT_READ_RESPONSE_EVENT, connection_handle, 0x06 /* Dummy L2CAP Header */, 0x00,0x00,status);
                    break;

                case ATT_READ_BY_GROUP_TYPE_REQUEST :
                    if (ATT_config.sub_procedure == GATT_DISCOVER_ALL_SERVICES)
                    {
                        GATT_Command_Complete_Event(connection_handle,(u_int8)(LE_GATT_CLIENT_PRIMARY_SERVICE_DISCOVERY&0xFF),status);
                    }
                    break;

                case ATT_WRITE_REQUEST :
                case ATT_PREPARE_WRITE_REQUEST :
                case ATT_EXECUTE_WRITE_REQUEST :
                    GATT_Event(GATT_WRITE_COMPLETE_EVENT, connection_handle, 0x00, 0x00, 0x00, status);
                    break;

                case ATT_HANDLE_VALUE_INDICATION :
                    GATT_Event(GATT_VALUE_CONFIRMATION_EVENT,ATT_config.start_handle,0x00,0x00,0x00,GATT_ERROR_TIMEOUT);
                    break;
                }

                //**************************************************************************************************
                // NOTE !! For Normal operation we should disconnect the link here !! BUT the
                // PTS tester does not like this in PROT_BV_01 and PROT_BV_02 so for now we disable the disconnect
                // HOWEVER, this should be re-enabled in Release builds OR when the PTS is fixed.
                //*************************************************************************************************
#if (PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5 != 1)
                LEconnection_Disconnect(p_connection,0x22 /* LL_Response_Timeout */);
              
#endif

            }
        }
    }
}

/*****************************************************************************
 * Function :- GATT_Check_Is_Link_Encrypted
 *
 * This function is used to check if a link is encrypted.
 *
 *****************************************************************************/


u_int8 GATT_Check_Is_Link_Encrypted(void)
{
#if (PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED==1)
    t_LE_Connection* p_connection = LEconnection_Find_P_Connection(ATT_config.connection_handle);

    return (p_connection->current_security_state & LE_ENCRYPTION_ACTIVE) ? 0 : 1;
#else
    return 0;
#endif
}

u_int16 GATT_Get_Active_Handle(void)
{
    return ATT_config.connection_handle;
}

#endif


u_int8 _GATT_Compare_UUID(u_int8 UUID_len,u_int8* p_UUID1,u_int8* p_UUID2 )
{
    u_int8 i;

    for (i=0;i<UUID_len;i++)
    {
        if (p_UUID1[i] != p_UUID2[i])
            return 0;
    }
    return 1;
}
#endif
