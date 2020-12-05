#ifndef RFBUTTON_H_
#define RFBUTTON_H_

enum RFButtonEvent {
    RFBUTTONEVENT_SINGLECLICK,
    RFBUTTONEVENT_DOUBLECLICK,
    RFBUTTONEVENT_LONGCLICK
};

typedef std::function<void(RFButtonEvent event)> RFButtonCallback;

typedef struct _RFButtonEntry {

    unsigned long buttonCode = 0;
    uint8_t id = -1;
    RFButtonCallback callback;

    //Config
    uint32_t stable_threshold = 120;
    uint32_t longclick_threshold = 1000;
    uint32_t doubleclick_threshold = 500;

    bool longClickEnabled = true;
    bool doubleClickEnable = true;

    // State
    unsigned long eventSrartTime;
    unsigned long firstPressedTime;
    unsigned long lastPressedTime;
    bool eventOccured = false;
    bool isWaitingForSecondClick = false;
    //======
    struct _RFButtonEntry *next;
} RFButtonEntry;

class RFButtonClass {
   public:

    RFButtonEntry *entries = nullptr;

    RFButtonClass() {
    }
    ~RFButtonClass() {
    }

    RFButtonEntry *add(uint8_t _id, unsigned long _code, bool _longClickEnabled, bool _doubleClickEnable, RFButtonCallback _callback) {
        RFButtonEntry *entry = new RFButtonEntry();
        entry->id = _id;
        entry->buttonCode = _code;
        entry->longClickEnabled = _longClickEnabled;
        entry->doubleClickEnable = _doubleClickEnable;
        entry->callback = _callback;

        entry->next = entries;
        entries = entry;
        return entry;
    }

    void handleRFSignal(unsigned long value) {
        RFButtonEntry *entry = entries;
        while (entry) {
            if (value == entry->buttonCode) {
                handleRFSignal(entry);
            }
            entry = entry->next;
        }
    }

    void loop() {
        RFButtonEntry *entry = entries;
        while (entry) {
            handleEntry(entry);
            entry = entry->next;
        }
    }

   private:
    void handleRFSignal(RFButtonEntry *entry) {
        entry->lastPressedTime = millis();
        if (entry->eventOccured) {
            return;
        }

        if (entry->firstPressedTime == 0) {  // Pressed event
            Serial.println("Pressed");

            if (!entry->longClickEnabled && !entry->doubleClickEnable) {
                clickEvent(entry);
            } else if (entry->doubleClickEnable && entry->eventSrartTime != 0 && millis() - entry->eventSrartTime <= entry->doubleclick_threshold) {  // if eventSrartTime != 0 then this is second (not first) press
                doubleClickEvent(entry);
            }

            entry->firstPressedTime = millis();
            if (entry->eventSrartTime == 0) {
                entry->eventSrartTime = entry->firstPressedTime;
            }
        } else if (entry->longClickEnabled && millis() - entry->firstPressedTime >= entry->longclick_threshold) {
            longClickEvent(entry);
        }
    }

    void handleEntry(RFButtonEntry *entry) {
        if (entry->lastPressedTime != 0) {
            if (millis() - entry->lastPressedTime > entry->stable_threshold) {  // Unpressed
                Serial.println("Unpressed");

                if (!entry->eventOccured && !entry->doubleClickEnable) {
                    clickEvent(entry);
                }

                entry->lastPressedTime = 0;
                entry->firstPressedTime = 0;
            }
        } else if (!entry->eventOccured && entry->doubleClickEnable && entry->eventSrartTime != 0 && millis() - entry->eventSrartTime > entry->doubleclick_threshold) {
            clickEvent(entry);
        }

        if (entry->eventOccured && entry->lastPressedTime == 0 && entry->eventSrartTime != 0) {
            entry->eventOccured = false;
            entry->eventSrartTime = 0;
        }
    }

    void clickEvent(RFButtonEntry *entry) {
        entry->eventOccured = true;
        entry->callback(RFBUTTONEVENT_SINGLECLICK);
        Serial.println(">>> clickEvent");
    }

    void longClickEvent(RFButtonEntry *entry) {
        entry->eventOccured = true;
        entry->callback(RFBUTTONEVENT_LONGCLICK);
        Serial.println(">>> longClickEvent");
    }

    void doubleClickEvent(RFButtonEntry *entry) {
        entry->eventOccured = true;
        entry->callback(RFBUTTONEVENT_DOUBLECLICK);
        Serial.println(">>> doubleClickEvent");
    }
};

RFButtonClass RFButton;

#endif /* ESPBUTTON_H_ */