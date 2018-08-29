#ifndef __SPRAIN_TIMERS_H__
#define __SPRAIN_TIMERS_H__

#ifdef __cplusplus
extern "C"{
#include "stm32f40x_timer.h"
}

class SPRain_TIMER
{
public:
    /**
     * @brief Timer_Application�๹�캯��
     * @param TIMER�ı��(1~8)
     */
    SPRain_TIMER(uint8_t timerNum);
	/**
     * @brief ֹͣ����
     */
    void pause(void);
	/**
     * @brief �ָ�һ��ֹͣ�Ķ�ʱ��
     */
    void resume(void);
	/**
     * @brief ��ȡ��ʱ���ķ�Ƶϵ��
     * @return Timer prescaler, from 1 to 65,536.
     */
    uint32_t getPrescaleFactor();
	/**
     * @brief ���ö�ʱ���ķ�Ƶϵ��
     *
     * @param factor The new prescale value to set, from 1 to 65,536.
	 * @param  TIM_PSCReloadMode: specifies the TIM Prescaler Reload mode
	 *   This parameter can be one of the following values:
	 *     @arg TIM_PSCReloadMode_Update: The Prescaler is loaded at the update event.
	 *     @arg TIM_PSCReloadMode_Immediate: The Prescaler is loaded immediately.
     */
	void setPrescaleFactor(uint32_t factor, uint16_t TIM_PSCReloadMode);
	/**
     * @brief �õ���ʱ�����ֵ
     */
    uint32_t getOverflow();
	/**
     * @brief ���ö�ʱ����װ��ֵ
     * @param val The new overflow value to set
     */
	void setOverflow(uint32_t val);
	/**
     * @brief ��õ�ǰ��ʱ������ֵ
     *
     * @return The timer's current count value
     */
    uint32_t getCount(void);

    /**
     * @brief ���ö�ʱ����ǰ����ֵ
     *
     * @param val The new count value to set.  If this value exceeds
     *            the timer's overflow value, it is truncated to the
     *            overflow value.
     */
    void setCount(uint32_t val);

    /**
     * @brief ��ָ��ʱ����΢��Ϊ��λ
     * @param microseconds The desired period of the timer.  This must be
     *                     greater than zero.
     * @return The new overflow value.
     */
    int setPeriod(uint32_t microseconds);

    /**
     * @brief �趨һ����ʱ��ͨ��ģʽ
     * @param channel Timer channel, from 1 to 4
     * @param mode Mode to set
     */
    void setMode(uint8_t channel, timer_mode mode);

    /**
     * @brief ȡ�ö�Ӧͨ���ıȽ�ֵ
     */
    uint16_t getCompare(uint8_t channel);

    /**
     * @brief ���ö�Ӧͨ���ıȽ�ֵ
     *
     * @param channel the channel whose compare to set, from 1 to 4.
     * @param compare The compare value to set.  If greater than this
     *                timer's overflow value, it will be truncated to
     *                the overflow value.
     */
    void setCompare(uint8_t channel, uint16_t compare);

    /**
     * @brief �����жϺ���
     *
     * @param channel the channel to attach the ISR to, from 1 to 4.
     * @param handler The ISR to attach to the given channel.
     * @see voidFuncPtr
     */
    void attachInterrupt(uint8_t interrupt, voidFuncPtr handler);

    /**
     * @brief ע���жϺ���
     *
     * The handler will no longer be called by this timer.
     *
     * @param channel the channel whose interrupt to detach, from 1 to 4.
     * @see HardwareTimer::attachInterrupt()
     */
    void detachInterrupt(uint8_t interrupt);

    /**
     * @brief ���ö�ʱ��
     */
    void refresh(void);
    
    /**
     * @brief ���ö�ʱ���ж�
     */
    void setPriority(uint8_t priority);
    
    /**
      * @brief  Specifies the TIMx Counter Mode to be used.
      * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
      * @param  TIM_CounterMode: specifies the Counter Mode to be used
      *          This parameter can be one of the following values:
      *            @arg TIM_CounterMode_Up: TIM Up Counting Mode
      *            @arg TIM_CounterMode_Down: TIM Down Counting Mode
      *            @arg TIM_CounterMode_CenterAligned1: TIM Center Aligned Mode1
      *            @arg TIM_CounterMode_CenterAligned2: TIM Center Aligned Mode2
      *            @arg TIM_CounterMode_CenterAligned3: TIM Center Aligned Mode3
      * @retval None
      */
    void setCounterMode(uint16_t TIM_CounterMode);
    /**
      * @brief  Sets the TIMx Clock Division value.
      * @param  TIMx: where x can be 1 to 14 except 6 and 7, to select the TIM peripheral.
      * @param  TIM_CKD: specifies the clock division value.
      *          This parameter can be one of the following value:
      *            @arg TIM_CKD_DIV1: TDTS = Tck_tim
      *            @arg TIM_CKD_DIV2: TDTS = 2*Tck_tim
      *            @arg TIM_CKD_DIV4: TDTS = 4*Tck_tim
      * @retval None
      */
    void setClockDiv(uint16_t TIM_CKD);

    
    /* ------------------------------------ ������ʹ�����г�Ա���� ----------------------------------------------- */

    /** @brief Deprecated; use setMode(channel, mode) instead. */
    void setChannelMode(uint8_t channel, timer_mode mode) 
	{
        setMode(channel, mode);
    }

    /** @brief Deprecated; use setMode(TIMER_CH1, mode) instead. */
    void setChannel1Mode(timer_mode mode) { setMode(TIMER_CC1, mode); }

    /** @brief Deprecated; use setMode(TIMER_CH2, mode) instead. */
    void setChannel2Mode(timer_mode mode) { setMode(TIMER_CC2, mode); }

    /** @brief Deprecated; use setMode(TIMER_CH3, mode) instead. */
    void setChannel3Mode(timer_mode mode) { setMode(TIMER_CC3, mode); }

    /** @brief Deprecated; use setMode(TIMER_CH4, mode) instead. */
    void setChannel4Mode(timer_mode mode) { setMode(TIMER_CC4, mode); }

    /** @brief Deprecated; use return getCompare(TIMER_CH1) instead. */
    uint16_t getCompare1() { return getCompare(TIMER_CC1); }

    /** @brief Deprecated; use return getCompare(TIMER_CH2) instead. */
    uint16_t getCompare2() { return getCompare(TIMER_CC2); }

    /** @brief Deprecated; use return getCompare(TIMER_CH3) instead. */
    uint16_t getCompare3() { return getCompare(TIMER_CC3); }

    /** @brief Deprecated; use return getCompare(TIMER_CH4) instead. */
    uint16_t getCompare4() { return getCompare(TIMER_CC4); }

    /** @brief Deprecated; use setCompare(TIMER_CH1, compare) instead. */
    void setCompare1(uint16_t compare) { setCompare(TIMER_CC1, compare); }

    /** @brief Deprecated; use setCompare(TIMER_CH2, compare) instead. */
    void setCompare2(uint16_t compare) { setCompare(TIMER_CC2, compare); }

    /** @brief Deprecated; use setCompare(TIMER_CH3, compare) instead. */
    void setCompare3(uint16_t compare) { setCompare(TIMER_CC3, compare); }

    /** @brief Deprecated; use setCompare(TIMER_CH4, compare) instead. */
    void setCompare4(uint16_t compare) { setCompare(TIMER_CC4, compare); }

    /** @brief Deprecated; use attachInterrupt(TIMER_CH1, handler) instead. */
    void attachCompare1Interrupt(voidFuncPtr handler) {
        attachInterrupt(TIMER_CC1, handler);
    }

    /** @brief Deprecated; use attachInterrupt(TIMER_CH2, handler) instead. */
    void attachCompare2Interrupt(voidFuncPtr handler) {
        attachInterrupt(TIMER_CC2, handler);
    }

    /** @brief Deprecated; use attachInterrupt(TIMER_CH3, handler) instead. */
    void attachCompare3Interrupt(voidFuncPtr handler) {
        attachInterrupt(TIMER_CC3, handler);
    }

    /** @brief Deprecated; use attachInterrupt(TIMER_CH4, handler) instead. */
    void attachCompare4Interrupt(voidFuncPtr handler) {
        attachInterrupt(TIMER_CC4, handler);
    }

    /** @brief Deprecated; use detachInterrupt(TIMER_CH1) instead. */
    void detachCompare1Interrupt(void) { detachInterrupt(TIMER_CC1); }

    /** @brief Deprecated; use detachInterrupt(TIMER_CH2) instead. */
    void detachCompare2Interrupt(void) { detachInterrupt(TIMER_CC2); }

    /** @brief Deprecated; use detachInterrupt(TIMER_CH3) instead. */
    void detachCompare3Interrupt(void) { detachInterrupt(TIMER_CC3); }

    /** @brief Deprecated; use detachInterrupt(TIMER_CH4) instead. */
    void detachCompare4Interrupt(void) { detachInterrupt(TIMER_CC4); }

    /** @brief Deprecated; use refresh() instead. */
    void generateUpdate(void) { refresh(); }
private:
    // ��ʱ�������ṹ��
    timer_dev *dev;
};

extern SPRain_TIMER SP_Timer3;

#endif
#endif