#ifndef __BHT_L1_PLX9056_H__
#define __BHT_L1_PLX9056_H__

#include <bht_L0.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PCI Configuration ID*/
#define PLX9056_PCIIDR					bht_L0_u64(0x0000)
#define PLX9056_PCIIDR_VID(_r_)			bht_L0_u16((_r_) & 0xFFFF)
#define PLX9056_PCIIDR_DID(_r_)			bht_L0_u16(((_r_) >> 16) & 0xFFFF)

/* PCI Command*/
#define PLX9056_PCICR					bht_L0_u64(0x0004)
#define PLX9056_PCICR_IOS				bht_L0_u16(1 << 0)
#define PLX9056_PCICR_MS				bht_L0_u16(1 << 1)
#define PLX9056_PCICR_ME				bht_L0_u16(1 << 2)
#define PLX9056_PCICR_SC				bht_L0_u16(1 << 3)
#define PLX9056_PCICR_MWIE				bht_L0_u16(1 << 4)
#define PLX9056_PCICR_VGAPS				bht_L0_u16(1 << 5)
#define PLX9056_PCICR_PER				bht_L0_u16(1 << 6)
#define PLX9056_PCICR_WCC				bht_L0_u16(1 << 7)
#define PLX9056_PCICR_SERRE				bht_L0_u16(1 << 8)
#define PLX9056_PCICR_FBTBE				bht_L0_u16(1 << 9)

/* PCI Status*/
#define PLX9056_PCISR					bht_L0_u64(0x0006)
#define PLX9056_PCISR_NCFS				bht_L0_u16(1 << 4)
#define PLX9056_PCISR_66MHZC			bht_L0_u16(1 << 5)
#define PLX9056_PCISR_UDF				bht_L0_u16(1 << 6)
#define PLX9056_PCISR_FBTBC				bht_L0_u16(1 << 7)
#define PLX9056_PCISR_MDPED				bht_L0_u16(1 << 8)
#define PLX9056_PCISR_DEVSELT			bht_L0_u16(3 << 9)
#define PLX9056_PCISR_TA				bht_L0_u16(1 << 11)
#define PLX9056_PCISR_RTA				bht_L0_u16(1 << 12)
#define PLX9056_PCISR_RMA				bht_L0_u16(1 << 13)
#define PLX9056_PCISR_SSE				bht_L0_u16(1 << 14)
#define PLX9056_PCISR_DPE				bht_L0_u16(1 << 15)

/* PCI Revision ID*/
#define PLX9056_PCIREV					bht_L0_u64(0x0008)

/* PCI Class Code*/
#define PLX9056_PCICCR					bht_L0_u64(0x0009)
#define PLX9056_PCICCR_RLPI(_r_)		bht_L0_u8((_r_) & 0xFF)
#define PLX9056_PCICCR_SC(_r_)			bht_L0_u8(((_r_) >> 8) & 0xFF)
#define PLX9056_PCICCR_BCC(_r_)			bht_L0_u8(((_r_) >> 16) & 0xFF)

/* PCI Cache Line Size*/
#define PLX9056_PCICLSR					bht_L0_u64(0x000C)

/* PCI Bus Latency Timer*/
#define PLX9056_PCILTR					bht_L0_u64(0x000D)

/* PCI Header Type*/
#define PLX9056_PCIHTR					bht_L0_u64(0x000E)
#define PLX9056_PCIHTR_CLT				bht_L0_u8(0x7F)
#define PLX9056_PCIHTR_HT				bht_L0_u8(1 << 7)

/* PCI Built-In Self Test*/
#define PLX9056_PCIBISTR				bht_L0_u64(0x000F)
#define PLX9056_PCIBISTR_PF				bht_L0_u8(0x0F)
#define PLX9056_PCIBISTR_IE				bht_L0_u8(1 << 6)
#define PLX9056_PCIBISTR_S				bht_L0_u8(1 << 7)

/* PCI Base Address Register for Memory Accesses to Local, Runtime, and DMA*/
#define PLX9056_PCIBAR0					bht_L0_u64(0x0010)
#define PLX9056_PCIBAR0_MSI				bht_L0_u64(1 << 0)
#define PLX9056_PCIBAR0_RL				bht_L0_u64(3 << 1)
#define PLX9056_PCIBAR0_P				bht_L0_u64(1 << 3)
#define PLX9056_PCIBAR0_MBA				bht_L0_u64(0xFFFFFFF0)

/* PCI Base Address Register for I/O Accesses to Local, Runtime, and DMA*/
#define PLX9056_PCIBAR1					bht_L0_u64(0x0014)
#define PLX9056_PCIBAR1_MSI				bht_L0_u64(1 << 0)
#define PLX9056_PCIBAR1_IOBA			bht_L0_u64(0xFFFFFFFC)

/* PCI Base Address Register for Memory Accesses to Local Address Space 0*/
#define PLX9056_PCIBAR2					bht_L0_u64(0x0018)
#define PLX9056_PCIBAR2_MSI				bht_L0_u64(1 << 0)
#define PLX9056_PCIBAR2_RL				bht_L0_u64(3 << 1)
#define PLX9056_PCIBAR2_P				bht_L0_u64(1 << 3)
#define PLX9056_PCIBAR2_MBA				bht_L0_u64(0xFFFFFFF0)

/* PCI Base Address Register for Memory Accesses to Local Address Space 1*/
#define PLX9056_PCIBAR3					bht_L0_u64(0x001C)
#define PLX9056_PCIBAR3_MSI				bht_L0_u64(1 << 0)
#define PLX9056_PCIBAR3_RL				bht_L0_u64(3 << 1)
#define PLX9056_PCIBAR3_P				bht_L0_u64(1 << 3)
#define PLX9056_PCIBAR3_MBA				bht_L0_u64(0xFFFFFFF0)

#define PLX9056_PCIBAR4					bht_L0_u64(0x0020)
#define PLX9056_PCIBAR5					bht_L0_u64(0x0024)

/* PCI Cardbus CIS Pointer*/
#define PLX9056_PCICIS					bht_L0_u64(0x0028)

/* PCI Subsystem Vendor ID*/
#define PLX9056_PCISVID					bht_L0_u64(0x002C)

/* PCI Subsystem ID*/
#define PLX9056_PCISID					bht_L0_u64(0x002E)

/* PCI Expansion ROM Base*/
#define PLX9056_PCIERBAR				bht_L0_u64(0x0030)
#define PLX9056_PCIERBAR_ADE			bht_L0_u64(1 << 0)
#define PLX9056_PCIERBAR_ERBA			bht_L0_u64(0xFFFFF800)

/* New Capability Pointer*/
#define PLX9056_CAP_PTR					bht_L0_u64(0x0034)
#define PLX9056_CAP_PTR_NCP				bht_L0_u64(0xFC)

/* PCI Interrupt Line*/
#define PLX9056_PCIILR					bht_L0_u64(0x003C)

/* PCI Interrupt Pin*/
#define PLX9056_PCIIPR					bht_L0_u64(0x003D)

/* PCI Min_Gnt*/
#define PLX9056_PCIMGR					bht_L0_u64(0x003E)

/* PCI Max_Lat*/
#define PLX9056_PCIMLR					bht_L0_u64(0x003F)

/* Power Management Capability ID*/
#define PLX9056_PMCAPID					bht_L0_u64(0x0040)

/* Power Management Next Capability Pointer*/
#define PLX9056_PMNEXT					bht_L0_u64(0x0041)
#define PLX9056_PMNEXT_NCP				bht_L0_u8(0xFC)

/* Power Management Capabilities*/
#define PLX9056_PMC						bht_L0_u64(0x0042)
#define PLX9056_PMC_V					bht_L0_u16(3 << 0)
#define PLX9056_PMC_CRPMES				bht_L0_u16(1 << 3)
#define PLX9056_PMC_DSI					bht_L0_u16(1 << 5)
#define PLX9056_PMC_AUXC				bht_L0_u16(7 << 6)
#define PLX9056_PMC_D1S					bht_L0_u16(1 << 9)
#define PLX9056_PMC_D2S					bht_L0_u16(1 << 10)
#define PLX9056_PMC_PMES				bht_L0_u16(0xF800)

/* Power Management Control/Status*/
#define PLX9056_PMCSR					bht_L0_u64(0x0044)
#define PLX9056_PMCSR_PS				bht_L0_u16(3 << 0)
#define PLX9056_PMCSR_PMEEN				bht_L0_u16(1 << 8)
#define PLX9056_PMCSR_DSEL				bht_L0_u16(0xF << 9)
#define PLX9056_PMCSR_DSC				bht_L0_u16(3 << 13)
#define PLX9056_PMCSR_PMES				bht_L0_u16(1 << 15)

/* PMCSR Bridge Support Extensions*/
#define PLX9056_PMCSR_BSE				bht_L0_u64(0x0046)

/* Power Management Data*/
#define PLX9056_PMDATA					bht_L0_u64(0x0047)

/* Hot Swap Control*/
#define PLX9056_HS_CNTL					bht_L0_u64(0x0048)

/* Hot Swap Next Capability Pointer*/
#define PLX9056_HS_NEXT					bht_L0_u64(0x0049)
#define PLX9056_HS_NEXT_NCP				bht_L0_u8(0xFC)

/* Hot Swap Control/Status*/
#define PLX9056_HS_CSR					bht_L0_u64(0x004A)
#define PLX9056_HS_CSR_ENUMIC			bht_L0_u16(1 << 1)
#define PLX9056_HS_CSR_LEDSS			bht_L0_u16(1 << 3)
#define PLX9056_HS_CSR_PI				bht_L0_u16(3 << 4)
#define PLX9056_HS_CSR_BRESI			bht_L0_u16(1 << 6)
#define PLX9056_HS_CSR_BIESI			bht_L0_u16(1 << 7)

/* PCI Vital Product Data Control*/
#define PLX9056_PVPDCNTL				bht_L0_u64(0x004C)

/* PCI Vital Product Data Next Capability Pointer*/
#define PLX9056_PVPD_NEXT				bht_L0_u64(0x004D)
#define PLX9056_PVPD_NEXT_NCP			bht_L0_u8(0xFC)

/* PCI Vital Product Data Address*/
#define PLX9056_PVPDAD					bht_L0_u64(0x004E)
#define PLX9056_PVPDAD_VPDA				bht_L0_u16(0x7FFF)
#define PLX9056_PVPDAD_F				bht_L0_u16(1 << 15)

/* PCI VPD Data*/
#define PLX9056_PVPDATA					bht_L0_u64(0x0050)


/*///////////////////////////////////////////////////////////////////////////////////
// LOCAL CONFIGURATION REGISTERS
*/

/* Local Address Space 0 Range Register for PCI-to-Local Bus*/
#define PLX9056_LAS0RR					bht_L0_u64(0x0000)
#define PLX9056_LAS0RR_MSI				bht_L0_u64(1 << 0)
#define PLX9056_LAS0RR_RL				bht_L0_u64(3 << 1)
#define PLX9056_LAS0RR_P				bht_L0_u64(1 << 3)
#define PLX9056_LAS0RR_R				bht_L0_u64(0xFFFFFFF0)

/*  Local Address Space 0 Local Base Address (Remap)*/
#define PLX9056_LAS0BA					bht_L0_u64(0x0004)
#define PLX9056_LAS0BA_E				bht_L0_u64(1 << 0)
#define PLX9056_LAS0BA_R				bht_L0_u64(0xFFFFFFF0)

/*  Mode/DMA Arbitration*/
#define PLX9056_MARBR					bht_L0_u64(0x0008)
#define PLX9056_MARBR_LBLT				bht_L0_u64(0xFF)
#define PLX9056_MARBR_LBPT				bht_L0_u64(0xFF00)
#define PLX9056_MARBR_LBLTE				bht_L0_u64(1 << 16)
#define PLX9056_MARBR_LBPTE				bht_L0_u64(1 << 17)
#define PLX9056_MARBR_LBBREQE			bht_L0_u64(1 << 18)
#define PLX9056_MARBR_DMACP				bht_L0_u64(3 << 19)
#define PLX9056_MARBR_LBDSRBM			bht_L0_u64(1 << 21)
#define PLX9056_MARBR_DSLE				bht_L0_u64(1 << 22)
#define PLX9056_MARBR_PCIRM				bht_L0_u64(1 << 23)
#define PLX9056_MARBR_DRM				bht_L0_u64(1 << 24)
#define PLX9056_MARBR_PCIRNWM			bht_L0_u64(1 << 25)
#define PLX9056_MARBR_PCIRWFM			bht_L0_u64(1 << 26)
#define PLX9056_MARBR_GLBLTBREQI		bht_L0_u64(1 << 27)
#define PLX9056_MARBR_PCIRNFM			bht_L0_u64(1 << 28)
#define PLX9056_MARBR_PCIVD				bht_L0_u64(1 << 29)
#define PLX9056_MARBR_FIFOFSF			bht_L0_u64(1 << 30)
#define PLX9056_MARBR_BIGENDWIOS		bht_L0_u64(1 << 31)

/*  Big/Little Endian Descriptor*/
#define PLX9056_BIGEND					bht_L0_u64(0x000C)
#define PLX9056_BIGEND_CRBEM			bht_L0_u8(1 << 0)
#define PLX9056_BIGEND_DMBEM			bht_L0_u8(1 << 1)
#define PLX9056_BIGEND_DSAS0BEM			bht_L0_u8(1 << 2)
#define PLX9056_BIGEND_DSAER0BEM		bht_L0_u8(1 << 3)
#define PLX9056_BIGEND_BEBLM			bht_L0_u8(1 << 4)
#define PLX9056_BIGEND_DSAS1BEM			bht_L0_u8(1 << 5)
#define PLX9056_BIGEND_DMA1BEM			bht_L0_u8(1 << 6)
#define PLX9056_BIGEND_DMA0BEM			bht_L0_u8(1 << 7)

/*  Local Miscellaneous Control 1*/
#define PLX9056_LMISC1					bht_L0_u64(0x000D)
#define PLX9056_LMISC1_BAR1E			bht_L0_u8(1 << 0)
#define PLX9056_LMISC1_BAR1S			bht_L0_u8(1 << 1)
#define PLX9056_LMISC1_LIS				bht_L0_u8(1 << 2)
#define PLX9056_LMISC1_DMWFIFOF			bht_L0_u8(1 << 3)
#define PLX9056_LMISC1_DMDRE			bht_L0_u8(1 << 4)
#define PLX9056_LMISC1_TEAIIM			bht_L0_u8(1 << 5)
#define PLX9056_LMISC1_DMWFIFOAF		bht_L0_u8(1 << 6)
#define PLX9056_LMISC1_DFRFIFO			bht_L0_u8(1 << 7)

/*  Serial EEPROM Write-Protected Address Boundary*/
#define PLX9056_PROT_AREA				bht_L0_u64(0x000E)

/*  Local Miscellaneous Control 2*/
#define PLX9056_LMISC2					bht_L0_u64(0x000F)
#define PLX9056_LMISC2_RTE				bht_L0_u8(1 << 0)
#define PLX9056_LMISC2_RTS				bht_L0_u8(1 << 1)
#define PLX9056_LMISC2_DSWD				bht_L0_u8(7 << 2)
#define PLX9056_LMISC2_DSWFFC			bht_L0_u8(1 << 5)

/*  Expansion ROM Range*/
#define PLX9056_EROMRR					bht_L0_u64(0x0010)
#define PLX9056_EROMRR_ADE				bht_L0_u64(1 << 0)
#define PLX9056_EROMRR_R				bht_L0_u64(0xFFFFF800)

/*  Expansion ROM Local Base Address (Remap) and BREQo Control*/
#define PLX9056_EROMBA					bht_L0_u64(0x0014)
#define PLX9056_EROMBA_RSADC			bht_L0_u64(0xF)
#define PLX9056_EROMBA_LBBE				bht_L0_u64(1 << 4)
#define PLX9056_EROMBA_BTR				bht_L0_u64(1 << 5)
#define PLX9056_EROMBA_R				bht_L0_u64(0xFFFFF800)

/*  Local Address Space0/Expansion ROM Bus Region Descriptor*/
#define PLX9056_LBRD0					bht_L0_u64(0x0018)
#define PLX9056_LBRD0_MSLBW				bht_L0_u64(3 << 0)
#define PLX9056_LBRD0_MSIWS				bht_L0_u64(0xF << 2)
#define PLX9056_LBRD0_MSTRIE			bht_L0_u64(1 << 6)
#define PLX9056_LBRD0_MSBIE				bht_L0_u64(1 << 7)
#define PLX9056_LBRD0_MSPD				bht_L0_u64(1 << 8)
#define PLX9056_LBRD0_ERSPD				bht_L0_u64(1 << 9)
#define PLX9056_LBRD0_PCE				bht_L0_u64(1 << 10)
#define PLX9056_LBRD0_PC				bht_L0_u64(0xF << 11)
#define PLX9056_LBRD0_ERSLBW			bht_L0_u64(3 << 16)
#define PLX9056_LBRD0_ERSIWS			bht_L0_u64(0xF << 18)
#define PLX9056_LBRD0_ERSTRIE			bht_L0_u64(1 << 22)
#define PLX9056_LBRD0_ERSBIE			bht_L0_u64(1 << 23)
#define PLX9056_LBRD0_MSBE				bht_L0_u64(1 << 24)
#define PLX9056_LBRD0_ELLSE				bht_L0_u64(1 << 25)
#define PLX9056_LBRD0_ERSBE				bht_L0_u64(1 << 26)
#define PLX9056_LBRD0_DSWM				bht_L0_u64(1 << 27)
#define PLX9056_LBRD0_DSRDC				bht_L0_u64(0xF << 28)

/*  Local Range Register for Direct Master-to-PCI*/
#define PLX9056_DMRR					bht_L0_u64(0x001C)
#define PLX9056_DMRR_R					bht_L0_u64(0xFFFF0000)

/*  Local Bus Base Address Register for Direct Master-to-PCI Memory*/
#define PLX9056_DMLBAM					bht_L0_u64(0x0020)
#define PLX9056_DMLBAM_R				bht_L0_u64(0xFFFF0000)

/*  Local Base Address Register for Direct Master-to-PCI I/O Configuration*/
#define PLX9056_DMLBAI					bht_L0_u64(0x0024)
#define PLX9056_DMLBAI_R				bht_L0_u64(0xFFFF0000)

/*  PCI Base Address (Remap) Register for Direct Master-to-PCI Memory*/
#define PLX9056_DMPBAM					bht_L0_u64(0x0028)
#define PLX9056_DMPBAM_MAE				bht_L0_u64(1 << 0)
#define PLX9056_DMPBAM_IOAE				bht_L0_u64(1 << 1)
#define PLX9056_DMPBAM_CE				bht_L0_u64(1 << 2)
#define PLX9056_DMPBAM_RM				bht_L0_u64(1 << 4)
#define PLX9056_DMPBAM_MWIM				bht_L0_u64(1 << 9)
#define PLX9056_DMPBAM_PL				bht_L0_u64(1 << 11)
#define PLX9056_DMPBAM_IORS				bht_L0_u64(1 << 13)
#define PLX9056_DMPBAM_WD				bht_L0_u64(3 << 14)
#define PLX9056_DMPBAM_R				bht_L0_u64(0xFFFF0000)

/*  PCI Configuration Address Register for Direct Master-to-PCI I/O Configuration*/
#define PLX9056_DMCFGA					bht_L0_u64(0x002C)
#define PLX9056_DMCFGA_CT				bht_L0_u64(3 << 0)
#define PLX9056_DMCFGA_RN				bht_L0_u64(0xFC)
#define PLX9056_DMCFGA_FN				bht_L0_u64(7 << 8)
#define PLX9056_DMCFGA_DN				bht_L0_u64(0xF800)
#define PLX9056_DMCFGA_BN				bht_L0_u64(0xFF << 16)
#define PLX9056_DMCFGA_CE				bht_L0_u64(1 << 31)

/*  Local Address Space 1 Range Register for PCI-to-Local Bus*/
#define PLX9056_LAS1RR					bht_L0_u64(0x00F0)
#define PLX9056_LAS1RR_MSI				bht_L0_u64(1 << 0)
#define PLX9056_LAS1RR_RL				bht_L0_u64(3 << 1)
#define PLX9056_LAS1RR_P				bht_L0_u64(1 << 3)
#define PLX9056_LAS1RR_R				bht_L0_u64(0xFFFFFFF0)

/*  Local Address Space 1 Local Base Address (Remap)*/
#define PLX9056_LAS1BA					bht_L0_u64(0x00F4)
#define PLX9056_LAS1BA_E				bht_L0_u64(1 << 0)
#define PLX9056_LAS1BA_R				bht_L0_u64(0xFFFFFFF0)

/*  Local Address Space1 Bus Region Descriptor*/
#define PLX9056_LBRD1					bht_L0_u64(0x00F8)
#define PLX9056_LBRD1_MSLBW				bht_L0_u64(3 << 0)
#define PLX9056_LBRD1_MSIWS				bht_L0_u64(0xF << 2)
#define PLX9056_LBRD1_MSTRIE			bht_L0_u64(1 << 6)
#define PLX9056_LBRD1_MSBIE				bht_L0_u64(1 << 7)
#define PLX9056_LBRD1_MSBE				bht_L0_u64(1 << 8)
#define PLX9056_LBRD1_MSPD				bht_L0_u64(1 << 9)
#define PLX9056_LBRD1_PCE				bht_L0_u64(1 << 10)
#define PLX9056_LBRD1_PC				bht_L0_u64(0xF << 11)

/*  Direct Master PCI Dual Address Cycle Upper Address*/
#define PLX9056_DMDAC					bht_L0_u64(0x00FC)

/*  PCI Arbiter Control*/
#define PLX9056_PCIARB					bht_L0_u64(0x0100)
#define PLX9056_PCIARB_AE				bht_L0_u64(1 << 0)
#define PLX9056_PCIARB_HP				bht_L0_u64(1 << 1)
#define PLX9056_PCIARB_EGR				bht_L0_u64(1 << 2)
#define PLX9056_PCIARB_AP				bht_L0_u64(1 << 3)

/*  PCI Abort Address*/
#define PLX9056_PABTADR					bht_L0_u64(0x0104)


/* ///////////////////////////////////////////////////////////////////////////////////
// RUNTIME REGISTERS
*/

/*  Mailbox Register 0*/
#define PLX9056_MBOX0					bht_L0_u64(0x0078)

/*  Mailbox Register 1*/
#define PLX9056_MBOX1					bht_L0_u64(0x007C)

/*  Mailbox Register 2*/
#define PLX9056_MBOX2					bht_L0_u64(0x0048)

/*  Mailbox Register 3*/
#define PLX9056_MBOX3					bht_L0_u64(0x004C)

/*  Mailbox Register 4*/
#define PLX9056_MBOX4					bht_L0_u64(0x0050)

/*  Mailbox Register 5*/
#define PLX9056_MBOX5					bht_L0_u64(0x0054)

/*  Mailbox Register 6*/
#define PLX9056_MBOX6					bht_L0_u64(0x0058)

/*  Mailbox Register 7*/
#define PLX9056_MBOX7					bht_L0_u64(0x005C)

/*  PCI-to-Local Doorbell*/
#define PLX9056_P2LDBELL				bht_L0_u64(0x0060)

/*  Local-to-PCI Doorbell*/
#define PLX9056_L2PDBELL				bht_L0_u64(0x0064)

/*  Interrupt Control/Status*/
#define PLX9056_INTCSR					bht_L0_u64(0x0068)
#define PLX9056_INTCSR_ELBTL			bht_L0_u64(1 << 0)
#define PLX9056_INTCSR_ELBTLP			bht_L0_u64(1 << 1)
#define PLX9056_INTCSR_GSI				bht_L0_u64(1 << 2)
#define PLX9056_INTCSR_MIE				bht_L0_u64(1 << 3)
#define PLX9056_INTCSR_PMIE				bht_L0_u64(1 << 4)
#define PLX9056_INTCSR_PMI				bht_L0_u64(1 << 5)
#define PLX9056_INTCSR_DMWDSRPE			bht_L0_u64(1 << 6)
#define PLX9056_INTCSR_DMWDSRP			bht_L0_u64(1 << 7)
#define PLX9056_INTCSR_IE				bht_L0_u64(1 << 8)
#define PLX9056_INTCSR_DIE				bht_L0_u64(1 << 9)
#define PLX9056_INTCSR_AIE				bht_L0_u64(1 << 10)
#define PLX9056_INTCSR_LIIE				bht_L0_u64(1 << 11)
#define PLX9056_INTCSR_RAE				bht_L0_u64(1 << 12)
#define PLX9056_INTCSR_DIA				bht_L0_u64(1 << 13)
#define PLX9056_INTCSR_AIA				bht_L0_u64(1 << 14)
#define PLX9056_INTCSR_LIIA				bht_L0_u64(1 << 15)
#define PLX9056_INTCSR_LIOE				bht_L0_u64(1 << 16)
#define PLX9056_INTCSR_LDIE				bht_L0_u64(1 << 17)
#define PLX9056_INTCSR_DMA0IE			bht_L0_u64(1 << 18)
#define PLX9056_INTCSR_DMA1IE			bht_L0_u64(1 << 19)
#define PLX9056_INTCSR_LDIA				bht_L0_u64(1 << 20)
#define PLX9056_INTCSR_DMA0IA			bht_L0_u64(1 << 21)
#define PLX9056_INTCSR_DMA1IA			bht_L0_u64(1 << 22)
#define PLX9056_INTCSR_BISTIA			bht_L0_u64(1 << 23)
#define PLX9056_INTCSR_DMBM				bht_L0_u64(1 << 24)
#define PLX9056_INTCSR_DMA0BM			bht_L0_u64(1 << 25)
#define PLX9056_INTCSR_DMA1BM			bht_L0_u64(1 << 26)
#define PLX9056_INTCSR_TA256			bht_L0_u64(1 << 27)
#define PLX9056_INTCSR_MBOX0			bht_L0_u64(1 << 28)
#define PLX9056_INTCSR_MBOX1			bht_L0_u64(1 << 29)
#define PLX9056_INTCSR_MBOX2			bht_L0_u64(1 << 30)
#define PLX9056_INTCSR_MBOX3			bht_L0_u64(1 << 31)

/*  Serial EEPROM Control, PCI Command Codes, User I/O Control, and Init Control*/
#define PLX9056_CNTRL					bht_L0_u64(0x006C)
#define PLX9056_CNTRL_GPO				bht_L0_u64(1 << 16)
#define PLX9056_CNTRL_GPI				bht_L0_u64(1 << 17)
#define PLX9056_CNTRL_SELECT_USERI		bht_L0_u64(1 << 18)
#define PLX9056_CNTRL_SELECT_USERO		bht_L0_u64(1 << 19)
#define PLX9056_CNTRL_LINT_STATUS		bht_L0_u64(1 << 20)
#define PLX9056_CNTRL_LSERR_STATUS		bht_L0_u64(1 << 21)
#define PLX9056_CNTRL_CLOCK				bht_L0_u64(1 << 24)
#define PLX9056_CNTRL_CHIP_SELECT		bht_L0_u64(1 << 25)
#define PLX9056_CNTRL_WRITE_BIT			bht_L0_u64(1 << 26)
#define PLX9056_CNTRL_READ_BIT			bht_L0_u64(1 << 27)
#define PLX9056_CNTRL_PRESENT			bht_L0_u64(1 << 28)
#define PLX9056_CNTRL_RELOAD_CONFIG		bht_L0_u64(1 << 29)
#define PLX9056_CNTRL_RESET				bht_L0_u64(1 << 30)
#define PLX9056_CNTRL_EEDO_ENABLE		bht_L0_u64(1 << 31)

#define EE46_CMD_LEN					9
#define EE56_CMD_LEN					11
#define EE66_CMD_LEN					11
#define EE_READ							0x0180
#define EE_WRITE						0x0140
#define EE_WREN							0x0130
#define EE_WRALL						0x0110
#define EE_PRREAD						0x0180
#define EE_PRWRITE						0x0140
#define EE_WDS							0x0100
#define EE_PREN							0x0130
#define EE_PRCLEAR						0x01FF
#define EE_PRDS							0x0100

/*  PCI Hardwired Configuration ID*/
#define PLX9056_PCIHIDR					bht_L0_u64(0x0070)

/*  PCI Hardwired Revision ID*/
#define PLX9056_PCIHREV					bht_L0_u64(0x0074)


/* ///////////////////////////////////////////////////////////////////////////////////
// DMA REGISTERS
*/


/*  DMA Channel 0 Mode*/
#define PLX9056_DMAMODE0				bht_L0_u64(0x0080)
#define PLX9056_DMAMODE0_LBW			bht_L0_u64(3 << 0)
#define PLX9056_DMAMODE0_IWS			bht_L0_u64(0xF << 2)
#define PLX9056_DMAMODE0_TARIE			bht_L0_u64(1 << 6)
#define PLX9056_DMAMODE0_BIE			bht_L0_u64(1 << 7)
#define PLX9056_DMAMODE0_LBE			bht_L0_u64(1 << 8)
#define PLX9056_DMAMODE0_SGM			bht_L0_u64(1 << 9)
#define PLX9056_DMAMODE0_DIE			bht_L0_u64(1 << 10)
#define PLX9056_DMAMODE0_LAM			bht_L0_u64(1 << 11)
#define PLX9056_DMAMODE0_DM				bht_L0_u64(1 << 12)
#define PLX9056_DMAMODE0_MWIM			bht_L0_u64(1 << 13)
#define PLX9056_DMAMODE0_EOTE			bht_L0_u64(1 << 14)
#define PLX9056_DMAMODE0_FSTMS			bht_L0_u64(1 << 15)
#define PLX9056_DMAMODE0_CCM			bht_L0_u64(1 << 16)
#define PLX9056_DMAMODE0_IS				bht_L0_u64(1 << 17)
#define PLX9056_DMAMODE0_DACCL			bht_L0_u64(1 << 18)
#define PLX9056_DMAMODE0_EOTEL			bht_L0_u64(1 << 19)
#define PLX9056_DMAMODE0_VME			bht_L0_u64(1 << 20)
#define PLX9056_DMAMODE0_VSC			bht_L0_u64(1 << 21)

/*  DMA Channel 0 PCI Address*/
#define PLX9056_DMAPADR0				bht_L0_u64(0x0084)

/*  DMA Channel 0 Local Address*/
#define PLX9056_DMALADR0				bht_L0_u64(0x0088)

/*  DMA Channel 0 Transfer Size*/
#define PLX9056_DMASIZ0					bht_L0_u64(0x008C)
#define PLX9056_DMASIZ0_S				bht_L0_u64(0x007FFFFF)
#define PLX9056_DMASIZ0_V				bht_L0_u64(1 << 31)

/*  DMA Channel 0 Descriptor Pointer*/
#define PLX9056_DMADPR0					bht_L0_u64(0x0090)
#define PLX9056_DMADPR0_DL				bht_L0_u64(1 << 0)
#define PLX9056_DMADPR0_EC				bht_L0_u64(1 << 1)
#define PLX9056_DMADPR0_ITC				bht_L0_u64(1 << 2)
#define PLX9056_DMADPR0_DT				bht_L0_u64(1 << 3)
#define PLX9056_DMADPR0_NDA				bht_L0_u64(0xFFFFFFF0)

/*  DMA Channel 1 Mode*/
#define PLX9056_DMAMODE1				bht_L0_u64(0x0094)
#define PLX9056_DMAMODE1_LBW			bht_L0_u64(3 << 0)
#define PLX9056_DMAMODE1_IWS			bht_L0_u64(0xF << 2)
#define PLX9056_DMAMODE1_TARIE			bht_L0_u64(1 << 6)
#define PLX9056_DMAMODE1_BIE			bht_L0_u64(1 << 7)
#define PLX9056_DMAMODE1_LBE			bht_L0_u64(1 << 8)
#define PLX9056_DMAMODE1_SGM			bht_L0_u64(1 << 9)
#define PLX9056_DMAMODE1_DIE			bht_L0_u64(1 << 10)
#define PLX9056_DMAMODE1_LAM			bht_L0_u64(1 << 11)
#define PLX9056_DMAMODE1_DM				bht_L0_u64(1 << 12)
#define PLX9056_DMAMODE1_MWIM			bht_L0_u64(1 << 13)
#define PLX9056_DMAMODE1_EOTE			bht_L0_u64(1 << 14)
#define PLX9056_DMAMODE1_FSTMS			bht_L0_u64(1 << 15)
#define PLX9056_DMAMODE1_CCM			bht_L0_u64(1 << 16)
#define PLX9056_DMAMODE1_IS				bht_L0_u64(1 << 17)
#define PLX9056_DMAMODE1_DACCL			bht_L0_u64(1 << 18)
#define PLX9056_DMAMODE1_EOTEL			bht_L0_u64(1 << 19)
#define PLX9056_DMAMODE1_VME			bht_L0_u64(1 << 20)
#define PLX9056_DMAMODE1_VSC			bht_L0_u64(1 << 21)

/*  DMA Channel 1 PCI Address*/
#define PLX9056_DMAPADR1				bht_L0_u64(0x0098)

/*  DMA Channel 1 Local Address*/
#define PLX9056_DMALADR1				bht_L0_u64(0x009C)

/*  DMA Channel 1 Transfer Size*/
#define PLX9056_DMASIZ1					bht_L0_u64(0x00A0)
#define PLX9056_DMASIZ1_S				bht_L0_u64(0x007FFFFF)
#define PLX9056_DMASIZ1_V				bht_L0_u64(1 << 31)

/*  DMA Channel 1 Descriptor Pointer*/
#define PLX9056_DMADPR1					bht_L0_u64(0x0090)
#define PLX9056_DMADPR1_DL				bht_L0_u64(1 << 0)
#define PLX9056_DMADPR1_EC				bht_L0_u64(1 << 1)
#define PLX9056_DMADPR1_ITC				bht_L0_u64(1 << 2)
#define PLX9056_DMADPR1_DT				bht_L0_u64(1 << 3)
#define PLX9056_DMADPR1_NDA				bht_L0_u64(0xFFFFFFF0)

/*  DMA Channel 0 Command/Status*/
#define PLX9056_DMACSR0					bht_L0_u64(0x00A8)
#define PLX9056_DMACSR0_E				bht_L0_u8(1 << 0)
#define PLX9056_DMACSR0_S				bht_L0_u8(1 << 1)
#define PLX9056_DMACSR0_A				bht_L0_u8(1 << 2)
#define PLX9056_DMACSR0_CI				bht_L0_u8(1 << 3)
#define PLX9056_DMACSR0_D				bht_L0_u8(1 << 4)

/*  DMA Channel 1 Command/Status*/
#define PLX9056_DMACSR1					bht_L0_u64(0x00A9)
#define PLX9056_DMACSR1_E				bht_L0_u8(1 << 0)
#define PLX9056_DMACSR1_S				bht_L0_u8(1 << 1)
#define PLX9056_DMACSR1_A				bht_L0_u8(1 << 2)
#define PLX9056_DMACSR1_CI				bht_L0_u8(1 << 3)
#define PLX9056_DMACSR1_D				bht_L0_u8(1 << 4)

/*  DMA Arbitration*/
#define PLX9056_DMAARB					bht_L0_u64(0x00AC)

/*  DMA Threshold*/
#define PLX9056_DMATHR					bht_L0_u64(0x00B0)

/*  DMA Channel 0 PCI Dual Address Cycle Upper Address*/
#define PLX9056_DMADAC0					bht_L0_u64(0x00B4)

/*  DMA Channel 1 PCI Dual Address Cycle Upper Address*/
#define PLX9056_DMADAC1					bht_L0_u64(0x00B8)


/*///////////////////////////////////////////////////////////////////////////////////
// MESSAGING QUEUE REGISTERS
*/

/*  Outbound Post Queue Interrupt Status*/
#define PLX9056_OPQIS					bht_L0_u64(0x0030)
#define PLX9056_OPQIS_I					bht_L0_u64(1 << 3)

/*  Outbound Post Queue Interrupt Mask*/
#define PLX9056_OPQIM					bht_L0_u64(0x0034)
#define PLX9056_OPQIM_M					bht_L0_u64(1 << 3)

/*  Inbound Queue Port*/
#define PLX9056_IQP						bht_L0_u64(0x0040)

/*  Outbound Queue Port*/
#define PLX9056_OQP						bht_L0_u64(0x0044)

/*  Messaging Queue Configuration*/
#define PLX9056_MQCR					bht_L0_u64(0x00C0)

/*  Queue Base Address*/
#define PLX9056_QBAR					bht_L0_u64(0x00C4)

/*  Inbound Free Head Pointer*/
#define PLX9056_IFHPR					bht_L0_u64(0x00C8)

/*  Inbound Free Tail Pointer*/
#define PLX9056_IFTPR					bht_L0_u64(0x00CC)

/*  Inbound Post Head Pointer*/
#define PLX9056_IPHPR					bht_L0_u64(0x00D0)

/*  Inbound Post Tail Pointer*/
#define PLX9056_IPTPR					bht_L0_u64(0x00D4)

/*  Outbound Free Head Pointer*/
#define PLX9056_OFHPR					bht_L0_u64(0x00D8)

/*  Outbound Free Tail Pointer*/
#define PLX9056_OFTPR					bht_L0_u64(0x00DC)

/* Outbound Post Head Pointer*/
#define PLX9056_OPHPR					bht_L0_u64(0x00E0)

/* Outbound Post Tail Pointer*/
#define PLX9056_OPTPR					bht_L0_u64(0x00E4)

/* Queue Status/Control*/
#define PLX9056_QSR						bht_L0_u64(0x00E8)




/* Definition of abort bits in commad (CMD) configuration register */
#define PLX_M_CR_ABORT_BITS (\
    (0x1 << 24)   /* Detected Master Data Parity Error */ \
    | (0x1 << 27) /* Signaled Target Abort */ \
    | (0x1 << 28) /* Received Target Abort */ \
    | (0x1 << 29) /* Received Master Abort */ \
    | (0x1 << 30) /* Signaled System Error */ \
    | (0x1 << 31) /* Detected Parity Error on PCI bus */ \
    )

#ifdef __cplusplus
}
#endif

#endif
