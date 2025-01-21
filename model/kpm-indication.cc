/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 Northeastern University
 * Copyright (c) 2022 Sapienza, University of Rome
 * Copyright (c) 2022 University of Padova
 * Copyright (c) 2024 Orange Innovation Egypt
 * Copyright (c) 2025 Pukyung National University, Korea
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Andrea Lacava <thecave003@gmail.com>
 *		   Tommaso Zugno <tommasozugno@gmail.com>
 *		   Michele Polese <michele.polese@gmail.com>
 *       Mostafa Ashraf <mostafa.ashraf.ext@orange.com>
 *       Aya Kamal <aya.kamal.ext@orange.com>
 *       Abdelrhman Soliman <abdelrhman.soliman.ext@orange.com>
 *       Jaewook Lee <jlee0315@pknu.ac.kr>
 */

#include <ns3/kpm-indication.h>
#include <ns3/asn1c-types.h>
#include <ns3/log.h>

extern "C" {
#include "E2SM-KPM-IndicationHeader-Format1.h"
#include "E2SM-KPM-IndicationMessage-Format1.h"
#include "GlobalE2node-ID.h"
#include "GlobalE2node-gNB-ID.h"
#include "GlobalE2node-eNB-ID.h"
#include "GlobalE2node-ng-eNB-ID.h"
#include "GlobalE2node-en-gNB-ID.h"
#include "NRCGI.h"
#include "PM-Containers-Item.h"
#include "RIC-EventTriggerStyle-Item.h"
#include "RIC-ReportStyle-Item.h"
#include "TimeStamp.h"
#include "CUUPMeasurement-Container.h"
#include "PlmnID-Item.h"
#include "EPC-CUUP-PM-Format.h"
#include "PerQCIReportListItemFormat.h"
#include "PerUE-PM-Item.h"
#include "PM-Info-Item.h"
#include "MeasurementInfoList.h"
#include "CellObjectID.h"
#include "CellResourceReportListItem.h"
#include "ServedPlmnPerCellListItem.h"
#include "EPC-DU-PM-Container.h"
#include "PerQCIReportListItem.h"
//==============================================================
#include "MeasurementRecordItem.h"
#include "MeasurementDataItem.h"
#include "MeasurementInfoItem.h"
#include "LabelInfoItem.h"
#include "MeasurementLabel.h"
#include "E2SM-KPM-IndicationMessage-Format3.h"
#include "UEMeasurementReportItem.h"
#include "UEID-GNB.h"

#include "conversions.h"

// #include "timing_ms.h"

BIT_STRING_t cp_amf_region_id_to_bit_string (uint8_t src);

uint8_t cp_amf_region_id_to_u8 (BIT_STRING_t src);

/* AMF Set ID */
BIT_STRING_t cp_amf_set_id_to_bit_string (uint16_t val);

uint16_t cp_amf_set_id_to_u16 (BIT_STRING_t src);

/* AMF Pointer */
BIT_STRING_t cp_amf_ptr_to_bit_string (uint8_t src);

uint8_t cp_amf_ptr_to_u8 (BIT_STRING_t src);

OCTET_STRING_t cp_plmn_identity_to_octant_string (uint8_t src);
}

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("KpmIndication");

KpmIndicationHeader::KpmIndicationHeader (GlobalE2nodeType nodeType,
                                          KpmRicIndicationHeaderValues values)
{
  m_nodeType = nodeType;
  E2SM_KPM_IndicationHeader_t *descriptor = new E2SM_KPM_IndicationHeader_t;
  FillAndEncodeKpmRicIndicationHeader (descriptor, values);
  delete descriptor;
}

KpmIndicationHeader::~KpmIndicationHeader ()
{
  NS_LOG_FUNCTION (this);
  free (m_buffer);
  m_size = 0;
}

uint64_t KpmIndicationHeader::time_now_us_clck() {
  struct timespec tms;

  /* The C11 way */
  /* if (! timespec_get(&tms, TIME_UTC))  */

  /* POSIX.1-2008 way */
  if (clock_gettime(CLOCK_REALTIME,&tms)) {
    return -1;
  }
  /* seconds, multiplied with 1 million */
  uint64_t micros = tms.tv_sec * 1000000;
  /* Add full microseconds */
  micros += tms.tv_nsec/1000;
  /* round up if necessary */
  if (tms.tv_nsec % 1000 >= 500) {
    ++micros;
  }
  NS_LOG_INFO("**micros Timing is " << micros);
  return micros;
}

// KPM ind_msg latency 
OCTET_STRING_t KpmIndicationHeader::get_time_now_us() {

  std::vector<uint8_t> byteArray(sizeof(uint64_t));

  auto val = time_now_us_clck();

  // NS_LOG_DEBUG("**VAL Timing is " << val);

  memcpy(byteArray.data(), &val, sizeof(uint64_t));

  // NS_LOG_DEBUG("ByteArray Size is " << byteArray.size());
  
  OCTET_STRING_t dst = {0};
  
  dst.buf = (uint8_t*)calloc(byteArray.size(), sizeof(uint8_t)); 
  dst.size = byteArray.size();

  memcpy(dst.buf, byteArray.data(), dst.size);

  return dst;
}

OCTET_STRING_t KpmIndicationHeader::int_64_to_octet_string(uint64_t x) {
    OCTET_STRING_t asn = {0};

    asn.buf = (uint8_t*) calloc(sizeof(x) + 1, sizeof(char));
    memcpy(asn.buf,&x,sizeof(x));
    asn.size = sizeof(x);

    return asn;
}

// OCTET_STRING_t KpmIndicationHeader::int_64_to_octet_string(uint64_t value)
// {
//   std::vector<uint8_t> byteArray(sizeof(uint64_t));
//   memcpy(byteArray.data(), &value, sizeof(uint64_t));

//   OCTET_STRING_t dst = {0};

//   dst.buf = (uint8_t*)calloc(byteArray.size(), sizeof(uint8_t)); 
//   dst.size = byteArray.size();

//   memcpy(dst.buf, byteArray.data(), dst.size);

//   return dst;
// }

uint64_t KpmIndicationHeader::octet_string_to_int_64(OCTET_STRING_t asn) {
    uint64_t x = {0};

    memcpy(&x, asn.buf, asn.size);

    return x;
}

void
KpmIndicationHeader::Encode (E2SM_KPM_IndicationHeader_t *descriptor) {
  asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
  asn_encode_to_new_buffer_result_s encodedHeader = asn_encode_to_new_buffer (
      opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_IndicationHeader, descriptor);

  if (encodedHeader.result.encoded < 0)
    {
      NS_FATAL_ERROR ("*Error during the encoding of the RIC Indication Header, errno: "
                      << strerror (errno) << ", failed_type "
                      << encodedHeader.result.failed_type->name << ", structure_ptr "
                      << encodedHeader.result.structure_ptr);
    }

  m_buffer = encodedHeader.buffer;
  m_size = encodedHeader.result.encoded;
}
//Update by Jlee
void
KpmIndicationHeader::FillAndEncodeKpmRicIndicationHeader (E2SM_KPM_IndicationHeader_t *descriptor,
                                                          KpmRicIndicationHeaderValues values){
  /*
  indicationHeader_formats
 - present
 - choice
   ▪ indicationHeader_Format1
      - colletStartTime (octetString)
      - *fileFormatversion (PrintableString_t) (option)
      - *senderName (PrintableString_t) (option)
      - *senderType (PrintableString_t) (option)
      - *vendorName (PrintableString_t) (option)
  */
  NS_LOG_INFO ("FillAndEncodeKpmRicIndicationHeader");

  E2SM_KPM_IndicationHeader_Format1_t *ind_header = (E2SM_KPM_IndicationHeader_Format1_t *) calloc (
      1, sizeof (E2SM_KPM_IndicationHeader_Format1_t));

  NS_LOG_DEBUG ("Timestamp received: " << values.m_timestamp);
  long bigEndianTimestamp = htobe64 (values.m_timestamp);
  NS_LOG_DEBUG ("Timestamp inverted: " << bigEndianTimestamp);
  
  Ptr<OctetString> collettime_value = Create<OctetString> ((void *) &bigEndianTimestamp, TIMESTAMP_LIMIT_SIZE);
  ind_header->colletStartTime = collettime_value->GetValue ();

  //ind_header->colletStartTime = get_time_now_us();
  NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationHeader_Format1, ind_header));

  descriptor->indicationHeader_formats.present = E2SM_KPM_IndicationHeader__indicationHeader_formats_PR_indicationHeader_Format1;
  descriptor->indicationHeader_formats.choice.indicationHeader_Format1 = ind_header;

  Encode (descriptor);
  ASN_STRUCT_FREE (asn_DEF_E2SM_KPM_IndicationHeader_Format1, ind_header);
}

KpmIndicationMessage::KpmIndicationMessage (KpmIndicationMessageValues values) {
  E2SM_KPM_IndicationMessage_t *descriptor = new E2SM_KPM_IndicationMessage_t ();
  CheckConstraints (values);
  FillAndEncodeKpmIndicationMessage (descriptor, values);
  delete descriptor;
}

KpmIndicationMessage::~KpmIndicationMessage () {
  free (m_buffer);
  m_size = 0;
}

void
KpmIndicationMessage::CheckConstraints (KpmIndicationMessageValues values)
{
}

typedef struct
{
  uint8_t *buf;
  size_t len;
} darsh_byte_array_t;

void
KpmIndicationMessage::Encode (E2SM_KPM_IndicationMessage_t *descriptor) {
      printf ("[JLEE: Encoding E2SM KPM Indicatin Message] ");
      asn_codec_ctx_t *opt_cod = 0; // disable stack bounds checking
      asn_encode_to_new_buffer_result_s encodedMsg = asn_encode_to_new_buffer (
          opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_IndicationMessage, descriptor);

      if (encodedMsg.result.encoded < 0)
        {
          assert (encodedMsg.result.structure_ptr != nullptr);
          NS_FATAL_ERROR ("Error during the encoding of the RIC Indication Message, errno: "
                          << strerror (errno) << ", failed_type "
                          << encodedMsg.result.failed_type->name << ", structure_ptr "
                          << encodedMsg.result.structure_ptr);
        }

      m_buffer = encodedMsg.buffer;
      m_size = encodedMsg.result.encoded;
}

//update by jlee
void
KpmIndicationMessage::FillAndEncodeKpmIndicationMessage (E2SM_KPM_IndicationMessage_t *descriptor,
                                                         KpmIndicationMessageValues values)
{
 /* KPM_Indication_Message (descriptor)
  - present
    - E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_NOTHING
    - E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1,
    - E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format2,
    - E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format3,
  - choice
    - choice.indicationMessage_Format1 
    - choice.indicationMessage_Format2 
    - choice.indicationMessage_Format3 

*/

/*
indicationMessage_Format3
 - ueMeasReportList
   - list (UEMeasurementReportItem)
     - ueID
     - measReport (Format1)

indicationMessage_Format1
  - measData->list (item 별로)->measRecodr->list (1개만 사용)-> (present, choice.integer/real) <== 값 적용  
  - *measInfoList-> (item 별로)->measType->list (1개만 사용)-> 
     -> measType (present, choice.measName/measID : measName 부터 사용), labelInfoList.nolabel= 0
*/
      /* Format 1 Message*/
      /*
      format1message
       - measData
         - list (MeasurementDataItem : measRecord)
           - measRecord
             - list (MeasurementRecordItem)
                - present (MeasurementRecordItem_PR_integer, MeasurementRecordItem_PR_real)
                - choice (integer, double)
       - *measInfoList (option)
          - list (MeasurementInfoItem : measType + labelInfoList)
            - measType
              - present (MeasurementType_PR_measName, MeasurementType_PR_measID)
              - choice (measName, measID)
            - labelInfoList
              - list (LabelInfoItem: measLabel)
                 - measLabel 
                   - *noLabel = 0      
      */

  const bool ENABLE_FORMAT_ONE = false; // To be deteted

  if (ENABLE_FORMAT_ONE)
    {
 
      printf ("ENABLE_FORMAT_ONE\n ");

      E2SM_KPM_IndicationMessage_t *kpmindmessage = (E2SM_KPM_IndicationMessage_t *) calloc (1, sizeof (E2SM_KPM_IndicationMessage_t));

      E2SM_KPM_IndicationMessage_Format1_t *format1message = (E2SM_KPM_IndicationMessage_Format1_t *) calloc ( 1, sizeof (E2SM_KPM_IndicationMessage_Format1_t));

      MeasurementRecord_t *measrecord = (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t));
      MeasurementRecordItem_t *measrecorditem = (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
      measrecorditem->present = MeasurementRecordItem_PR_integer;
      measrecorditem->choice.integer = 1;

      ASN_SEQUENCE_ADD (&measrecord->list, measrecorditem);

      MeasurementDataItem_t *measdataitem = (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));

      measdataitem->measRecord = *measrecord;

      MeasurementData_t *measdata =(MeasurementData_t *) calloc (1, sizeof (MeasurementData_t));

      ASN_SEQUENCE_ADD (&measdata->list, measdataitem);

      format1message->measData = *measdata;

      kpmindmessage->indicationMessage_formats.present = E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1;
      kpmindmessage->indicationMessage_formats.choice.indicationMessage_Format1 = format1message;

      // ASN_SEQUENCE_ADD(&ind_message->indicationMessage_formats.choice.indicationMessage_Format1, test_kpm_ind_message) ;

      Encode (kpmindmessage);
    }
  else
    {
      printf ("ENABLE_FORMAT_THREE\n "); 
      E2SM_KPM_IndicationMessage_t *kpmindmessage = (E2SM_KPM_IndicationMessage_t *) calloc (1, sizeof (E2SM_KPM_IndicationMessage_t));

      // Create Format 3
      /*
      indicationMessage_Format3
        - ueMeasReportList
        - list (UEMeasurementReportItem)
            - ueID
            - measReport (Format1)
     */
      E2SM_KPM_IndicationMessage_Format3_t *format_3 = (E2SM_KPM_IndicationMessage_Format3_t *) calloc (1, sizeof (E2SM_KPM_IndicationMessage_Format3_t));
      assert (format_3 != nullptr && "Memory exhausted");
      // TODOL Multi UEs
      if(values.m_UeMeasItems.size() > 0) {
          UEMeasurementReportList_t* m_ueMeasReportList = (UEMeasurementReportList_t *) calloc (values.m_UeMeasItems.size(), sizeof (UEMeasurementReportList_t));
          int _index = 0;
          for(auto ueitem : values.m_UeMeasItems) {
              UEMeasurementReportItem_t *ueMeasItem = (UEMeasurementReportItem_t *) calloc (1, sizeof (UEMeasurementReportItem_t));
              // Set UE ID Information
              UEID_GNB_t *gnb_asn = (UEID_GNB_t *) calloc (1, sizeof (UEID_GNB_t));
              assert (gnb_asn != NULL && "Memory exhausted");

              gnb_asn->amf_UE_NGAP_ID.buf = (uint8_t *) calloc (5, sizeof (uint8_t));
              assert (gnb_asn->amf_UE_NGAP_ID.buf != NULL && "Memory exhausted");
              //0103 Update 
              //asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID,
              //static_cast<unsigned long>(KpmIndicationHeader::octet_string_to_int_64(ueIndication->GetId())));
              asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID, 1);
              printf (" \n[Jlee] UE ID : %s", ueitem.ueID.c_str());

              gnb_asn->guami.aMFPointer = cp_amf_ptr_to_bit_string ((rand () % 2 ^ 6) + 0);
              gnb_asn->guami.aMFSetID = cp_amf_set_id_to_bit_string ((rand () % 2 ^ 10) + 0);
              gnb_asn->guami.aMFRegionID = cp_amf_region_id_to_bit_string ((rand () % 2 ^ 8) + 0);
              gnb_asn->guami.pLMNIdentity = cp_plmn_identity_to_octant_string (rand() % 505, rand() % 99, 2);
              gnb_asn->ran_UEID = (RANUEID_t *) calloc(1, sizeof(*gnb_asn->ran_UEID));
              gnb_asn->ran_UEID->buf = (uint8_t *) calloc(8, sizeof(*gnb_asn->ran_UEID->buf));
              uint8_t ran_ue_id[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
              memcpy(gnb_asn->ran_UEID->buf, ran_ue_id, 8);
              gnb_asn->ran_UEID->size = 8;
              UEID_t *ueid = (UEID_t *) calloc (1, sizeof (UEID_t));
              ueid->present = UEID_PR_gNB_UEID;
              ueid->choice.gNB_UEID = gnb_asn;
              ueMeasItem->ueID = *ueid;
 
              // create Indication Message Format 1 list
              // measreport (format1) = 
              //                measdata (list (measdataitem), measrecord, list (measrecodritem))
              //              + measinfolist (list, (meastype, labelinfolist->list))
              E2SM_KPM_IndicationMessage_Format1_t *measreport =  (E2SM_KPM_IndicationMessage_Format1_t *) calloc (1, sizeof (E2SM_KPM_IndicationMessage_Format1_t));
              MeasurementData_t *measdata = (MeasurementData_t *) calloc (1, sizeof (MeasurementData_t));
              MeasurementInfoList_t * measinfolist =  (MeasurementInfoList_t *) calloc (1, sizeof (MeasurementInfoList_t));          

              for (auto mesitem : ueitem.measItems) {
                  // 1) fill up measdata
                  // measdata (list (measdataitem), measrecord, list (measrecodritem))
                  MeasurementDataItem_t *measdataitem = (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));
                  // 2) fill up measdataitem
                  MeasurementRecord_t *measrecord = (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t)); 
                  // 3) fill up measredocrd
                  MeasurementRecordItem_t *measrecorditem = (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
              
                  measrecorditem->present = MeasurementRecordItem_PR_integer; // to be change according to the meas data type
                  measrecorditem->choice.integer =  mesitem.measValue; // mease value
                  ASN_SEQUENCE_ADD (&measrecord->list, measrecorditem);
                  measdataitem->measRecord = *measrecord;
                  ASN_SEQUENCE_ADD (&measdata->list, measdataitem);
        
                  // 2-1) fill up measinfolist
                  // measinfolist (list, (meastype, labelinfolist->list))
                  MeasurementInfoItem_t * measinfoitem = (MeasurementInfoItem_t *) calloc (1, sizeof (MeasurementInfoItem_t));
                  MeasurementType_t * meastype = (MeasurementType_t *) calloc (1, sizeof (MeasurementType_t));
                  // 2-2) fill up meas type
                  meastype->present = MeasurementType_PR_measName; // to be change measID
                  std::string meas_name = mesitem.measName; // to be updating : measurementItem?     
                  MeasurementTypeName_t * meastypename = (MeasurementTypeName_t *) calloc (1, sizeof (MeasurementTypeName_t));
                  meastypename->buf = (uint8_t *) calloc (1, sizeof (OCTET_STRING));
                  meastypename->size = meas_name.length ();
                  memcpy (meastypename->buf, meas_name.c_str (), meastypename->size);
                  meastype->choice.measName = *meastypename;
                  // for loging
                  printf (" \n [Jlee] Mesaurement Label : %s ", mesitem.measName.c_str());
                  printf (" \n [Jlee] Mesaurement Label : %ld ", mesitem.measValue);


                  // 2-3) fill up labelinfolist
                  LabelInfoList_t * labelinfolist = (LabelInfoList_t *) calloc (1, sizeof (LabelInfoList_t));
                  LabelInfoItem_t * labelinfoitem = (LabelInfoItem_t *) calloc (1, sizeof (LabelInfoItem_t));
                  MeasurementLabel_t * measlabel = (MeasurementLabel_t *) calloc(1, sizeof(MeasurementLabel_t));
                  measlabel->noLabel = (long *) malloc (sizeof(long));
                  *measlabel->noLabel = 0;
                  labelinfoitem->measLabel = *measlabel;
                  assert (measlabel->noLabel != NULL && "Memory exhausted");
                  ASN_SEQUENCE_ADD (&labelinfolist->list, labelinfoitem);  
                  measinfoitem->measType = *meastype;
                  measinfoitem->labelInfoList = *labelinfolist;
                  ASN_SEQUENCE_ADD (&measinfolist->list, measinfoitem);
              } // end for measurment

              measreport->measData = *measdata;
              measreport->measInfoList = measinfolist;
              // Print ASN from Indication Message, format 1.
              NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format1, measreport));
              ueMeasItem->measReport = *measreport;

              ASN_SEQUENCE_ADD(&m_ueMeasReportList[_index++].list, ueMeasItem);
            }
            format_3->ueMeasReportList = *m_ueMeasReportList;
           // ASN_SEQUENCE_ADD (&format_3->ueMeasReportList.list, m_ueMeasReportList);
      } else {
            printf ("values.m_ueIndications.size() == 0 \n ");
            UEMeasurementReportItem_t *ueMeasItem =  (UEMeasurementReportItem_t *) calloc (1, sizeof (UEMeasurementReportItem_t));
            // To 수정
            UEID_GNB_t *gnb_asn = (UEID_GNB_t *) calloc (1, sizeof (UEID_GNB_t));
            assert (gnb_asn != NULL && "Memory exhausted");
            gnb_asn->amf_UE_NGAP_ID.buf = (uint8_t *) calloc (5, sizeof (uint8_t));
            assert (gnb_asn->amf_UE_NGAP_ID.buf != NULL && "Memory exhausted");
            asn_ulong2INTEGER (&gnb_asn->amf_UE_NGAP_ID, 1);
            gnb_asn->guami.aMFPointer = cp_amf_ptr_to_bit_string (0);
            gnb_asn->guami.aMFSetID = cp_amf_set_id_to_bit_string (1);
            gnb_asn->guami.aMFRegionID = cp_amf_region_id_to_bit_string (2 + 0);
            gnb_asn->guami.pLMNIdentity = cp_plmn_identity_to_octant_string (rand() % 505, rand() % 99, 2);
            gnb_asn->ran_UEID = (RANUEID_t *) calloc(1, sizeof(*gnb_asn->ran_UEID));
            gnb_asn->ran_UEID->buf = (uint8_t *) calloc(8, sizeof(*gnb_asn->ran_UEID->buf));
            uint8_t ran_ue_id[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            memcpy(gnb_asn->ran_UEID->buf, ran_ue_id, 8);
            gnb_asn->ran_UEID->size = 8;

            UEID_t *ueid = (UEID_t *) calloc (1, sizeof (UEID_t));
            ueid->present = UEID_PR_gNB_UEID;
            ueid->choice.gNB_UEID = gnb_asn;

            ueMeasItem->ueID = *ueid;

            MeasurementRecord_t *measrecord = (MeasurementRecord_t *) calloc (1, sizeof (MeasurementRecord_t));
            MeasurementRecordItem_t *measrecorditem = (MeasurementRecordItem_t *) calloc (1, sizeof (MeasurementRecordItem_t));
            measrecorditem->present = MeasurementRecordItem_PR_real;
            measrecorditem->choice.real = 0;//(rand() % 256) + 0.1;
            ASN_SEQUENCE_ADD (&measrecord->list, measrecorditem);

            MeasurementDataItem_t *measdataitem = (MeasurementDataItem_t *) calloc (1, sizeof (MeasurementDataItem_t));
            measdataitem->measRecord = *measrecord;

            MeasurementData_t *measdata = (MeasurementData_t *) calloc (1, sizeof (MeasurementData_t));
            ASN_SEQUENCE_ADD (&measdata->list, measdataitem);

            MeasurementType_t * meastype = (MeasurementType_t *) calloc (1, sizeof (MeasurementType_t));
            meastype->present = MeasurementType_PR_measName;
            // DRB.RlcSduDelayDl
            std::string name = "DRB.RlcSduDelayDl";
            MeasurementTypeName_t * measname = (MeasurementTypeName_t *) calloc (1, sizeof (MeasurementTypeName_t));
            measname->buf = (uint8_t *) calloc (1, sizeof (OCTET_STRING));
            measname->size = name.length ();
            memcpy (measname->buf, name.c_str (), measname->size);
            meastype->choice.measName = *measname;

            MeasurementLabel_t * measlabel = (MeasurementLabel_t *) calloc(1, sizeof(MeasurementLabel_t));
            measlabel->noLabel = (long *) malloc (sizeof(long));
            assert (measlabel->noLabel != NULL && "Memory exhausted");
            *measlabel->noLabel = 0;


            LabelInfoItem_t * labelinfoitem = (LabelInfoItem_t *) calloc (1, sizeof (LabelInfoItem_t));
            labelinfoitem->measLabel = *measlabel;

            LabelInfoList_t * labelinfolist = (LabelInfoList_t *) calloc (1, sizeof (LabelInfoList_t));
            ASN_SEQUENCE_ADD (&labelinfolist->list, labelinfoitem);

            MeasurementInfoItem_t * infoitem = (MeasurementInfoItem_t *) calloc (1, sizeof (MeasurementInfoItem_t));
            infoitem->labelInfoList = *labelinfolist;
            infoitem->measType = *meastype;

            MeasurementInfoList_t * infolist = (MeasurementInfoList_t *) calloc (1, sizeof (MeasurementInfoList_t));
            ASN_SEQUENCE_ADD (&infolist->list, infoitem);


            // 4. create Indication Message Format 1
            E2SM_KPM_IndicationMessage_Format1_t *measreport = (E2SM_KPM_IndicationMessage_Format1_t *) calloc ( 1, sizeof (E2SM_KPM_IndicationMessage_Format1_t));

            measreport->measData = *measdata;
            measreport->measInfoList = infolist;
            // Print ASN from Indication Message, format 1.
            NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format1, measreport));
            ueMeasItem->measReport = *measreport;

            // equivelent to code below -- old  code
            // ASN_SEQUENCE_ADD (&UE_data_list->list,UE_data);
            // format_3->ueMeasReportList = *UE_data_list ;
            ASN_SEQUENCE_ADD (&format_3->ueMeasReportList.list, ueMeasItem);
          }
      NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage_Format3, format_3));
      kpmindmessage->indicationMessage_formats.present = E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format3;
      kpmindmessage->indicationMessage_formats.choice.indicationMessage_Format3 = format_3;

      NS_LOG_INFO (xer_fprint (stderr, &asn_DEF_E2SM_KPM_IndicationMessage, kpmindmessage));
      Encode (kpmindmessage);
      printf (" \n *** Done Encoding INDICATION Message ****** \n ");
    }
}

MeasurementItemList::MeasurementItemList ()
{
  m_id = NULL;
}

MeasurementItemList::MeasurementItemList (std::string id)
{
  m_id = Create<OctetString> (id, id.length ());
}

MeasurementItemList::~MeasurementItemList (){};

std::vector<Ptr<MeasurementItem>>
MeasurementItemList::GetItems ()
{
  return m_items;
}

OCTET_STRING_t
MeasurementItemList::GetId ()
{
  NS_ABORT_IF (m_id == NULL);
  return m_id->GetValue ();
}

} // namespace ns3