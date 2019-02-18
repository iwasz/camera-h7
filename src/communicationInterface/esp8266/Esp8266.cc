/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Esp8266.h"
#include "SendNetworkAction.h"
#include "communicationInterface/modem/GsmCommandAction.h"

// TODO !!!! wywalić!
Usart *modemUsart;

enum ModemState : size_t {
        RESET_STAGE_DECIDE,
        RESET_STAGE_POWER_OFF,
        RESET_STAGE_POWER_ON,
        INIT,
        LIST_ACCESS_POINTS,
        CONNECT_TO_ACCESS_POINT,
        SIGNAL_QUALITY_CHECK,
        VERIFY_CONNECTED_ACCESS_POINT,
        CHECK_MY_IP,
        SET_MULTI_CONNECTION_MODE,
        ACTIVATE_PDP_CONTEXT,
        APN_USER_PASSWD_INPUT,
        GPRS_CONNECTION_UP,
        DNS_CONFIG,
        CONNECT_TO_SERVER,
        CLOSE_AND_RECONNECT,
        SHUT_DOWN_STAGE_START,
        SHUT_DOWN_STAGE_POWER_OFF,
        SHUT_DOWN,
        GPRS_RESET,
        CANCEL_SEND,
        GPS_USART_ECHO_ON,
        NETWORK_SEND,
        NETWORK_BEGIN_SEND,
        NETWORK_PREPARE_SEND,
        NETWORK_QUERY_MODEM_OUTPUT_BUFFER_MAX_LEN,
        NETWORK_GPS_USART_ECHO_OFF,
        NETWORK_DECLARE_READ,
        NETWORK_ACK_CHECK,
        NETWORK_ACK_CHECK_PARSE,
        RESET_BOARD,
        SET_OPERATING_MODE,
        USE_DNS,
        CFG_CLOSE_AND_RECONNECT,
        CFG_CONNECT_TO_SERVER,
        CFG_PREPARE_SEND,
        CFG_SEND_QUERY,
        CFG_RECEIVE,
        MQTT_CONN,
        MQTT_PUB_START,
        MQTT_PUB,
        CFG_CANCEL_SEND
};

/*****************************************************************************/

Esp8266::Esp8266 (Usart &u, StringQueue &g) : WifiCard (u), outputBuffer (2048), gsmQueue (g), machine (&gsmQueue)
{
        modemUsart = &u;

        static StringCondition ok ("OK");
        static StringCondition okA (">OK");
        static bool alwaysTrueB = true;
        //        static bool alwaysFalseB = false;
        static BoolCondition alwaysTrue (&alwaysTrueB);
        //        static WatchdogRefreshAction wdgRefreshAction (&watchdog);

        static DelayAction delay (100);
        static DelayAction longDelay (1000);
        static LikeCondition error ("%ERROR%");
        static TimeCounter gsmTimeCounter; /// Odmierza czas między zmianami stanów GPS i GSM aby wykryć zwiechę.
        machine.setTimeCounter (&gsmTimeCounter);

        StateMachine *m = &machine;

        static TimePassedCondition softResetDelay (SOFT_RESET_DELAY_MS, &gsmTimeCounter);
        static TimePassedCondition hardResetDelay (HARD_RESET_DELAY_MS, &gsmTimeCounter);
        static_assert (HARD_RESET_DELAY_MS < SOFT_RESET_DELAY_MS, "HARD_RESET_DELAY_MS musi być mniejsze niż SOFT_RESET_DELAY_MS");
        static_assert (TCP_SEND_DATA_DELAY_MS < SOFT_RESET_DELAY_MS, "TCP_SEND_DATA_DELAY_MS musi być mniejsze niż SOFT_RESET_DELAY_MS");

        /*---------------------------------------------------------------------------*/
        /*--HARD-I-SOFT-RESET--------------------------------------------------------*/
        /*---------------------------------------------------------------------------*/
        /* clang-format off */

        m->transition (INIT)->when (&softResetDelay);

        m->state (INIT, State::INITIAL)->entry (at ("AT\r\n"))
                ->transition (SET_OPERATING_MODE)->when (&ok)->then (&delay);

        m->state (SET_OPERATING_MODE)->entry (and_action (at ("AT+CWMODE=1\r\n"), delayMs (1000)))
                ->transition (RESET_BOARD)->when (&ok)->then (&delay);

        m->state (RESET_BOARD)->entry (at ("AT+RST\r\n"))
                ->transition (LIST_ACCESS_POINTS)->when (eq ("ready"))->then (&delay);

        m->state (LIST_ACCESS_POINTS)->entry (at ("AT+CWLAP\r\n"))
                ->transition (CONNECT_TO_ACCESS_POINT)->when (like ("%Honor 9%"))->then (&delay);

        m->state (CONNECT_TO_ACCESS_POINT)->entry (at ("AT+CWJAP=\"Honor 9\",\"f386dd74bbc3\"\r\n"))
                ->transition (VERIFY_CONNECTED_ACCESS_POINT)->when (&ok)->then (&delay);

        m->state (VERIFY_CONNECTED_ACCESS_POINT)->entry (at ("AT+CWJAP?\r\n"))
                ->transition (VERIFY_CONNECTED_ACCESS_POINT)->when (anded (eq ("AT+CWJAP?"), beginsWith ("busy")))->then (delayMs (5000))
                ->transition (CHECK_MY_IP)->when (like ("%Honor 9%"))->then (&delay);

        m->state (CHECK_MY_IP)->entry (at ("AT+CIFSR\r\n"))
                ->transition (SET_MULTI_CONNECTION_MODE)->when (like ("%.%.%.%"))->then (&delay)
                ->transition (LIST_ACCESS_POINTS)->when (&error)->then (&delay);

        m->state (SET_MULTI_CONNECTION_MODE)->entry (at ("AT+CIPMUX=1\r\n"))
                ->transition (CONNECT_TO_SERVER)->when (anded (eq ("AT+CIPMUX=1"), &ok))->then (&delay);

        m->state (CONNECT_TO_SERVER)->entry (at ("AT+CIPSTART=0,\"TCP\",\"www.google.com\",80\r\n"))
                ;//->transition (CHECK_MY_IP)->when (like ("Honor 9"))->then (&delay);


#if 0
        m->state (ENTER_PIN)->entry (gsm ("AT+CPIN=1220\r\n"))
                ->transition (LIST_ACCESS_POINTS)->when (anded (beginsWith ("AT+CPIN="), &ok))->then (&delay)
                ->transition (LIST_ACCESS_POINTS)->when (&error)->then (delayMs (4000));

        /*
         * Check Signal Quality response:
         * +CSQ: <rssi>,<ber>
         *
         * <rssi>
         * 0 -115 dBm or less
         * 1 -111 dBm
         * 2...30 -110... -54 dBm
         * 31 -52 dBm or greater
         * 99 not known or not detectable
         *
         * <ber> (in percent):
         * 0...7 As RXQUAL values in the table in GSM 05.08 [20] subclause 7.2.4
         * 99 Not known or not detectable
         */
        // TODO Tu powinno być jakieś logowanie tej siły sygnału.
        m->state (SIGNAL_QUALITY_CHECK)->entry (gsm ("AT+CSQ\r\n"))
                ->transition (SIGNAL_QUALITY_CHECK)->when (anded (like ("+CSQ: 99,%"), &ok))->then (&longDelay)
                ->transition (NETWORK_REGISTRATION_CHECK)->when (anded (beginsWith ("+CSQ:"), &ok))->then (&delay);

        /*
         * Check Network Registration Status response:
         * +CREG: <n>,<stat>[,<lac>,<ci>]
         *
         * <n>
         * 0 Disable network registration unsolicited result code
         * 1 Enable network registration unsolicited result code +CREG: <stat>
         * 2 Enable network registration unsolicited result code with location information +CREG: <stat>[,<lac>,<ci>]
         *
         * <stat>
         * 0 Not registered, MT is not currently searching a new operator to register to
         * 1 Registered, home network
         * 2 Not registered, but MT is currently searching a new operator to register to
         * 3 Registration denied
         * 4 Unknown
         * 5 Registered, roaming
         *
         * <lac>
         * String type (string should be included in quotation marks);
         * two byte location area code in hexadecimal format
         *
         * <ci>
         * String type (string should be included in quotation marks);
         * two byte cell ID in hexadecimal format
         */
        // TODO Parsowanie drugiej liczby po przecinku. Jak 1 lub 5, to idziemy dalej.
        m->state (NETWORK_REGISTRATION_CHECK)->entry (gsm ("AT+CREG?\r\n"))
                ->transition (SIGNAL_QUALITY_CHECK)->when (anded (ored (like ("+CREG: %,0%"), like ("+CREG: %,2%")), &ok))->then (delayMs (5000))
                ->transition (SIGNAL_QUALITY_CHECK)->when (anded (ored (like ("+CREG: %,3%"), like ("+CREG: %,4%")), &ok))->then (delayMs (5000))
                ->transition (INIT)->when (&error)->then (&longDelay)
                ->transition (APN_USER_PASSWD_INPUT)->when (anded (ored (like ("+CREG: %,1%"), like ("+CREG: %,5%")), &ok)); // Zarejestorwał się do sieci.

        // Set context 0 as foreground context.
        // TODO zmienić nazwę stanu!
//        m->state (GPRS_ATTACH)->entry (gsm ("AT+QIFGCNT=0\r\n"))
//                ->transition (GPRS_RESET)->when (&error)->then (&longDelay)
//                ->transition(APN_USER_PASSWD_INPUT)->when (anded(eq ("AT+QIFGCNT=0"), &ok))->then (&delay);

        // Start Task and Set APN, USER NAME, PASSWORD
        // TODO Uwaga! kiedy ERROR, to idzie do GPRS_RESET, który ostatnio też zwracał ERROR i wtedy twardy reset. Czemu?
        m->state (APN_USER_PASSWD_INPUT)->entry (gsm ("AT+QICSGP=1,1,\"internet\"\r\n"))
                ->transition (GPRS_RESET)->when (&error)->then (&longDelay)
                ->transition (PDP_CONTEXT_CHECK)->when (anded (beginsWith ("AT+QICSGP="), &ok))->then (&delay);

        m->state (PDP_CONTEXT_CHECK)->entry (gsm ("AT+QIACT?\r\n"))
                ->transition(DNS_CONFIG)->when (like ("+QIACT:%1,%"))->then (&delay)
                ->transition(ACTIVATE_PDP_CONTEXT)->when (anded (beginsWith ("AT+QIACT?"), &ok))->then (&delay);

        m->state (ACTIVATE_PDP_CONTEXT)->entry (gsm ("AT+QIACT=1\r\n"))
                ->transition(DNS_CONFIG)->when (anded (beginsWith ("AT+QIACT="), &ok))->then (&delay);

        // TODO zmienić nazwę stanu!
        // Set connectiopn as non-transparent
//        m->state (GPRS_CONNECTION_UP)->entry (gsm ("AT+QIMODE=0\r\n"))
//                ->transition (DNS_CONFIG)->when (anded (eq ("AT+QIMODE=0"), &ok))->then (&delay);

        m->state (DNS_CONFIG)->entry (gsm ("AT+QIDNSCFG=1,\"8.8.8.8\",\"8.8.4.4\"\r\n"))
                ->transition (INIT)->when (&error)->then (&longDelay)
                ->transition (NETWORK_GPS_USART_ECHO_OFF)->when (anded (beginsWith ("AT+QIDNSCFG="), &ok))->then (&delay);

        // Use dns
//        m->state (USE_DNS)->entry (gsm ("AT+QIDNSIP=1\r\n"))
//                ->transition (NETWORK_GPS_USART_ECHO_OFF)->when (anded (beginsWith ("AT+QIDNSIP="), &ok))->then (&delay);

        // Wyłącz ECHO podczas wysyłania danych.
        m->state (NETWORK_GPS_USART_ECHO_OFF)->entry (gsm ("AT+QISDE=0\r\n"))
                ->transition (CLOSE_AND_RECONNECT)->when (/*anded (&configurationWasRead,*/ anded (beginsWith ("AT+QISDE=0"), &ok))
                /*->transition (CFG_CLOSE_AND_RECONNECT)->when (anded (beginsWith ("AT+QISDE=0"), &ok))*/;

        /*---------------------------------------------------------------------------*/
        /*--WYSYŁKA-DANYCH-TCP-IP----------------------------------------------------*/
        /*---------------------------------------------------------------------------*/

        m->state (CLOSE_AND_RECONNECT)->entry (gsm ("AT+QICLOSE=1\r\n"))
                ->transition (CONNECT_TO_SERVER)->when(anded (beginsWith ("AT+QICLOSE"), &ok))->then (delayMs (1000));

        // Połącz się z serwerem (połączenie TCP).
        m->state (CONNECT_TO_SERVER)->entry (func ([&u] (const char *) {
                static char qiopenCommand[QIOPEN_BUF_LEN];
                const char *serverAddress = "trackmatevm.cloudapp.net";
                const uint16_t serverPort = 1883;
                snprintf (qiopenCommand, QIOPEN_BUF_LEN, "AT+QIOPEN=1,1,\"TCP\",\"%s\",%d\r\n", serverAddress, serverPort);
                u.transmit (qiopenCommand);
                return true; }))
                ->transition (NETWORK_BEGIN_SEND)->when (beginsWith ("+QIOPEN: 1,0"))->then (delayMs (1000))
                ->transition (GPRS_RESET)->when (ored (beginsWith ("+QIOPEN: 1"), &error))->then(delayMs(1000));

        /*
         * Sprawdź bufor wyjśy GSM (ten z zserializowanymi danymi). Jeśli pusty, to kręć się w tym stanie w kółko,
         * co daje szansę na uruchomienie się maszyny GPS (bo jest INC_SYNCHRO). Jeśli są jakieś dane do wysłania,
         * to sprawdź ile max może przyjąć modem.
         */
        m->state (NETWORK_BEGIN_SEND)->exit (delayMs (GPS_READ_PERIOD_APPROX_MS))
                ->transition (CLOSE_AND_RECONNECT)->whenf ([this] (const char *input) { return outputBuffer.size() == 0 ; })
                ->transition (NETWORK_QUERY_MODEM_OUTPUT_BUFFER_MAX_LEN)->when (&alwaysTrue);

        /*
         * Jeszcze nie wiem czemu, ale czasem zwraca ten max rozmiar bajtów modemie jako 0. Próbuję się wtedy ponownie połączyć
         * z serwerem. To dziwna sprawa. Po kilku sprawdzeniach AT+CIPSEND? dostaję odpowiedź 0! Myślałem że to wtedy gdy serwer
         * się rozłącza, ale jednak chyba wcześniej.
         */
        m->state (NETWORK_QUERY_MODEM_OUTPUT_BUFFER_MAX_LEN)->entry (gsm ("AT+QISEND=?\r\n"))
                ->transition (CLOSE_AND_RECONNECT)->when (beginsWith ("+QISEND: 0"))
                // ->transition (NETWORK_PREPARE_SEND)->when (anded (beginsWith ("+QISEND:", StringCondition::STRIP, StringCondition::RETAIN_INPUT), &ok));
                ->transition (NETWORK_PREPARE_SEND)->when (anded (beginsWith ("+QISEND: (0-11),(0-1460)"), &ok));

        /*
         * Uwaga, wysłanie danych jest zaimplementowane w 2 stanach. W NETWORK_PREPARE_SEND idzie USARTem komenda AT+CIPSEND=<bbb>, a w
         * stanie NETWORK_SEND idzie <bbb> bajtów. Jeżeli w NETWORK_SEND nie wyślemy USARTem bajtów do modemu, to modem będzie czekał
         * bez żadnej odpowiedzi i to wygląda jakby się zawiesił.
         *
         * Uwaga, zauważyłem, że modem próbuje zwrócić echo z tymi wysłanymi danymi. Ja mu podaję opk 300B, a on mi odpowiada jakimś
         * "> -" i potem bajty. Firmware się zawiesza, bo ma bufor wejściowy tylko 128B. I ten błąd się objawiał niezależnie czy używałem
         * mojej implementacji sendBlocking czy HAL_UART_Transmit z HAL. Jedyna różnica była taka, że debuger lądował w innych miejscach.
         * Jednak w obydwu przypadkach wysłąnie większej ilości danych przez USART do modemu powodowało flage ORE (rx overrun) podczas
         * odbierania. Było to spowodowane właśnie tym idiotycznycm echem, które przepełniało bufor, ISR przestawało odczytywać flagę
         * RDR, co w konsekwencji oznacza że RXNE przestawało być czyszcone (odczyt z RDR je czyści AFAIK) i następował ORE.
         */
        static uint32_t bytesToSendInSendStage = 0;
        // Łapie odpowiedź z poprzedniego stanu, czyli max liczbę bajtów i wysyła komendę CIPSEND=<obliczona liczba B>
        static SendNetworkAction prepareAction (&outputBuffer, SendNetworkAction::STAGE_PREPARE, &bytesToSendInSendStage);
        static IntegerCondition bytesToSendZero ((int *)&bytesToSendInSendStage, IntegerCondition::EQ, 0);
        m->state (NETWORK_PREPARE_SEND)->entry (&prepareAction)
                ->transition (CLOSE_AND_RECONNECT)->when (&bytesToSendZero)
                ->transition (CLOSE_AND_RECONNECT)->when (&error)->then (&longDelay)
                ->transition (NETWORK_SEND)->when (beginsWith ("AT+QISEND="));

        // Ile razy wykonaliśmy cykl NETWORK_ACK_CHECK -> NETWORK_ACK_CHECK_PARSE (oczekiwanie na ACK danych).
        static uint8_t ackQueryRetryNo = 0;
        static SendNetworkAction sendAction (&outputBuffer, SendNetworkAction::STAGE_SEND, &bytesToSendInSendStage);
        static IntegerAction resetRetry ((int *)&ackQueryRetryNo, IntegerAction::CLEAR);
        static IntegerAction incRetry ((int *)&ackQueryRetryNo, IntegerAction::INC);
        m->state (NETWORK_SEND)->entry (&sendAction)
                ->transition (NETWORK_ACK_CHECK)->when (eq ("SEND OK"))->then (&resetRetry)
                ->transition (CANCEL_SEND)->when (msPassed (TCP_SEND_DATA_DELAY_MS, &gsmTimeCounter))
                ->transition (GPRS_RESET)->when (ored (&error, eq ("CLOSED"), eq ("SEND FAIL"), eq ("+PDP: DEACT")))->then (&longDelay);

        m->state (NETWORK_ACK_CHECK)->entry (gsm ("AT+QISEND=1,0\r\n"))
                ->transition (NETWORK_ACK_CHECK_PARSE)->when (anded (&ok, beginsWith ("+QISEND: ", StringCondition::STRIP, StringCondition::RETAIN_INPUT)));

        static uint16_t sent;
        static uint16_t acked;
        static uint16_t nAcked;
        static QueryAckAction queryAck (&sent, &acked, &nAcked);
        // TODO nie ackuje mi!!!
        // static IntegerCondition allAcked (reinterpret_cast <int *> (&acked), IntegerCondition::EQ, reinterpret_cast <int *> (&bytesToSendInSendStage));
        static IntegerCondition allAcked (reinterpret_cast <int *> (&sent), IntegerCondition::EQ, reinterpret_cast <int *> (&bytesToSendInSendStage));
        static IntegerCondition retryLimitReached ((int *)&ackQueryRetryNo, IntegerCondition ::GT, QUERY_ACK_COUNT_LIMIT);
        m->state (NETWORK_ACK_CHECK_PARSE)->entry (&queryAck)->exit (&longDelay)
                ->transition (CANCEL_SEND)->when (&retryLimitReached)
                ->transition (NETWORK_DECLARE_READ)->when (&allAcked)->then (&resetRetry)
                ->transition (NETWORK_ACK_CHECK)->when (&alwaysTrue)->then (&incRetry);

        // static SendNetworkAction declareAction (&outputBuffer, SendNetworkAction::STAGE_DECLARE, reinterpret_cast <uint32_t *> (&acked));
        // TODO nie ackuje mi!!!
        static SendNetworkAction declareAction (&outputBuffer, SendNetworkAction::STAGE_DECLARE, reinterpret_cast <uint32_t *> (&sent));
        m->state (NETWORK_DECLARE_READ)->entry (&declareAction)
                ->transition (CLOSE_AND_RECONNECT)->when (&alwaysTrue);

        /*---------------------------------------------------------------------------*/

        m->state (CANCEL_SEND)->entry (and_action(delayMs (1200), gsm ("+++\r\n")))
                ->transition (CLOSE_AND_RECONNECT)->when (eq ("SEND OK"))
                ->transition (GPRS_RESET)->when (ored (&error, eq ("> -")))->then (delayMs (500));

        /*---------------------------------------------------------------------------*/
        /*--WYŁĄCZENIE-PODCZAS-SLEEP-------------------------------------------------*/
        /*---------------------------------------------------------------------------*/

        m->state (SHUT_DOWN_STAGE_START)
                ->transition (SHUT_DOWN_STAGE_POWER_OFF)->when (&statusHigh)
                ->transition (SHUT_DOWN)->when (&statusLow);

        // Suppress, bo wyłączanie modemu może potrwać nawet do 12 sekund.
        m->state (SHUT_DOWN_STAGE_POWER_OFF, State::SUPPRESS_GLOBAL_TRANSITIONS)->entry (and_action (&deinitgsmUsart, gsmPwrCycle))
                ->transition (SHUT_DOWN)->when (&statusLow)->then (delayMs (500));

        // Stąd tylko resetem maszyny stanów.
        m->state (SHUT_DOWN, State::SUPPRESS_GLOBAL_TRANSITIONS);
#endif
}

/*****************************************************************************/

bool Esp8266::send(int connectionNumber, uint8_t *data, size_t len)
{
    return (outputBuffer.store(data, len) == len);
}
