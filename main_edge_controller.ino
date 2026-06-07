#include <WiFi.h>
#include <PubSubClient.h>

// --- Hardware & Pin Configuration ---
const int SOLENOID_PIN = 22; // MOSFET Gate driving the 12V Solenoid

// --- Network & MQTT Configuration ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const char* lwt_topic = "telemetry/lock01/status";
const char* auth_topic = "telemetry/lock01/events";

WiFiClient espClient;
PubSubClient client(espClient);

// --- Local Failover Storage Buffer ---
struct LocalLog {
    String userId;
    String method;
    unsigned long timestamp;
};
LocalLog offlineBuffer[10]; 
int bufferCount = 0;

void setup() {
    Serial.begin(115200);
    pinMode(SOLENOID_PIN, OUTPUT);
    digitalWrite(SOLENOID_PIN, LOW); // Lock engaged by default
    
    setup_wifi();
    client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
    delay(10);
    WiFi.begin(ssid, password);
    // Non-blocking approach for network resilience initialization
    long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 5000) {
        delay(500);
    }
}

void reconnect_mqtt() {
    if (!client.connected()) {
        // Connect with Last Will and Testament (LWT) for cloud health auditing
        if (client.connect("ESP32_Lock_Node", lwt_topic, 1, true, "Offline")) {
            client.publish(lwt_topic, "Online", true);
            sync_offline_buffer();
        }
    }
}

void process_authentication(String uid, String method) {
    unsigned long startTime = millis();
    bool accessGranted = verify_local_hash(uid); // Local template matching (Edge Layer)
    unsigned long latency = millis() - startTime;

    if (accessGranted) {
        trigger_solenoid();
        log_event(uid, method, latency, "Success");
    } else {
        log_event(uid, method, latency, "Failed");
    }
}

bool verify_local_hash(String inputUid) {
    // Simulated local SHA-hash matching protocol execution
    if (inputUid == "13A4F92B" || inputUid == "user_fingerprint_hash_placeholder") {
        return true; 
    }
    return false;
}

void trigger_solenoid() {
    digitalWrite(SOLENOID_PIN, HIGH); // Disengage lock (Open)
    delay(3000);                      // Hold period
    digitalWrite(SOLENOID_PIN, LOW);  // Re-engage lock (Secure)
}

void log_event(String uid, String method, unsigned long latency, String result) {
    String payload = "{\"userId\":\"" + uid + "\",\"method\":\"" + method + "\",\"latency\":" + String(latency) + ",\"result\":\"" + result + "\"}";
    
    if (WiFi.status() == WL_CONNECTED && client.connected()) {
        client.publish(auth_topic, payload.c_str());
    } else {
        // Network disruption logic: Failover safely to Local Storage Buffer
        if (bufferCount < 10) {
            offlineBuffer[bufferCount] = {uid, method, millis()};
            bufferCount++;
            Serial.println("[FAILOVER] Network offline. Telemetry event buffered locally.");
        }
    }
}

void sync_offline_buffer() {
    if (bufferCount == 0) return;
    Serial.println("[SYNC] Connection re-established. Flushing local storage buffer to cloud...");
    for (int i = 0; i < bufferCount; i++) {
        String payload = "{\"userId\":\"" + offlineBuffer[i].userId + "\",\"method\":\"" + offlineBuffer[i].method + "\",\"syncStatus\":\"Buffered\"}";
        client.publish(auth_topic, payload.c_str());
    }
    bufferCount = 0; // Clear buffer
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        reconnect_mqtt();
        client.loop();
    }
    // Edge authentication loops continue running autonomously here regardless of WiFi state
}
