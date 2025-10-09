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

namespace dbuscan_bl_test
{   

    /*----- START: mocking own internal (static) and external interface functions (optional!) *****/
    /* Note: you may delete this block (start to end) if no internal or external mocks are needed */
    #include "../dbuscan_bl.h"
    /* To mock internal functions the following macros are used in the code under test:
     * MOCKI(function-name)(paramA, paramB,...)
     * The parameters are the same as in the original function.
     * For own external functions the macro MOCKE(function-name)(paramA, paramB,...) is used.
     */

    class
    {/* no. params function name                   return type         parameter list */
      //MAKE_MOCK0(DBCBL_isValid                  , bool              ( void ));
    }int_mock;

    class
    {/* no. params function name                   return type         parameter list */
      //MAKE_MOCK1(DBCBL_readData                , uint8_t*          ( uint16_t dataSize ));
    }ext_mock;

    /*---- END: mocking own functions ------------------------------------------------------------*/




    #include "../dbuscan_bl.c"

    void test_setup(void)
    {
        /* Optional interface 
           Do here here everything to prepare the tests.
           E.g. common variable initialization.
           Note: The final test result shall be independent of the test order
                 This is why you should prepare each test case.
        */
    }   


    TEST_SUITE("dbuscan_bl") // the test suite for the unit dbuscan_bl
    {
        // here you can declare variables and constants you need in all test cases within this test suite.

        TEST_CASE("DBCBL_readData")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //uint8_t* DBCBL_readData(uint16_t dataSize)
                //uint8_t* ret = DBCBL_readData(dataSize);
                CHECK(true);
            }
        }

        TEST_CASE("DBCBL_sendData")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //enum DBC_Error DBCBL_sendData(const uint8_t *dataBuf, uint16_t dataSize)
                //enum DBC_Error ret = DBCBL_sendData(dataBuf, dataSize);
                CHECK(true);
            }
        }

        TEST_CASE("DBCBL_isTxOngoing")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //bool DBCBL_isTxOngoing(void)
                //bool ret = DBCBL_isTxOngoing(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCBL_clearIrq")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //bool DBCBL_clearIrq(void)
                //bool ret = DBCBL_clearIrq(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCBL_readTxStatus")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //void DBCBL_readTxStatus(void)
                //DBCBL_readTxStatus(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCBL_clearRxFifo")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //void DBCBL_clearRxFifo(void)
                //DBCBL_clearRxFifo(void);
                CHECK(true);
            }
        }

        TEST_CASE("DBCBL_getRxDataSize")
        {
            test_setup(); // prepare the test

            SUBCASE("Check....")
            {   //uint16_t DBCBL_getRxDataSize(void)
                //uint16_t ret = DBCBL_getRxDataSize(void);
                CHECK(true);
            }
        }

    } // TEST_SUITE("dbuscan_bl")
} // namespace dbuscan_bl_test

