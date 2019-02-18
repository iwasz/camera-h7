/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef PREPARE_SEND_NETWORK_ACTION_H
#define PREPARE_SEND_NETWORK_ACTION_H

#include "stateMachine/Action.h"

class CircularBuffer;

/**
 * @brief Określa jaki jest rozmiar danych do wysłania i deklaruje ile chce wysłać.
 */
class SendNetworkAction : public Action {
public:
        enum Stage { STAGE_PREPARE, STAGE_SEND, STAGE_DECLARE };

        SendNetworkAction (CircularBuffer *g, Stage s, uint32_t *b) : gsmBuffer (g), stage (s), bytesToSendInSendStage (b) {}

        virtual ~SendNetworkAction () {}
        bool run (const char *);

#ifndef UNIT_TEST
private:
#endif
        static uint32_t parseCipsendRespoinse (const char *input);

private:

        CircularBuffer *gsmBuffer; // To są bajty do wysłania.
        Stage stage;
        uint32_t *bytesToSendInSendStage;
};

#endif // SendNetworkAction_H
