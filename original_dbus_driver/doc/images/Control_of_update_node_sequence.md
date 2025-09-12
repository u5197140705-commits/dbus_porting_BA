%%{init: { 'sequence': {'noteAlign': 'left'}  }}%%

sequenceDiagram
    participant SMM as System Master#colon; SMM
    participant UN as Node for Update#colon; #micro;C
    participant ON as All other nodes#colon; #micro;C
    
    
    Note over SMM,ON: Prepare Communication
    SMM ->>   ON: SilentModeTransitionRequest Broadcast
    note right of ON: Silent mode for ECU(s) without DBusCAN chip#colon;<br/> - do not send application specific messages<br/> - do not send negative acknowledges<br/> - continue handling service messages
    note right of ON: Silent mode for ECU(s) with DBusCAN chip#colon;<br/> - do not send any messages<br/> - do not send any acknowledges<br/>* see notes below
    
    Note over SMM,ON: Prepare node
    SMM ->>   UN:  Update ModeVerifyRequest
    UN -->>  SMM: UpdateModeVerifyResponse(update_transition_delay)
    SMM ->>  UN : UpdateModeTransitionRequest

    note right of UN: Update mode#colon; <br/> - prepare for update<br/> - optional switch to a separate programming application
    
    SMM ->>  UN : any addressed service message (only for ECU(s) with DBusCAN chip)
    note right of UN: ECU(s) with DBusCAN chip#colon;<br/> - leave Silent Mode (after the 1st addressed service message received)

    SMM ->>  UN : BaudRateVerifyRequest(update_baud_rate)
    UN  -->> SMM: BaudRateVerifyResponse(baud_rate_transition_delay)
    SMM ->>  UN : BaudRateTransitionRequest
    note right of UN: Baudrate change#colon; <br/> - start timeout when baudrate is != default<br/> - restart timeout with every received BaudRateTrigger message<br/> - switch to default baudrate after timeout<br/> - stop timeout when baudrate == default
    
    loop wait
        SMM -->  UN: baud_rate_transition_delay
    end

    Note over SMM,ON: Write to node
   loop for each node in sytstem
        loop
            SMM ->>  UN : MAP/FAP Request
            UN  -->> SMM: MAP/FAP Response
        end
        SMM ->>  UN : BaudRateTrigger
    end

    Note over SMM,ON: Clean up node
    SMM ->>  UN : BaudRateVerifyRequest(default_baud_rate)
    UN  -->> SMM: BaudRateVerifyResponde(baud_rate_transition_delay)
    SMM ->>  UN : BaudRateTransitionRequest
    loop wait
        SMM -->  UN: baud_rate_transition_delay
    end
    %% note right of UN: ECU(s) with DBusCAN - Chip#colon;<br/> - enable Silent Mode
