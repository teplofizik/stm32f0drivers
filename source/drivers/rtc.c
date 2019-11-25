// ***********************************************************
//  rtc.c
// 	Управление часами v2.0.0
//  Таймеры
//
//  Требования: clock
//  teplofizik, 2019
// ***********************************************************

#include "rtc.h"
#include "clock.h"
#include <stm32f0xx.h>

#define MCU_YEAR_OFFSET 100

#define RTC_WRITE_PROTECTION_KEY1   (0xCA)
#define RTC_WRITE_PROTECTION_KEY2   (0x53)
#define RTC_SYNC_PRESCALER          (0xff) /**< prescaler for 32.768 kHz oscillator */
#define RTC_ASYNC_PRESCALER         (0x7f) /**< prescaler for 32.768 kHz oscillator */

bool Reinit = false;

/**
 * Convert a number from unsigned to BCD
 *
 * @param[in] value to be converted
 * @return BCD representation of the value
 */
static uint8_t byte2bcd(uint8_t value)
{
  uint8_t bcdhigh = 0;

  while (value >= 10)
  {
    bcdhigh++;
    value -= 10;
  }

  return  ((uint8_t)(bcdhigh << 4) | value);
}

static void SetLocalTime(struct tm *time)
{
	if(RTC->CR & RTC_CR_FMT) RTC->CR &= ~RTC_CR_FMT;
    RTC->DR = ( (((uint32_t)byte2bcd(time->tm_year - MCU_YEAR_OFFSET) << 16) & (RTC_DR_YT | RTC_DR_YU) ) |
                (((uint32_t)byte2bcd(time->tm_mon+1)<<  8) & (RTC_DR_MT | RTC_DR_MU) ) |
                (((uint32_t)byte2bcd(time->tm_mday) <<  0) & (RTC_DR_DT | RTC_DR_DU) ) );

    RTC->TR = ( (((uint32_t)byte2bcd(time->tm_hour) << 16) & (RTC_TR_HT | RTC_TR_HU) ) |
                (((uint32_t)byte2bcd(time->tm_min)  <<  8) & (RTC_TR_MNT| RTC_TR_MNU)) |
                (((uint32_t)byte2bcd(time->tm_sec)  <<  0) & (RTC_TR_ST | RTC_TR_SU) ) );
}


static bool Init(void)
{
    if(!drv_Require(&rtc, "clock")) return false;
	clock_EnablePeripheral(PR_PWR);
	
	PWR->CR |= PWR_CR_DBP;
	
	if(((RTC->ISR & RTC_ISR_INITS) == 0) || Reinit)
	{
		volatile int Timeout = 20000;
		/* Enable the LSE clock (external 32.768 kHz oscillator) */
		RCC->BDCR &= ~(RCC_BDCR_LSEON);
		RCC->BDCR &= ~(RCC_BDCR_LSEBYP);
		RCC->BDCR |= RCC_BDCR_LSEON;
		while (Timeout-- && ((RCC->BDCR & RCC_BDCR_LSERDY) == 0));

		if(RCC->BDCR & RCC_BDCR_LSERDY)
		{
			/* Switch RTC to LSE clock source */
			RCC->BDCR &= ~(RCC_BDCR_RTCSEL);
			RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;

			/* Enable the RTC */
			RCC->BDCR |= RCC_BDCR_RTCEN;

			/* Unlock RTC write protection */
			RTC->WPR = RTC_WRITE_PROTECTION_KEY1;
			RTC->WPR = RTC_WRITE_PROTECTION_KEY2;
			
			/* Enter RTC Init mode */
			RTC->ISR = 0;
			RTC->ISR |= RTC_ISR_INIT;
			while ( (RTC->ISR & RTC_ISR_INITF) == 0 );
			
			/* Set 24-h clock */
		//	RTC->CR |= RTC_CR_FMT;
			/* Timestamps enabled */
			RTC->CR |= RTC_CR_TSE;
			
			/* Configure the RTC PRER */
			RTC->PRER = RTC_SYNC_PRESCALER;
			RTC->PRER |= (RTC_ASYNC_PRESCALER << 16);

			{
				struct tm time;
				time.tm_mday = 15;
				time.tm_mon = 1;
				time.tm_year = 19;
				SetLocalTime(&time);
			}
			/* Exit RTC init mode */
			RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

			/* Enable RTC write protection */
			RTC->WPR = 0xff;
		}
		else
			return false;
	}
    return true;
}

bool rtc_Available(void)
{
	return (RTC->ISR & RTC_ISR_INITS) != 0;
}

int rtc_SetTimeDebug(uint32_t Time, uint32_t Date, uint8_t Mask)
{   
	/* Enable write access to RTC registers */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    /* Unlock RTC write protection */
    RTC->WPR = RTC_WRITE_PROTECTION_KEY1;
    RTC->WPR = RTC_WRITE_PROTECTION_KEY2;

    /* Enter RTC Init mode */
    RTC->ISR |= RTC_ISR_INIT;
    while ( (RTC->ISR & RTC_ISR_INITF) == 0 );

	if(Mask & 0x01) RTC->TR = Time;
	if(Mask & 0x02) RTC->DR = Date;
	
    /* Exit RTC init mode */
    RTC->ISR &= (uint32_t)~RTC_ISR_INIT;
    /* Enable RTC write protection */
    RTC->WPR = 0xFF;
    return 0;
}

int rtc_SetTime(struct tm *time)
{
    /* Enable write access to RTC registers */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    /* Unlock RTC write protection */
    RTC->WPR = RTC_WRITE_PROTECTION_KEY1;
    RTC->WPR = RTC_WRITE_PROTECTION_KEY2;

    /* Enter RTC Init mode */
    RTC->ISR |= RTC_ISR_INIT;
    while ( (RTC->ISR & RTC_ISR_INITF) == 0 );

	SetLocalTime(time);

    /* Exit RTC init mode */
    RTC->ISR &= (uint32_t)~RTC_ISR_INIT;
    /* Enable RTC write protection */
    RTC->WPR = 0xFF;
    return 0;
}

int rtc_GetTime(struct tm *time)
{
    time->tm_year = MCU_YEAR_OFFSET;
    time->tm_year +=(((RTC->DR & RTC_DR_YT)  >> 20) * 10) + ((RTC->DR & RTC_DR_YU)  >> 16);
    time->tm_mon  = (((RTC->DR & RTC_DR_MT)  >> 12) * 10) + ((RTC->DR & RTC_DR_MU)  >>  8) - 1;
    time->tm_mday = (((RTC->DR & RTC_DR_DT)  >>  4) * 10) + ((RTC->DR & RTC_DR_DU)  >>  0);
    time->tm_hour = (((RTC->TR & RTC_TR_HT)  >> 20) * 10) + ((RTC->TR & RTC_TR_HU)  >> 16);
    if ( RTC->TR & RTC_TR_PM )
        time->tm_hour += 12;
    time->tm_min  = (((RTC->TR & RTC_TR_MNT) >> 12) * 10) + ((RTC->TR & RTC_TR_MNU) >>  8);
    time->tm_sec  = (((RTC->TR & RTC_TR_ST)  >>  4) * 10) + ((RTC->TR & RTC_TR_SU)  >>  0);
    return 0;
}


static void Uninit(void)
{
	clock_DisablePeripheral(PR_PWR);
}

const TDriver rtc = { "rtc", &Init, &Uninit, 0 };
