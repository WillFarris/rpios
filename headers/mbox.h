#pragma once

extern volatile unsigned int mbox[36];

#define MBOX_REQUEST    0

/* channels */
enum MailboxChannel
{
    MBoxChannelPower = 0,
    MBoxChannelFB    = 1,
    MBoxChannelUART  = 2,
    MBoxChannelVCHIQ = 3,
    MBoxChannelLEDS  = 4,
    MBoxChannelBTNS  = 5,
    MBoxChannelTOUCH = 6,
    MBoxChannelCOUNT = 7,
    MBoxChannelPROP  = 8,
};

/* tags */
#define MBOX_TAG_SETPOWER       0x28001
#define MBOX_TAG_SETCLKRATE     0x38002
#define MBOX_TAG_LAST           0

int mbox_call(unsigned char ch);