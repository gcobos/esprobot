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
                (int)axis[0]["quantum"]
            );
            actuators_configure_axis_duty(
                (actuator_id<<ACTUATOR_BASE)+axis_index,
                (int)axis[0]["duties"][0], (int)axis[0]["duties"][1]
            );
        }
        actuators_configure_address(actuator_id, it->value["address"]);
    }
    webserver.send( 200, "text/json", "{result:true}");
}

void handleMotion(void)
{
    StaticJsonBuffer<2000> plain_json;

    JsonObject& root = plain_json.parseObject(webserver.arg("plain"));

    root.prettyPrintTo(Serial);

    JsonObject& actuators = root["actuators"];

    for(JsonObject::iterator it=actuators.begin(); it!=actuators.end(); ++it) {
        int actuator_id = it->key[0] - '0';
        JsonArray& axis = it->value;

        for (int axis_index = 0; axis_index < NUM_AXIS_PER_ACTUATOR; axis_index++) {
            scheduler_move_axis((actuator_id << ACTUATOR_BASE)+axis_index, (int)it->value[axis_index]);
        }
    }
    webserver.send( 200, "text/json", "{result:true}");
}

void ICACHE_FLASH_ATTR handleRoot(void)
{
    webserver.send(200, "text/plain", "hello from esp8266!");
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

    webserver.on("/motion", handleMotion);

  	webserver.on("/inline", []() {
        for (int i = 0; i < 16; i+=2) {
            scheduler_move_axis(i, 10);
        }
        Serial.println("Acabo de mover los ejes");
    	webserver.send(200, "text/plain", "this works as well");
  	});

  	webserver.onNotFound(handleNotFound);

    // Initiate HTTP server
    webserver.begin();
    Serial.println("HTTP server started");
}

void ICACHE_FLASH_ATTR webservice_handle(void) {
    webserver.handleClient();
}
