#ifndef WATCHDOG_H_
#define WATCHDOG_H_
//------------------NECESSARY-----------
#include <Arduino.h>
#include <commonvar.h>
#include <commondef.h>
#include "driver/rtc_io.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>

bool watchdog_task();



struct error_struct {
    char error_type;
    uint32_t error_code;
    char timestamp[20]; // ISO 8601 format
};

class errorBuffer {
private:
    error_struct* buffer;  // Pointer ke array buffer
    int capacity;                      // Kapasitas maksimum buffer
    int head;                          // Indeks elemen tertua
    int tail;                          // Indeks berikutnya untuk penulisan
    int count;                         // Jumlah elemen saat ini

public:
    // Constructor: Inisialisasi buffer dengan ukuran tertentu
    errorBuffer(int size) 
        : capacity(size), head(0), tail(0), count(0) {
        buffer = new error_struct[capacity];
    }

    // Destructor: Dealokasi memori
    ~errorBuffer() {
        delete[] buffer;
    }

    /**
     * Menyisipkan data baru ke buffer.
     * Jika buffer penuh, data tertua (head) akan ditimpa.
     */
    void insert(const error_struct& newData) {
        error_struct bufferData;
        if(this->get(newData.error_code, bufferData)){
            // Jika data sudah ada, update
            this->update(newData.error_code, newData);
            return;
        }

        if (count == capacity) {
            // Buffer penuh: timpa data tertua (head)
            buffer[head] = newData;
            head = (head + 1) % capacity; // Geser head ke elemen berikutnya
            tail = head; // Tail mengikuti head karena penuh
        } 
        
        else {
            // Buffer belum penuh: tambahkan di tail
            buffer[tail] = newData;
            tail = (tail + 1) % capacity;
            count++;
        }
    
    }

    /**
     * Update data berdasarkan error_code.
     * 
     * @param error_code ID yang dicari
     * @param updateData Data baru untuk diupdate
     * @return true jika data ditemukan dan diupdate, false jika tidak
    */
    
    bool update(uint32_t error_code, const error_struct& updateData) {
        for (int i = 0; i < count; i++) {
            int index = (head + i) % capacity;
            if (buffer[index].error_code == error_code) {
                buffer[index] = updateData;
                return true;
            }
        }
        return false;
    }

    /**
     * Mengambil data berdasarkan message_id.
     * 
     * @param error_code ID yang dicari
     * @param value Output untuk menyalin data yang ditemukan
     * @return true jika data ditemukan, false jika tidak
     */
    bool get(uint32_t error_code, error_struct& value) {
        for (int i = 0; i < count; i++) {
            int index = (head + i) % capacity;
            if (buffer[index].error_code == error_code) {
                value = buffer[index];
                return true;
            }
        }
        return false;
    }

    bool get(JsonDocument &doc){
        if (count ==0)
            return false; // Tidak ada data untuk dikembalikan

        JsonArray array = doc.to<JsonArray>();
        for (int i = 0; i < count; i++) {
            int index = (head + i) % capacity;
            JsonObject object = array.add<JsonObject>();
            if(buffer[index].error_type=='w'){
                object["type"] = "warning";
            }
            else if(buffer[index].error_type=='e'){
                object["type"] = "error";
            }
            else if(buffer[index].error_type=='i'){
                object["type"] = "info";
            }
            else{
                object["type"] = "unknown";
            }
            object["code"] = buffer[index].error_code;
            object["timestamp"] = buffer[index].timestamp;
            }
        return true;
    }
};


bool get_error_status(JsonDocument &doc);

bool get_error_status(error_struct &error);

bool insert_errorcode(error_struct &error);

#endif