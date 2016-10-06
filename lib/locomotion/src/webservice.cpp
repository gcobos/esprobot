#include "webservice.h"

MDNSResponder mdns;
ESP8266WebServer webserver(80);

void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += webserver.uri();
    message += "\nMethod: ";
    message += (webserver.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += webserver.args();
    message += "\n";
    for (uint8_t i=0; i<webserver.args(); i++){
        message += " " + webserver.argName(i) + ": " + webserver.arg(i) + "\n";
    }
    webserver.send(404, "text/plain", message);
}

void handleConfig(void)
{
    StaticJsonBuffer<2000> plain_json;

    JsonObject& root = plain_json.parseObject(webserver.arg("plain"));

    root.prettyPrintTo(Serial);

    JsonObject& actuators = root["actuators"];

    for(JsonObject::iterator it=actuators.begin(); it!=actuators.end(); ++it) {
        int actuator_id = it->key[0] - '0';
        JsonArray& axis = it->value["axis"];

        for (int axis_index = 0; axis_index < NUM_AXIS_PER_ACTUATOR; axis_index++) {
            actuators_configure_axis_quantum(
                (actuator_id<<ACTUATOR_BASE)+axis_index,
                (int)axis[axis_index]["quantum"]
            );
            actuators_configure_axis_duty(
                (actuator_id<<ACTUATOR_BASE)+axis_index,
                (int)axis[axis_index]["duties"][0], (int)axis[axis_index]["duties"][1]
            );
            actuators_configure_axis_inactivity_period(
                (actuator_id<<ACTUATOR_BASE)+axis_index,
                (int)axis[axis_index]["inactivity_period"]
            );
            Serial.print("Inactivity period now: ");
            Serial.print((int)axis[axis_index]["inactivity_period"]);
        }
        actuators_configure_address(actuator_id, (int)it->value["address"]);
    }
    webserver.send( 200, "text/json", "{\"result\":true}");
}

void handleGetMotion(void)
{
    StaticJsonBuffer<2000> plain_json;
    String output = "";

    JsonObject& root = plain_json.createObject();
    JsonObject& actuators = root.createNestedObject("actuators");

    for (int16_t actuator_id = 0; actuator_id < NUM_ACTUATORS; actuator_id++) {
        JsonArray& durations = actuators.createNestedArray(String(actuator_id));

        for (int axis_index = 0; axis_index < NUM_AXIS_PER_ACTUATOR; axis_index++) {
            durations.add(scheduler_get_axis_vector((actuator_id << ACTUATOR_BASE)+axis_index));
        }
    }
    root.printTo(output);
    webserver.send(200, "text/json", output);
}


void handlePostMotion(void)
{
    StaticJsonBuffer<2000> plain_json;

    JsonObject& root = plain_json.parseObject(webserver.arg("plain"));

    root.prettyPrintTo(Serial);

    JsonObject& actuators = root["actuators"];

    for(JsonObject::iterator it=actuators.begin(); it!=actuators.end(); ++it) {
        int actuator_id = String(it->key[0]).toInt();
        JsonArray& axis = it->value;

        for (int axis_index = 0; axis_index < NUM_AXIS_PER_ACTUATOR; axis_index++) {
            scheduler_move_axis((actuator_id << ACTUATOR_BASE)+axis_index, (int)it->value[axis_index]);
        }
    }
    webserver.send( 200, "text/json", "{\"result\":true}");
}

void ICACHE_FLASH_ATTR handleRoot(void)
{
    webserver.send(200, "text/plain", "ESP Robot API");
}

// Initialize webserver and attach handlers
void ICACHE_FLASH_ATTR webservice_init(IPAddress ip)
{
    if (mdns.begin("esp8266", ip)) {
        Serial.println("MDNS responder started");
    }

    // Declare handlers for every path
    webserver.on("/", handleRoot);
    webserver.on("/config", handleConfig);
    webserver.on("/motion", HTTP_GET, handleGetMotion);
    webserver.on("/motion", HTTP_POST, handlePostMotion);
  	webserver.onNotFound(handleNotFound);

    // Initiate HTTP server
    webserver.begin();
    Serial.println("HTTP server started");
}

void ICACHE_FLASH_ATTR webservice_handle(void) {
    webserver.handleClient();
}
