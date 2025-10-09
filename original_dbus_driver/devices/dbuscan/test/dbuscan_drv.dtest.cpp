/*******************************************************************************
*  Copyright (c) 2024 BSH Hausgeraete GmbH,
*  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*  All rights reserved. This program and the accompanying materials
*  are protected by international copyright laws.
*  Please contact copyright holder for licensing information.
*
*******************************************************************************/

#include <doctest/doctest.h>
#include <trompeloeil/doctest/trompeloeil.hpp>
/* include your required mock headers here. Lowest SW layer first!*/


using trompeloeil::_;

namespace dbuscan_drv_test
{   

    /*----- START: mocking own internal (static) and external interface functions (optional!) *****/
    /* Note: you may delete this block (start to end) if no internal or external mocks are needed */
    #include "../dbuscan_drv.h"
    /* To mock internal functions the following macros are used in the code under test:
     * MOCKI(function-name)(paramA, paramB,...)
     * The parameters are the same as in the original function.
     * For own external functions the macro MOCKE(function-name)(paramA, paramB,...) is used.
     */

    class
    {/* no. params function name                   return type         parameter list */
      //MAKE_MOCK0(DBCDRV_isValid                  , bool              ( void ));
    }int_mock;

    class
    {/* no. params function name                   return type         parameter list */
      //MAKE_MOCK0(DBCDRV_getMextiChannel        , const MEXTI_Channel* ( void ));
    }ext_mock;

    /*---- END: mocking own functions ------------------------------------------------------------*/




    #include "../dbuscan_drv.c"

    void test_setup(void)
    {
        /* Optional interface 
           Do here here everything to prepare the tests.
           E.g. common variable initialization.
           Note: The final test result shall be independent of the test order
                 This is why you should prepare each test case.
        */
    }   


    TEST_SUITE("dbuscan_drv") // the test suite for the unit dbuscan_drv
    {
        // here you can declare variables and constants you need in all test cases within this test suite.

        TEST_CASE("DBCDRV_getMextiChannel")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //const struct MEXTI_Channel* DBCDRV_getMextiChannel(void)
                //const struct MEXTI_Channel* ret = DBCDRV_getMextiChannel(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_getMspiFrequency")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //uint32_t DBCDRV_getMspiFrequency(void)
                //uint32_t ret = DBCDRV_getMspiFrequency(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_getConfig")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //DBC_Cfg_t DBCDRV_getConfig(void)
                //DBC_Cfg_t ret = DBCDRV_getConfig(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_initComChannels")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_initComChannels(MCAL_CallbackFunction_t irqHandleCbFunc)
                //enum DBC_Error ret = DBCDRV_initComChannels(irqHandleCbFunc);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_init")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_init(MCAL_CallbackFunction_t irqHandleCbFunc)
                //enum DBC_Error ret = DBCDRV_init(irqHandleCbFunc);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_disableIrq")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //void DBCDRV_disableIrq(void)
                //DBCDRV_disableIrq(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_HandleTask")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //uint8_t DBCDRV_HandleTask(void)
                //uint8_t ret = DBCDRV_HandleTask(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_isPowerOnReset")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //bool DBCDRV_isPowerOnReset(void)
                //bool ret = DBCDRV_isPowerOnReset(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_notifyPowerOnReset")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //void DBCDRV_notifyPowerOnReset(void)
                //DBCDRV_notifyPowerOnReset(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_reset")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_reset(enum DBC_Reset rstType)
                //enum DBC_Error ret = DBCDRV_reset(rstType);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_spiReset")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //void DBCDRV_spiReset(void)
                //DBCDRV_spiReset(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_isResetFinished")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //bool DBCDRV_isResetFinished(void)
                //bool ret = DBCDRV_isResetFinished(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_setPowerMode")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_setPowerMode(enum DBC_PowerMode mode)
                //enum DBC_Error ret = DBCDRV_setPowerMode(mode);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_write")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_write(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_WriteCmd cmd)
                //enum DBC_Error ret = DBCDRV_write(addr, writeBuf, len, cmd);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_writeNbl")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_writeNbl(enum DBC_RegAddr addr, DBC_WriteBuf_t *writeBuf, uint16_t len, enum DBC_WriteCmd cmd)
                //enum DBC_Error ret = DBCDRV_writeNbl(addr, writeBuf, len, cmd);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_read")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_read(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_ReadCmd cmd)
                //enum DBC_Error ret = DBCDRV_read(addr, readBuf, len, cmd);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_readNbl")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_readNbl(enum DBC_RegAddr addr, uint8_t *readBuf, uint16_t len, enum DBC_ReadCmd cmd)
                //enum DBC_Error ret = DBCDRV_readNbl(addr, readBuf, len, cmd);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_writeReg32")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_writeReg32(enum DBC_RegAddr addr, uint32_t data)
                //enum DBC_Error ret = DBCDRV_writeReg32(addr, data);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_writeReg32Nbl")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_writeReg32Nbl(enum DBC_RegAddr addr, uint32_t data)
                //enum DBC_Error ret = DBCDRV_writeReg32Nbl(addr, data);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_readReg32")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //uint32_t DBCDRV_readReg32(enum DBC_RegAddr addr)
                //uint32_t ret = DBCDRV_readReg32(addr);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_readReg32Nbl")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_readReg32Nbl(enum DBC_RegAddr addr)
                //enum DBC_Error ret = DBCDRV_readReg32Nbl(addr);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_getReadReg32Nbl")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //uint32_t DBCDRV_getReadReg32Nbl(void)
                //uint32_t ret = DBCDRV_getReadReg32Nbl(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_writeRegIpec")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_writeRegIpec(uint32_t bitVal, uint32_t bitPos, uint32_t bitMask)
                //enum DBC_Error ret = DBCDRV_writeRegIpec(bitVal, bitPos, bitMask);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_isIrqEvent")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //bool DBCDRV_isIrqEvent(void)
                //bool ret = DBCDRV_isIrqEvent(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_readIrq")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_readIrq(union DBC_IrqBuf *irqBuf)
                //enum DBC_Error ret = DBCDRV_readIrq(irqBuf);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_enableCfgDbus")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_enableCfgDbus(void)
                //enum DBC_Error ret = DBCDRV_enableCfgDbus(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_disableCfgDbus")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_disableCfgDbus(void)
                //enum DBC_Error ret = DBCDRV_disableCfgDbus(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_convertDbusBaudValue")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //uint16_t DBCDRV_convertDbusBaudValue(uint16_t baudrate)
                //uint16_t ret = DBCDRV_convertDbusBaudValue(baudrate);
                CHECK(true);
            }
        }

        TEST_CASE("DBCDRV_setDbusBaudrate")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCDRV_setDbusBaudrate(uint16_t baudrate)
                //enum DBC_Error ret = DBCDRV_setDbusBaudrate(baudrate);
                CHECK(true);
            }
        }

    } // TEST_SUITE("dbuscan_drv")
} // namespace dbuscan_drv_test

