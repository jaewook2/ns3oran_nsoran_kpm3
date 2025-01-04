/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2022 Northeastern University
 * Copyright (c) 2022 Sapienza, University of Rome
 * Copyright (c) 2022 University of Padova
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
 */

#include <ns3/mmwave-indication-message-helper.h>

namespace ns3 {

MmWaveIndicationMessageHelper::MmWaveIndicationMessageHelper (IndicationMessageType type,
                                                              bool isOffline, bool reducedPmValues)
    : IndicationMessageHelper (type, isOffline, reducedPmValues)
{
}

void
MmWaveIndicationMessageHelper::AddCuUpUePmItem (std::string ueImsiComplete,
                                                long txPdcpPduBytesNrRlc, long txPdcpPduNrRlc)
{
  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);
  if (!m_reducedPmValues)
    {
      // UE-specific PDCP PDU volume transmitted to NR gNB (Unit is Kbits)
      ueVal->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", txPdcpPduBytesNrRlc);

      // UE-specific number of PDCP PDUs split with NR gNB
      ueVal->AddItem<long> ("DRB.PdcpPduNbrDl.Qos.UEID", txPdcpPduNrRlc);
    }

  m_msgValues.m_ueIndications.insert (ueVal);


   if (!m_reducedPmValues)
    {
      ueMeasItem uemeasitem;
      uemeasitem.ueID = ueImsiComplete;
      MeasItem measitem;
      std::vector <MeasItem> measitems;
      // UE-specific PDCP SDU volume from LTE eNB. Unit is Mbits
      measitem.measName = "QosFlow.PdcpPduVolumeDL_Filter.UEID";
      measitem.measValue = txPdcpPduBytesNrRlc; //long
      measitems.push_back(measitem);
      // UE-specific number of PDCP SDUs from LTE eNB
      measitem.measName = "DRB.PdcpPduNbrDl.Qos.UEID";
      measitem.measValue = txPdcpPduNrRlc; //long
      measitems.push_back(measitem);

      uemeasitem.measItems = measitems;
      m_msgValues.m_UeMeasItems.push_back (uemeasitem);
      for (auto ueitem : m_msgValues.m_UeMeasItems){
        printf (" \n[Jlee-MMWAVE] UE ID : %s", ueitem.ueID.c_str());

        for (auto mesitem : ueitem.measItems) {
            printf (" \n [Jlee-MMWAVE] Mesaurement Label : %s ", mesitem.measName.c_str());
            printf (" \n [Jlee-MMWAVE] Mesaurement Label : %ld ", mesitem.measValue);
        }
      }
    }

}

void
MmWaveIndicationMessageHelper::FillCuUpValues (std::string plmId)
{
  FillBaseCuUpValues (plmId);
}

void
MmWaveIndicationMessageHelper::FillCuCpValues (uint16_t numActiveUes)
{
  FillBaseCuCpValues (numActiveUes);

}

void
MmWaveIndicationMessageHelper::FillDuValues (std::string cellObjectId)
{
  m_msgValues.m_cellObjectId = cellObjectId;
  m_msgValues.m_pmContainerValues = m_duValues;
}

void
MmWaveIndicationMessageHelper::AddDuUePmItem (
    std::string ueImsiComplete, long macPduUe, long macPduInitialUe, long macQpsk, long mac16Qam,
    long mac64Qam, long macRetx, long macVolume, long macPrb, long macMac04, long macMac59,
    long macMac1014, long macMac1519, long macMac2024, long macMac2529, long macSinrBin1,
    long macSinrBin2, long macSinrBin3, long macSinrBin4, long macSinrBin5, long macSinrBin6,
    long macSinrBin7, long rlcBufferOccup, double drbThrDlUeid)
{

  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);
  if (!m_reducedPmValues)
    {
      ueVal->AddItem<long> ("TB.TotNbrDl.1.UEID", macPduUe);
      ueVal->AddItem<long> ("TB.TotNbrDlInitial.UEID", macPduInitialUe);
      ueVal->AddItem<long> ("TB.TotNbrDlInitial.Qpsk.UEID", macQpsk);
      ueVal->AddItem<long> ("TB.TotNbrDlInitial.16Qam.UEID", mac16Qam);
      ueVal->AddItem<long> ("TB.TotNbrDlInitial.64Qam.UEID", mac64Qam);
      ueVal->AddItem<long> ("TB.ErrTotalNbrDl.1.UEID", macRetx);
      ueVal->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter.UEID", macVolume);
      ueVal->AddItem<long> ("RRU.PrbUsedDl.UEID", (long) std::ceil (macPrb));
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin1.UEID", macMac04);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin2.UEID", macMac59);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin3.UEID", macMac1014);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin4.UEID", macMac1519);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin5.UEID", macMac2024);
      ueVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin6.UEID", macMac2529);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin34.UEID", macSinrBin1);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin46.UEID", macSinrBin2);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin58.UEID", macSinrBin3);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin70.UEID", macSinrBin4);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin82.UEID", macSinrBin5);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin94.UEID", macSinrBin6);
      ueVal->AddItem<long> ("L1M.RS-SINR.Bin127.UEID", macSinrBin7);
      ueVal->AddItem<long> ("DRB.BufferSize.Qos.UEID", rlcBufferOccup);
    }

  // This value is not requested anymore, so it has been removed from the delivery, but it will be still logged;
  // ueVal->AddItem<double> ("DRB.UEThpDlPdcpBased.UEID", drbThrDlPdcpBasedUeid);
  
  ueVal->AddItem<double> ("DRB.UEThpDl.UEID", drbThrDlUeid);
  m_msgValues.m_ueIndications.insert (ueVal);

  // update Jlee
  ueMeasItem uemeasitem;
  uemeasitem.ueID = ueImsiComplete;
  MeasItem measitem;
  std::vector <MeasItem> measitems;
  measitem.measName ="DRB.UEThpDl.UEID";
  measitem.measValue = drbThrDlUeid;
  measitems.push_back(measitem);

   if (!m_reducedPmValues) // ALL LONG
    {
      measitem.measName ="TB.TotNbrDl.1.UEID";
      measitem.measValue = macPduUe;
      measitems.push_back(measitem);

      measitem.measName ="TB.TotNbrDlInitial.1.UEID";
      measitem.measValue = macPduInitialUe;
      measitems.push_back(measitem);

      ueVal->AddItem<long> ("TB.TotNbrDlInitial.Qpsk.UEID", macQpsk);
      measitem.measName ="TB.TotNbrDlInitial.Qpsk.UEID";
      measitem.measValue = macQpsk;
      measitems.push_back(measitem);

      measitem.measName ="TB.TotNbrDlInitial.16Qam.UEID";
      measitem.measValue = mac16Qam;
      measitems.push_back(measitem);

      ueVal->AddItem<long> ("TB.TotNbrDlInitial.64Qam.UEID", mac64Qam);
      measitem.measName ="TB.TotNbrDlInitial.64Qam.UEID";
      measitem.measValue = mac64Qam;
      measitems.push_back(measitem);

      measitem.measName ="TB.ErrTotalNbrDl.1.UEID";
      measitem.measValue = macRetx;
      measitems.push_back(measitem);

      measitem.measName ="QosFlow.PdcpPduVolumeDL_Filter.UEID";
      measitem.measValue = macVolume;
      measitems.push_back(measitem);

      measitem.measName ="RRU.PrbUsedDl.UEID";
      measitem.measValue = (long) std::ceil (macPrb);
      measitems.push_back(measitem);

      measitem.measName ="CARR.PDSCHMCSDist.Bin1.UEID";
      measitem.measValue = macMac04;
      measitems.push_back(measitem);

      measitem.measName ="CARR.PDSCHMCSDist.Bin2.UEID";
      measitem.measValue = macMac59;
      measitems.push_back(measitem);

      measitem.measName ="CARR.PDSCHMCSDist.Bin3.UEID";
      measitem.measValue = macMac1014;
      measitems.push_back(measitem);

      measitem.measName ="CARR.PDSCHMCSDist.Bin4.UEID";
      measitem.measValue = macMac1519;
      measitems.push_back(measitem);

      measitem.measName ="CARR.PDSCHMCSDist.Bin5.UEID";
      measitem.measValue = macMac2024;
      measitems.push_back(measitem);

      measitem.measName ="CARR.PDSCHMCSDist.Bin6.UEID";
      measitem.measValue = macMac2529;
      measitems.push_back(measitem);

      ueVal->AddItem<long> ("L1M.RS-SINR.Bin34.UEID", macSinrBin1);
      measitem.measName ="L1M.RS-SINR.Bin34.UEID";
      measitem.measValue = macSinrBin1;
      measitems.push_back(measitem);

      ueVal->AddItem<long> ("L1M.RS-SINR.Bin46.UEID", macSinrBin2);
      measitem.measName ="L1M.RS-SINR.Bin46.UEID";
      measitem.measValue = macSinrBin2;
      measitems.push_back(measitem);

      measitem.measName ="L1M.RS-SINR.Bin58.UEID";
      measitem.measValue = macSinrBin3;
      measitems.push_back(measitem);

      measitem.measName ="L1M.RS-SINR.Bin70.UEID";
      measitem.measValue = macSinrBin4;
      measitems.push_back(measitem);

      measitem.measName ="L1M.RS-SINR.Bin82.UEID";
      measitem.measValue = macSinrBin5;
      measitems.push_back(measitem);

      measitem.measName ="L1M.RS-SINR.Bin94.UEID";
      measitem.measValue = macSinrBin6;
      measitems.push_back(measitem);

      measitem.measName ="L1M.RS-SINR.Bin127.UEID";
      measitem.measValue = macSinrBin7;
      measitems.push_back(measitem);

      measitem.measName ="DRB.BufferSize.Qos.UEID";
      measitem.measValue = rlcBufferOccup;
      measitems.push_back(measitem);

    }
  uemeasitem.measItems = measitems;
  m_msgValues.m_UeMeasItems.push_back (uemeasitem);
  for (auto ueitem : m_msgValues.m_UeMeasItems){
    printf (" \n[Jlee-HLEPLER-5G_CUCP] UE ID : %s", ueitem.ueID.c_str());

    for (auto mesitem : ueitem.measItems) {
        printf (" \n [Jlee-HLEPLER-5G_CUCP] Mesaurement Label : %s ", mesitem.measName.c_str());
        printf (" \n [Jlee-HLEPLER-5G_CUCP] Mesaurement Label : %ld ", mesitem.measValue);
    }
  } 
}

void
MmWaveIndicationMessageHelper::AddDuCellPmItem (
    long macPduCellSpecific, long macPduInitialCellSpecific, long macQpskCellSpecific,
    long mac16QamCellSpecific, long mac64QamCellSpecific, double prbUtilizationDl,
    long macRetxCellSpecific, long macVolumeCellSpecific, long macMac04CellSpecific,
    long macMac59CellSpecific, long macMac1014CellSpecific, long macMac1519CellSpecific,
    long macMac2024CellSpecific, long macMac2529CellSpecific, long macSinrBin1CellSpecific,
    long macSinrBin2CellSpecific, long macSinrBin3CellSpecific, long macSinrBin4CellSpecific,
    long macSinrBin5CellSpecific, long macSinrBin6CellSpecific, long macSinrBin7CellSpecific,
    long rlcBufferOccupCellSpecific, long activeUeDl)
{
  Ptr<MeasurementItemList> cellVal = Create<MeasurementItemList> ();

  if (!m_reducedPmValues)
    {
      cellVal->AddItem<long> ("TB.TotNbrDl.1", macPduCellSpecific);
      cellVal->AddItem<long> ("TB.TotNbrDlInitial", macPduInitialCellSpecific);
    }

  cellVal->AddItem<long> ("TB.TotNbrDlInitial.Qpsk", macQpskCellSpecific);
  cellVal->AddItem<long> ("TB.TotNbrDlInitial.16Qam", mac16QamCellSpecific);
  cellVal->AddItem<long> ("TB.TotNbrDlInitial.64Qam", mac64QamCellSpecific);
  cellVal->AddItem<long> ("RRU.PrbUsedDl", (long) std::ceil (prbUtilizationDl));

  if (!m_reducedPmValues)
    {
      cellVal->AddItem<long> ("TB.ErrTotalNbrDl.1", macRetxCellSpecific);
      cellVal->AddItem<long> ("QosFlow.PdcpPduVolumeDL_Filter", macVolumeCellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin1", macMac04CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin2", macMac59CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin3", macMac1014CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin4", macMac1519CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin5", macMac2024CellSpecific);
      cellVal->AddItem<long> ("CARR.PDSCHMCSDist.Bin6", macMac2529CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin34", macSinrBin1CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin46", macSinrBin2CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin58", macSinrBin3CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin70", macSinrBin4CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin82", macSinrBin5CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin94", macSinrBin6CellSpecific);
      cellVal->AddItem<long> ("L1M.RS-SINR.Bin127", macSinrBin7CellSpecific);
      cellVal->AddItem<long> ("DRB.BufferSize.Qos", rlcBufferOccupCellSpecific);
    }

  cellVal->AddItem<long> ("DRB.MeanActiveUeDl",activeUeDl);

  m_msgValues.m_cellMeasurementItems = cellVal;

  // update Jlee
  MeasItem measitem;
   if (!m_reducedPmValues)
    {

    }

    if (!m_reducedPmValues)
    {
      measitem.measName = "TB.TotNbrDl.1";
      measitem.measValue = macPduCellSpecific;
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "TB.TotNbrDlInitial";
      measitem.measValue = macPduInitialCellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

    }

  measitem.measName = "TB.TotNbrDlInitial.Qpsk";
  measitem.measValue = macQpskCellSpecific; 
  m_msgValues.m_CellMeasItems.push_back(measitem);

  measitem.measName = "TB.TotNbrDlInitial.16Qam";
  measitem.measValue = mac16QamCellSpecific; 
  m_msgValues.m_CellMeasItems.push_back(measitem);  

  measitem.measName = "TB.TotNbrDlInitial.64Qam";
  measitem.measValue = mac64QamCellSpecific; 
  m_msgValues.m_CellMeasItems.push_back(measitem);

  measitem.measName = "RRU.PrbUsedDl";
  measitem.measValue = (long) std::ceil (prbUtilizationDl); 
  m_msgValues.m_CellMeasItems.push_back(measitem);

  if (!m_reducedPmValues)
    {
      measitem.measName = "TB.ErrTotalNbrDl.1";
      measitem.measValue = macRetxCellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);
 
      measitem.measName = "QosFlow.PdcpPduVolumeDL_Filter";
      measitem.measValue = macVolumeCellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "CARR.PDSCHMCSDist.Bin1";
      measitem.measValue = macMac04CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "CARR.PDSCHMCSDist.Bin2";
      measitem.measValue = macMac59CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "CARR.PDSCHMCSDist.Bin3";
      measitem.measValue = macMac1014CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "CARR.PDSCHMCSDist.Bin4";
      measitem.measValue = macMac1519CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "CARR.PDSCHMCSDist.Bin5";
      measitem.measValue = macMac2024CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "CARR.PDSCHMCSDist.Bin6";
      measitem.measValue = macMac2529CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "L1M.RS-SINR.Bin34";
      measitem.measValue = macSinrBin1CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "L1M.RS-SINR.Bin46";
      measitem.measValue = macSinrBin2CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "L1M.RS-SINR.Bin58";
      measitem.measValue = macSinrBin3CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "L1M.RS-SINR.Bin70";
      measitem.measValue = macSinrBin4CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "L1M.RS-SINR.Bin82";
      measitem.measValue = macSinrBin5CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "L1M.RS-SINR.Bin94";
      measitem.measValue = macSinrBin6CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "L1M.RS-SINR.Bin127";
      measitem.measValue = macSinrBin7CellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);

      measitem.measName = "DRB.BufferSize.Qos";
      measitem.measValue = rlcBufferOccupCellSpecific; 
      m_msgValues.m_CellMeasItems.push_back(measitem);
    }

  measitem.measName = "DRB.MeanActiveUeDl";
  measitem.measValue = activeUeDl; 
  m_msgValues.m_CellMeasItems.push_back(measitem);
}

void
MmWaveIndicationMessageHelper::AddDuCellResRepPmItem (Ptr<CellResourceReport> cellResRep)
{
  m_duValues->m_cellResourceReportItems.insert (cellResRep);
  /*
  measitem.measName = "DRB.MeanActiveUeDl";
  measitem.measValue = activeUeDl; 
  m_msgValues.m_CellMeasItems.push_back(measitem);
  */
}
// To update
void
MmWaveIndicationMessageHelper::AddCuCpUePmItem (std::string ueImsiComplete, long numDrb,
                                                long drbRelAct,
                                                Ptr<L3RrcMeasurements> l3RrcMeasurementServing,
                                                Ptr<L3RrcMeasurements> l3RrcMeasurementNeigh)
{

  Ptr<MeasurementItemList> ueVal = Create<MeasurementItemList> (ueImsiComplete);
  if (!m_reducedPmValues)
    {
      ueVal->AddItem<long> ("DRB.EstabSucc.5QI.UEID", numDrb);
      ueVal->AddItem<long> ("DRB.RelActNbr.5QI.UEID", drbRelAct); // not modeled in the simulator
    }

  ueVal->AddItem<Ptr<L3RrcMeasurements>> ("HO.SrcCellQual.RS-SINR.UEID", l3RrcMeasurementServing);
  ueVal->AddItem<Ptr<L3RrcMeasurements>> ("HO.TrgtCellQual.RS-SINR.UEID", l3RrcMeasurementNeigh);
  m_msgValues.m_ueIndications.insert (ueVal);

  // update Jlee

  if (!m_reducedPmValues)
    {
        ueMeasItem uemeasitem;
        uemeasitem.ueID = ueImsiComplete;
        MeasItem measitem;
        std::vector <MeasItem> measitems;
        
        measitem.measName ="DRB.EstabSucc.5QI.UEID";
        measitem.measValue = numDrb;
        measitems.push_back(measitem);

        measitem.measName = "DRB.RelActNbr.5QI.UEID";
        measitem.measValue = drbRelAct;
        measitems.push_back(measitem);

        uemeasitem.measItems = measitems;
        m_msgValues.m_UeMeasItems.push_back (uemeasitem);

    }
}
  
  // update Jlee

void
MmWaveIndicationMessageHelper::AddservSINRsValue (std::string ueImsiComplete, 
                                                  uint16_t  servCellid,  double servSINR,  double servconvertedSINR)
{
  ueMeasItem uemeasitem;
  uemeasitem.ueID = ueImsiComplete;
  MeasItem measitem;
  std::vector <MeasItem> measitems;

  measitem.measName ="servingcellID";
  measitem.measValue = servCellid ;
  measitems.push_back(measitem);

  measitem.measName = "servingSINR";
  measitem.measValue = (long) std::ceil (servSINR);
  measitems.push_back(measitem);

  measitem.measName = "servingconvertedSINR";
  measitem.measValue = (long) std::ceil (servconvertedSINR);
  measitems.push_back(measitem);

  uemeasitem.measItems = measitems;
  m_msgValues.m_UeMeasItems.push_back (uemeasitem);

  for (auto ueitem : m_msgValues.m_UeMeasItems){
    printf (" \n[Jlee-HLEPLER-5G_CUCP] UE ID : %s", ueitem.ueID.c_str());

    for (auto mesitem : ueitem.measItems) {
        printf (" \n [Jlee-HLEPLER-5G_CUCP] Mesaurement Label : %s ", mesitem.measName.c_str());
        printf (" \n [Jlee-HLEPLER-5G_CUCP] Mesaurement Value : %ld ", mesitem.measValue);
    }
  }
}

void
MmWaveIndicationMessageHelper::AddheighSINRsValue (std::string ueImsiComplete, 
                                                    uint16_t  neigCellid1,  double neigSINR1,  double neigconvertedSINR1,
                                                    uint16_t  neigCellid2,  double neigSINR2,  double neigconvertedSINR2,
                                                    uint16_t  neigCellid3,  double neigSINR3,  double neigconvertedSINR3,
                                                    uint16_t  neigCellid4,  double neigSINR4,  double neigconvertedSINR4,
                                                    uint16_t  neigCellid5,  double neigSINR5,  double neigconvertedSINR5,
                                                    uint16_t  neigCellid6,  double neigSINR6,  double neigconvertedSINR6,
                                                    uint16_t  neigCellid7,  double neigSINR7,  double neigconvertedSINR7,
                                                    uint16_t  neigCellid8,  double neigSINR8,  double neigconvertedSINR8)
{
  ueMeasItem uemeasitem;
  uemeasitem.ueID = ueImsiComplete;
  MeasItem measitem;
  std::vector <MeasItem> measitems;

  measitem.measName ="neigCellid1";
  measitem.measValue = neigCellid1 ;
  measitems.push_back(measitem);

  measitem.measName = "neigSINR1";
  measitem.measValue = (long) std::ceil (neigSINR1);
  measitems.push_back(measitem);

  measitem.measName = "neigconvertedSINR1";
  measitem.measValue = (long) std::ceil (neigconvertedSINR1);
  measitems.push_back(measitem);

  measitem.measName ="neigCellid2";
  measitem.measValue =neigCellid2 ;
  measitems.push_back(measitem);

  measitem.measName = "neigSINR2";
  measitem.measValue = (long) std::ceil (neigSINR2);
  measitems.push_back(measitem);

  measitem.measName = "neigconvertedSINR2";
  measitem.measValue = (long) std::ceil (neigconvertedSINR2);
  measitems.push_back(measitem);

  measitem.measName ="neigCellid3";
  measitem.measValue = neigCellid3 ;
  measitems.push_back(measitem);

  measitem.measName = "neigSINR3";
  measitem.measValue = (long) std::ceil (neigSINR3);
  measitems.push_back(measitem);

  measitem.measName = "neigconvertedSINR3";
  measitem.measValue = (long) std::ceil (neigconvertedSINR3);
  measitems.push_back(measitem);

    measitem.measName ="neigCellid4";
  measitem.measValue = neigCellid4 ;
  measitems.push_back(measitem);

  measitem.measName = "neigSINR4";
  measitem.measValue = (long) std::ceil (neigSINR4);
  measitems.push_back(measitem);

  measitem.measName = "neigconvertedSINR4";
  measitem.measValue = (long) std::ceil (neigconvertedSINR4);
  measitems.push_back(measitem);

  measitem.measName ="neigCellid5";
  measitem.measValue = neigCellid5;
  measitems.push_back(measitem);

  measitem.measName = "neigSINR5";
  measitem.measValue = (long) std::ceil (neigSINR5);
  measitems.push_back(measitem);

  measitem.measName = "neigconvertedSINR5";
  measitem.measValue = (long) std::ceil (neigconvertedSINR5);
  measitems.push_back(measitem);

  measitem.measName ="neigCellid6";
  measitem.measValue = neigCellid6 ;
  measitems.push_back(measitem);

  measitem.measName = "neigSINR6";
  measitem.measValue = (long) std::ceil (neigSINR6);
  measitems.push_back(measitem);

  measitem.measName = "neigconvertedSINR6";
  measitem.measValue = (long) std::ceil (neigconvertedSINR6);
  measitems.push_back(measitem);

  measitem.measName ="neigCellid7";
  measitem.measValue = neigCellid7 ;
  measitems.push_back(measitem);

  measitem.measName = "neigSINR7";
  measitem.measValue = (long) std::ceil (neigSINR7);
  measitems.push_back(measitem);

  measitem.measName = "neigconvertedSINR7";
  measitem.measValue = (long) std::ceil (neigconvertedSINR7);
  measitems.push_back(measitem);

  measitem.measName ="neigCellid8";
  measitem.measValue = neigCellid8;
  measitems.push_back(measitem);

  measitem.measName = "neigSINR8";
  measitem.measValue = (long) std::ceil (neigSINR8);
  measitems.push_back(measitem);

  measitem.measName = "neigconvertedSINR8";
  measitem.measValue = (long) std::ceil (neigconvertedSINR8);
  measitems.push_back(measitem);

  uemeasitem.measItems = measitems;
  m_msgValues.m_UeMeasItems.push_back (uemeasitem);

  for (auto ueitem : m_msgValues.m_UeMeasItems){
    printf (" \n[Jlee-HLEPLER-5G_CUCP] UE ID : %s", ueitem.ueID.c_str());

    for (auto mesitem : ueitem.measItems) {
        printf (" \n [Jlee-HLEPLER-5G_CUCP] Mesaurement Label : %s ", mesitem.measName.c_str());
        printf (" \n [Jlee-HLEPLER-5G_CUCP] Mesaurement Value : %ld ", mesitem.measValue);
    }
  }
}


MmWaveIndicationMessageHelper::~MmWaveIndicationMessageHelper ()
{
}

} // namespace ns3