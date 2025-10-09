/*******************************************************************************
 *  Copyright (c) 2024 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW Test
 ******************************************************************************/

#include <doctest/doctest.h>
#include "spi_flash.mock.h"
#include "../abs_spi_flash.c"
#include <vector>
#include <algorithm>

using trompeloeil::_;
using trompeloeil::eq; // matching equal values
using std::vector;

/* might be moved to abs_spi_flash.c later, currently it is used for test only */
enum ABSSF_jedecmanufacturerID
{
    ABSSF_Macronix = 0xC2,
    ABSSF_Winbond  = 0xDA
    
};



TEST_SUITE("Spi-Flash")
{
    trompeloeil::sequence seq;
    vector<uint8_t> cmd(1);
    vector<uint8_t> cmdWrEnable{ABSSF_cmdWrEnable};

    TEST_CASE("ABSSF_init: Do the initialization, and read ID form device")
    {
        const uint8_t Memory_Type    = 0x20;
        const uint8_t Memory_density = 0x15;  //16MBit
        vector<uint8_t> rdIdent{ABSSF_Macronix, Memory_Type, Memory_density};
        cmd[0]= ABSSF_cmdRdId;

    /* following expected calls in this order (sequence:seq) */
        REQUIRE_CALL(flashCfgMock, ABSSF_initInterface())                 .IN_SEQUENCE(seq);
        REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                    .IN_SEQUENCE(seq);
        REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmd.size(),cmd))       .IN_SEQUENCE(seq);
        REQUIRE_CALL(flashCfgMock, ABSSF_readData(rdIdent.size(),_))      .IN_SEQUENCE(seq)
            .LR_SIDE_EFFECT(_2 = rdIdent); /* copy the ident structure to the buffer */
        REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                  .IN_SEQUENCE(seq);
    /* sequence end */

        ABSSF_init();
        CHECK(ABSSF_jedecID.manufacturer == ABSSF_Macronix);
        CHECK(ABSSF_jedecID.type         == Memory_Type);
        CHECK(ABSSF_jedecID.capacity     == Memory_density);  
    }

    TEST_CASE("ABSSF_isBusy")
    {
        const uint8_t busy_mask = 0x01;
        vector<uint8_t> rdStatus{ABSSF_cmdRdStatusReg};
        vector<uint8_t> status{busy_mask};

        SUBCASE("ABSSF_isBusy: busy mask set")
        {
        /* following expected calls in this order (sequence:seq) */
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                  .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(rdStatus.size(),rdStatus))           .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_readData(status.size(),_))                     .IN_SEQUENCE(seq)
                .LR_SIDE_EFFECT(_2=status);/* copy busy_mask to buffer */
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                                .IN_SEQUENCE(seq);
        /* sequence end */
            CHECK(ABSSF_isBusy() == true);
        }

        SUBCASE("ABSSF_isBusy: busy mask not set")
        {
            status[0] = 0;
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip());
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(rdStatus.size(),rdStatus))           .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_readData(status.size(),_))                     .IN_SEQUENCE(seq)
                .LR_SIDE_EFFECT(_2=status);/* copy busy_mask to buffer */
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip());
            CHECK(ABSSF_isBusy() == false);
        }

        SUBCASE("ABSSF_isBusy: busy mask not set, some other status bits set")
        {
            status[0] = 0x0FE;
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip());
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(rdStatus.size(),rdStatus))           .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_readData(status.size(),_))                     .IN_SEQUENCE(seq)
                .LR_SIDE_EFFECT(_2=status);/* copy busy_mask to buffer */
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip());
            CHECK(ABSSF_isBusy() == false);
        }
        SUBCASE("ABSSF_isBusy: busy mask set, including other flags")
        {
            status[0] = 0x0FF;
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip());
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(rdStatus.size(),rdStatus))           .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_readData(status.size(),_))                     .IN_SEQUENCE(seq)
                .LR_SIDE_EFFECT(_2=status);/* copy busy_mask to buffer */
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip());
            CHECK(ABSSF_isBusy() == true);
        }

    }

    TEST_CASE("Erase")
    {
        SUBCASE("ABSSF_eraseSector")
        {
            const uint32_t address = 0x123456;
            vector<uint8_t> cmdErase{ABSSF_cmdEraseSector, (uint8_t)(address>>16), (uint8_t)(address>>8u), (uint8_t)address};

        /* write enable */
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdWrEnable.size(), cmdWrEnable))  .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);
        /* write enable end*/
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdErase.size(), cmdErase))        .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);

            ABSSF_eraseSector(address);
        }
        
        SUBCASE("ABSSF_eraseBlock32")
        {
            const uint32_t address = 0x654321;
            vector<uint8_t> cmdErase{ABSSF_cmdEraseBlock32, (uint8_t)(address>>16), (uint8_t)(address>>8u), (uint8_t)address};

        /* write enable */
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdWrEnable.size(), cmdWrEnable))  .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);
        /* write enable end*/
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdErase.size(), cmdErase))        .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);

            
            ABSSF_eraseBlock32(address);
        }
        SUBCASE("ABSSF_eraseBlock64")
        {
            const uint32_t address = 0x112233;
            vector<uint8_t> cmdErase{ABSSF_cmdEraseBlock64, (uint8_t)(address>>16), (uint8_t)(address>>8u), (uint8_t)address};

        /* write enable */
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdWrEnable.size(), cmdWrEnable))  .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);
        /* write enable end*/
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdErase.size(), cmdErase))        .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);
            
            ABSSF_eraseBlock64(address);
        }
    }

    TEST_CASE("ABSSF_limitWriteSize")
    {
        const uint32_t page_size = 0x100;

        SUBCASE("address aligned and size < page size")
        {
            CHECK(ABSSF_limitWriteSize(0, 47) == 47);
        }
        SUBCASE("address still aligned and size < page size")
        {
            CHECK(ABSSF_limitWriteSize(0x100, 42) == 42);
        }
        SUBCASE("address aligned and size > page size")
        {
            CHECK(ABSSF_limitWriteSize(0x800, 0x101) == 0x100);
        }
        SUBCASE("address unaligned and not enough size in page")
        {
            CHECK(ABSSF_limitWriteSize(0x801, 0x100) == 0xFF);
        }
        SUBCASE("address unaligned and enough size in page")
        {
            CHECK(ABSSF_limitWriteSize(0x810, 0x0F0) == 0xF0);
        }

    }

    TEST_CASE("ABSSF_getMemorySize")
    {
        const uint32_t mege_bits_in_flash = 16u * 1024u * 1024u; // 16 MBit
        const uint32_t bits_in_byte = 8;
        ABSSF_jedecID.capacity = 0x15;
        CHECK(ABSSF_getMemorySize() == (uint32_t)(mege_bits_in_flash/bits_in_byte)); // 16Mbit = 2MByte
    }

    TEST_CASE("ABSSF_powerDown")
    {
        cmd[0]= ABSSF_cmdPowerDown;
        REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())               .IN_SEQUENCE(seq);
        REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmd.size(),cmd))  .IN_SEQUENCE(seq);
        REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())             .IN_SEQUENCE(seq);

        ABSSF_powerDown();
    }
    
    TEST_CASE("ABSSF_releasePowerDown")
    {
        cmd[0]= ABSSF_cmdRelPowerDown;
        REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())               .IN_SEQUENCE(seq);
        REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmd.size(),cmd))  .IN_SEQUENCE(seq);
        REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())             .IN_SEQUENCE(seq);

        ABSSF_releasePowerDown();
    }

    TEST_CASE("ABSSF_write")
    {
        const uint32_t address  = 0x112233;
        const uint32_t page_size = 0x100;
        vector<uint8_t> buffer(100);
        vector<uint8_t> write_cmd{ABSSF_cmdPageProgram, (uint8_t)(address>>16), (uint8_t)(address>>8u), (uint8_t)address};
        buffer = {42,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

        SUBCASE("write zero byte")
        {
            CHECK(ABSSF_write(address, 0, buffer.data()) == 0u);
        }

        SUBCASE("write a byte")
        {
            buffer = {42};
        /* write enable */
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdWrEnable.size(), cmdWrEnable))  .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);
        /* write enable end*/

            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                 .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(write_cmd.size(),write_cmd))        .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(buffer.size(),buffer))              .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                               .IN_SEQUENCE(seq);
            
            CHECK(ABSSF_write(address, buffer.size(), buffer.data()) == 1);
        }

        SUBCASE("try to write more than a page at aligned address")
        {
            const uint32_t size     = 0x110;
            const uint32_t address  = 0x300;
            vector<uint8_t> write_cmd{ABSSF_cmdPageProgram, (uint8_t)(address>>16), (uint8_t)(address>>8u), (uint8_t)address};
            buffer.resize(page_size); // extend the buffer size to be one page

        /* write enable */
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdWrEnable.size(), cmdWrEnable))  .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);
        /* write enable end*/

            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(write_cmd.size(),write_cmd))       .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(buffer.size(),buffer))             .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);
            
            CHECK(ABSSF_write(address, size, buffer.data()) == page_size);
        }

        SUBCASE("try to write more than a page at unaligned address")
        {
            const uint32_t size     = 0x110;
            const uint32_t offset   = 0x08;
            const uint32_t address  = 0x300 + offset;
            vector<uint8_t> write_cmd{ABSSF_cmdPageProgram, (uint8_t)(address>>16), (uint8_t)(address>>8u), (uint8_t)address};
            buffer.resize(page_size-offset); // extend the buffer size to be the expected write size

        /* write enable */
            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(cmdWrEnable.size(), cmdWrEnable))  .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);
        /* write enable end*/

            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                                .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(write_cmd.size(),write_cmd))       .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(buffer.size(),buffer))             .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                              .IN_SEQUENCE(seq);

            
            CHECK(ABSSF_write(address, size, buffer.data()) == page_size-offset);
        }
    }
    TEST_CASE("ABSSF_read")
    {
        vector<uint8_t> buffer;
        const uint32_t address  = 0x112233;
        
        SUBCASE("read zero bytes")
        {
            ABSSF_read(address, 0, buffer.data()); // has no effect, no mocks called, no data changed
        }

        SUBCASE("read some bytes")
        {
            vector<uint8_t> read_cmd{ABSSF_cmdRdData, (uint8_t)(address>>16), (uint8_t)(address>>8u), (uint8_t)address};
            buffer = {42,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
            vector<uint8_t> rdBuffer(buffer.size());

            REQUIRE_CALL(flashCfgMock, ABSSF_selectChip())                       .IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_writeData(read_cmd.size(),read_cmd)).IN_SEQUENCE(seq);
            REQUIRE_CALL(flashCfgMock, ABSSF_readData(buffer.size(),_))          .IN_SEQUENCE(seq)
                .LR_SIDE_EFFECT(_2 = buffer); /* copy content of buffer to 2nd parameter */
            REQUIRE_CALL(flashCfgMock, ABSSF_unselectChip())                   .IN_SEQUENCE(seq);

            ABSSF_read(address,buffer.size(),rdBuffer.data());
            CHECK(rdBuffer == buffer);
        }
    }
}
