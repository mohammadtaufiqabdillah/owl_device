#ifndef RADIOTASK_H
#define RADIOTASK_H

#include <Arduino.h>
#include <RadioLib.h>
#include <commonvar.h>
#include <commondef.h>
#include <commandHandler.h>

#define MAX_PACKAGE_STATUS_HOLD 7
#define MAX_CHANNEL_RADIO 4

struct radio_status_struct {
    bool connected;
    uint8_t channel;
    float rssi;
    float snr;
};

class RadioAckBuffer {
private:
    radio_acknowledge_struct* buffer;  // Pointer ke array buffer
    int capacity;                      // Kapasitas maksimum buffer
    int head;                          // Indeks elemen tertua
    int tail;                          // Indeks berikutnya untuk penulisan
    int count;                         // Jumlah elemen saat ini

public:
    // Constructor: Inisialisasi buffer dengan ukuran tertentu
    RadioAckBuffer(int size) 
        : capacity(size), head(0), tail(0), count(0) {
        buffer = new radio_acknowledge_struct[capacity];
    }

    // Destructor: Dealokasi memori
    ~RadioAckBuffer() {
        delete[] buffer;
    }

    /**
     * Menyisipkan data baru ke buffer.
     * Jika buffer penuh, data tertua (head) akan ditimpa.
     */
    void insert(const radio_acknowledge_struct& newData) {
        if (count == capacity) {
            // Buffer penuh: timpa data tertua (head)
            buffer[head] = newData;
            head = (head + 1) % capacity; // Geser head ke elemen berikutnya
            tail = head; // Tail mengikuti head karena penuh
        } else {
            // Buffer belum penuh: tambahkan di tail
            buffer[tail] = newData;
            tail = (tail + 1) % capacity;
            count++;
        }
    }

    /**
     * Update data berdasarkan message_id.
     * 
     * @param message_id ID yang dicari
     * @param updateData Data baru untuk diupdate
     * @return true jika data ditemukan dan diupdate, false jika tidak
     */
    bool update(uint32_t message_id, const radio_acknowledge_struct& updateData) {
        for (int i = 0; i < count; i++) {
            int index = (head + i) % capacity;
            if (buffer[index].message_id == message_id) {
                buffer[index] = updateData;
                return true;
            }
        }
        return false;
    }

    /**
     * Mengambil data berdasarkan message_id.
     * 
     * @param message_id ID yang dicari
     * @param value Output untuk menyalin data yang ditemukan
     * @return true jika data ditemukan, false jika tidak
     */
    bool get(uint32_t message_id, radio_acknowledge_struct& value) {
        for (int i = 0; i < count; i++) {
            int index = (head + i) % capacity;
            if (buffer[index].message_id == message_id) {
                value = buffer[index];
                return true;
            }
        }
        return false;
    }

    bool get(JsonDocument &doc){
        JsonArray array = doc.to<JsonArray>();
        for (int i = 0; i < count; i++) {
            int index = (head + i) % capacity;
            JsonObject object = array.add<JsonObject>();
            object["message_id"] = buffer[index].message_id;
            object["message_type"] = buffer[index].message_type;
            object["status_send"] = buffer[index].status_send;
            object["rssi"] = buffer[index].rssi;
            object["snr"] = buffer[index].snr;
            object["timeonair"] = buffer[index].timeonair;
        }
        return true;
    }
};

bool get_radio_status(radio_status_struct &result);

bool get_radio_acknowledge_status (JsonDocument &doc);

bool get_radio_acknowledge_status (const uint32_t message_id, radio_acknowledge_struct &data);

bool radio_task();

#endif