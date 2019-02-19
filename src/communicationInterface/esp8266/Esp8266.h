/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef BG96_H
#define BG96_H

#include "Gpio.h"
#include "Timer.h"
#include "Usart.h"
#include "collection/CircularBuffer.h"
#include "stateMachine/StateMachine.h"

struct ICommunicationInterface {
        enum class Error : uint8_t { NO_ERROR };

        struct Callback {
                virtual ~Callback () = default;
                virtual void onData (uint8_t *data, size_t len) = 0;
                virtual void onError (ICommunicationInterface::Error error) = 0;
        };

        virtual ~ICommunicationInterface () = default;
        virtual int send (int connectionNumber, uint8_t *data, size_t len) = 0;
        Callback *callback = nullptr;
};

struct IModem : public ICommunicationInterface {
        virtual ~IModem () = default;
};

/**
 * @brief The AbstractModem class
 */
class AbstractModem : public IModem {
public:
        AbstractModem (Usart &u, Gpio &pwrKey, Gpio &status) : usart (u), pwrKeyPin (pwrKey), statusPin (status) {}
        virtual ~AbstractModem () = default;
        void power (bool on);

protected:
        Usart &usart;
        Gpio &pwrKeyPin;
        Gpio &statusPin;
};

class WifiCard : public ICommunicationInterface {
public:
        WifiCard (Usart &u) : usart (u) {}
        virtual ~WifiCard () = default;

private:
        Usart &usart;
};

/**
 * @brief The Bg96 class
 * TODO Do not CWLAP if already connected. ESP connects on its own when known AP is in range.
 * TODO Do not connect to AP as well if already connected (CWJAP disconnects and connects again in this situation).
 * TODO The addres we are connecting to is hardcoded. Api should have connect (returns ID/socket whatever) and disconnect like all normal APIS
 * does.
 * TODO Manage AP access somehow. Maybe some list of defined APs by the user?
 */
class Esp8266 : public WifiCard {
public:
        Esp8266 (Usart &u, StringQueue &g);
        virtual ~Esp8266 () override = default;

        int send (int connectionNumber, uint8_t *data, size_t len) override;
        void run () { machine.run (); }

private:
        CircularBuffer outputBuffer;
        StringQueue &gsmQueue; /// Odpowiedzi z modemu.
                               //        TimeCounter gsmTimeCounter;  /// Odmierza czas między zmianami stanów GPS i GSM aby wykryć zwiechę.
        StateMachine machine;
};

/*****************************************************************************/
// To jest inna historia, bo to jest "sensor"
struct IGnss {
        virtual ~IGnss () = default;
};

#endif // BG96_H
