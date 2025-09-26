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
        SPI_ABSTRACTION[SPI Abstraction] --> DLL
        SPI_ABSTRACTION --> MSPI[MSPI: SPI MCAL]
    end

    subgraph External Dependencies
        DBM[DBM: DBus Mapping]
        STIM[STIM: System Timer]
        MSPI[MSPI: SPI MCAL]
        BUSTYPE[BUSTYPE: Bus Types]
        LIBTYPE[LIBTYPE: Library Types]
        BSH_STDINC[BSH_STDINC: Standard Includes]
        LIBDEFINE[LIBDEFINE: Library Defines]
        HSUP[HSUP: Helper Support]
        MCAL_TYPES[MCAL_TYPES: MCAL Types]
        MCAL_CHANNELS[MCAL_CHANNELS: MCAL Channels]
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

    SPI_ABSTRACTION --> BSH_STDINC
    SPI_ABSTRACTION --> MCAL_CHANNELS

    style BAL fill:#f9f,stroke:#333,stroke-width:2px
    style DLL fill:#bbf,stroke:#333,stroke-width:2px
    style DBPL fill:#9cf,stroke:#333,stroke-width:2px
    style DBAL fill:#ffc,stroke:#333,stroke-width:2px
    style DBAL_CFG fill:#ccf,stroke:#333,stroke-width:2px
    style SPI_ABSTRACTION fill:#cff,stroke:#333,stroke-width:2px