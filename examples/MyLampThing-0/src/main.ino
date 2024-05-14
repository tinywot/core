/*!
  SPDX-FileCopyrightText: 2024 Junde Yhi <junde@yhi.moe>
  SPDX-License-Identifier: MIT
*/

#include <string.h>

#include <ArduinoHttpServer.h>
#include <Ethernet.h>
#include <tinywot/core.h>

static char const application_json[] = "application/json";

static unsigned char t_scratchpad[32] = {};
static struct tinywot_request t_request = {};
static struct tinywot_response t_response = {};
static struct tinywot_thing t_thing = {};

static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte ip[] = { 10, 36, 24, 51 };
static EthernetServer server = EthernetServer(80);

void setup(void) {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Ethernet.begin(mac, ip);
  server.begin();
}

void loop(void) {
  enum tinywot_status t_status = TINYWOT_STATUS_UNKNOWN;

  Ethernet.maintain();

  EthernetClient eth_client = server.available();
  if (!eth_client) {
    return;
  }

  ArduinoHttpServer::StreamHttpRequest<256> httpRequest(eth_client);

  if (httpRequest.readRequest()) {
    ArduinoHttpServer::Method method = httpRequest.getMethod();

    if (method == ArduinoHttpServer::Method::Get) {
      t_request.op = TINYWOT_OPERATION_TYPE_READPROPERTY;
    } else if (method == ArduinoHttpServer::Method::Post) {
      t_request.op = TINYWOT_OPERATION_TYPE_WRITEPROPERTY;
    } else {
      t_request.op = TINYWOT_OPERATION_TYPE_UNKNOWN;
    }

    const String& href = httpRequest.getResource().toString();
    memcpy(t_request.target, href.c_str(), href.length());

    t_response.payload.content = t_scratchpad;

    t_status = tinywot_thing_process_request(&t_thing, &t_response, &t_request);
    if (t_status != TINYWOT_STATUS_SUCCESS) {
      client.stop();
      memset(t_scratchpad, 0, sizeof(t_scratchpad));
      memset(&t_request, 0, sizeof(struct tinywot_request));
      memset(&t_response, 0, sizeof(struct tinywot_response));
      return;
    }

    // if (method == ArduinoHttpServer::Method::Get) {
    //   if (httpRequest.getResource().toString() == "/status") {
    //     ArduinoHttpServer::StreamHttpReply response(client, application_json);
    //     response.send(digitalRead(13) == HIGH ? "true" : "false");
    //   } else {
    //     ArduinoHttpServer::StreamHttpErrorReply response(client, application_json, "404");
    //   }
    // } else {
    //   ArduinoHttpServer::StreamHttpErrorReply response(client, application_json, "405");
    // }
  }

  client.stop();

  memset(t_scratchpad, 0, sizeof(t_scratchpad));
  memset(&t_request, 0, sizeof(struct tinywot_request));
  memset(&t_response, 0, sizeof(struct tinywot_response));
}
