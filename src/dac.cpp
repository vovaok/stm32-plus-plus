#include "dac.h"

Dac::Dac(Channels channels, Resolution resolution) :
    mChannels(channels),
    mEnabled(true)
{
    mResolution = resolution;
    mAlign = DAC_Align_8b_R;
    if (mResolution == Res12bit)
        mAlign = DAC_Align_12b_R;
    else if (mResolution == Res16bit)
        mAlign = DAC_Align_12b_L;

    // GPIOA clock enable (to be used with DAC)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // DAC Periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    /* DAC pin(s) configuration */
    uint32_t dacPins = ((mChannels & Channel1)? GPIO_Pin_4: 0) | 
                       ((mChannels & Channel2)? GPIO_Pin_5: 0);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = dacPins;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);
//    GPIO_ResetBits(GPIOC, GPIO_Pin_13);

    // DAC channel Configuration
    DAC_StructInit(&mConfig);
//    mConfig.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    
    // enable DAC
    if (mChannels & Channel1)
    {
        DAC_Init(DAC_Channel_1, &mConfig);
        DAC_Cmd(DAC_Channel_1, ENABLE);
    }
    if (mChannels & Channel2)
    {
        DAC_Init(DAC_Channel_2, &mConfig);
        DAC_Cmd(DAC_Channel_2, ENABLE);
    }
}

Dac::~Dac()
{
    if (mChannels & Channel1)
    {
        DAC_Cmd(DAC_Channel_1, DISABLE);
        //DAC_DeInit();
    }
    if (mChannels & Channel2)
    {
        DAC_Cmd(DAC_Channel_2, DISABLE); 
        //DAC_DeInit();
    }
    if (mChannels == ChannelBoth)
    {
        DAC_DeInit(); 
    }
}
//---------------------------------------------------------------------------

void Dac::selectTrigger(Trigger trigger)
{
    mConfig.DAC_Trigger = trigger;
    // reconfigure DAC
    if (mChannels & Channel1)
        DAC_Init(DAC_Channel_1, &mConfig);
    if (mChannels & Channel2)
        DAC_Init(DAC_Channel_2, &mConfig);
}

void Dac::setValue(unsigned short value, unsigned short value2)
{
    switch (mChannels)
    {
      case Channel1:
        DAC_SetChannel1Data(mAlign, value);
        break;
      case Channel2:
        DAC_SetChannel2Data(mAlign, value);
        break;
      case ChannelBoth:
        DAC_SetDualChannelData(mAlign, value2, value);
        break;
    };
}

void Dac::setEnabled(bool enabled)
{
    mEnabled = enabled;
    if (mChannels & Channel1)
        DAC_Cmd(DAC_Channel_1, enabled? ENABLE: DISABLE);
    if (mChannels & Channel2)
        DAC_Cmd(DAC_Channel_2, enabled? ENABLE: DISABLE);
}
//---------------------------------------------------------------------------

void Dac::configDma(Dma *dma)
{
    void *address;  
    int dataSize = mResolution==Res8bit? 1: 2;
    switch (mChannels)
    {
      case Channel1:
        address = (unsigned char*)&(DAC->DHR12R1) + mAlign;
        break;
        
      case Channel2:
        address = (unsigned char*)&(DAC->DHR12R2) + mAlign;
        break;
        
      case ChannelBoth:
        address = (unsigned char*)&(DAC->DHR12RD) + mAlign;
        break;
    }
    
    dma->setSink(address, dataSize);
    
    if (mChannels & Channel1)
        DAC_DMACmd(DAC_Channel_1, ENABLE);
    if (mChannels & Channel2)
        DAC_DMACmd(DAC_Channel_2, ENABLE);
}
//---------------------------------------------------------------------------
