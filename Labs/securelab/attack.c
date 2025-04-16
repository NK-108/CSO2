#include "lab.h"

void forward_change_to_B(struct message *message) {
    if (message->to == 'B') {
        strcpy(message->data, "This replaces the original message.");
        message->data_size = strlen("This replaces the original message.");
    }
    send_message(message);
}

void forward_send_A_back_to_B(struct message *message) {
    if (message->to == 'B' && message->from == 'A') {
        message->to = 'A';
        message->from = 'B';
    }
    send_message(message);
}

static int counter = 0;
void forward_with_additional(struct message *message) {
    if (counter == 0) {
        send_message(new_message('A', 'B', 
                                  "This is an additional message.", 
                                  false, false));
        counter += 1;
    }
    send_message(message);
}

/* forward function for "attack 0" case.
   change this code to implement your attack
 */
int count0 = 0;
void forward_attack_0(struct message *message) {
    if (message->to == 'B') {
        strcpy(message->data, "PAY $10000438 TO M");
        message->data_size = strlen("PAY $10000438 TO M");
        if (count0 == 0) {
            send_message(message);
        }
        count0 += 1;
        return;
    }
    send_message(message);
}

/* forward function for "attack 1" case.
   change this code to implement your attack
 */
int count1 = 0;
void forward_attack_1(struct message *message) {
    if (message->to == 'B') {
        if (count0 == 0) {
            send_message(new_message('A', 'B', 
                "PAY $10000438 TO M", 
                true, false));
        }
        count0 += 1;
        return;
    }
    send_message(message);
}

/* forward function for "attack 2" case.
   change this code to implement your attack
 */
void forward_attack_2(struct message *message) {
    if (message->to == 'B') {
        send_message(message);
        send_message(message);
        return;
    }
    send_message(message);
}

/* forward function for "attack 3" case.
   change this code to implement your attack
 */
int count3 = 0;
void forward_attack_3(struct message *message) {
    if (message->to == 'B') {
        if (count3 == 0) {
            send_message(message);
        }
        else if (count3 == 1) {
            send_message(message);
            send_message(message);
        }
        count3 += 1;
    }
    else {
        send_message(message);
    }
}

/* forward function for "attack 4" case.
   change this code to implement your attack */
void forward_attack_4(struct message *message) {
    send_message(message);
}

/* forward function for "attack 5" case.
   I did not intend this one to be possible. */
void forward_attack_5(struct message *message) {
    send_message(message);
}
