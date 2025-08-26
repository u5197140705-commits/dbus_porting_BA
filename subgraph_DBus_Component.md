graph TD
    subgraph DBus Component
        BAL[BAL: Bus Application Layer] --> DLL
        DLL[DLL: Data Link Layer] --> DBM[DBM: DBus Mapping]
        DLL --> DBPL
        DBPL[DBPL: DBus Presentation Layer] --> DLL
        DBAL[DBAL: DBus Application Layer] --> BAL
        DBAL --> DLL
        DBAL --> DBPL
        DBAL --> STIM[STIM: System Timer]
        DBAL_CFG[DBAL_cfg: Configuration] --> DBAL
        DBUSCAN_DRV[DBusCAN Driver] --> DLL
        DBUSCAN_DRV --> MSPI[MSPI: SPI MCAL]
        DBUSCAN_DRV --> MDMA[MDMA: DMA MCAL]
        DBUSCAN_DRV --> MDIO[MDIO: Digital I/O MCAL]
    end

    subgraph External Dependencies
        DBM[DBM: DBus Mapping]
        STIM[STIM: System Timer]
        MSPI[MSPI: SPI MCAL]
        MDMA[MDMA: DMA MCAL]
        MDIO[MDIO: Digital I/O MCAL]
        BUSTYPE[BUSTYPE: Bus Types]
        LIBTYPE[LIBTYPE: Library Types]
        BSH_STDINC[BSH_STDINC: Standard Includes]
        LIBDEFINE[LIBDEFINE: Library Defines]
        HSUP[HSUP: Helper Support]
        MCAL_TYPES[MCAL_TYPES: MCAL Types]
        MCAL_CHANNELS[MCAL_CHANNELS: MCAL Channels]
        DBUSCAN_TYPES[DBUSCAN_TYPES: DBusCAN Types]
        DBUSCAN[DBUSCAN: DBusCAN Core]
    end

    BAL --> BUSTYPE
    BAL --> LIBTYPE
    BAL --> BSH_STDINC
    BAL --> STIM

    DLL --> BUSTYPE
    DLL --> LIBTYPE
    DLL --> BSH_STDINC
    DLL --> HSUP
    DLL --> MCAL_TYPES

    DBPL --> BUSTYPE
    DBPL --> LIBTYPE
    DBPL --> BSH_STDINC

    DBAL --> BUSTYPE
    DBAL --> BSH_STDINC
    DBAL --> LIBDEFINE

    DBUSCAN_DRV --> BSH_STDINC
    DBUSCAN_DRV --> MCAL_CHANNELS
    DBUSCAN_DRV --> DBUSCAN_TYPES
    DBUSCAN_DRV --> DBUSCAN

    style BAL fill:#f9f,stroke:#333,stroke-width:2px
    style DLL fill:#bbf,stroke:#333,stroke-width:2px
    style DBPL fill:#9cf,stroke:#333,stroke-width:2px
    style DBAL fill:#ffc,stroke:#333,stroke-width:2px
    style DBAL_CFG fill:#ccf,stroke:#333,stroke-width:2px
    style DBUSCAN_DRV fill:#cff,stroke:#333,stroke-width:2px
