/*
 * FritzBoy Request-Client
 * Sender: Sends SOAP-requests to the router
*/

// Maximum bandwidth usage
const String soapAction_GetCommonLinkProperties = "SoapAction:urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetCommonLinkProperties";
const String soapU_GetCommonLinkProperties  = "<u:GetCommonLinkProperties xmlns:u=urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1 />";

// Current bandwidth usage
const String soapAction_GetAddonInfos = "SoapAction:urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetAddonInfos";
const String soapU_GetAddonInfos = "<u:GetAddonInfos xmlns:u=urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1 />";

boolean sendRequest_maximumbandwidth() {
  return sendRequest(soapAction_GetCommonLinkProperties, soapU_GetCommonLinkProperties);
}

boolean sendRequest_bandwidthusage() {
  return sendRequest(soapAction_GetAddonInfos, soapU_GetAddonInfos);
}

boolean sendRequest(String soapAction, String soapU) {
  Serial.println("Sending request to router");

  rate_receive_abs = UNDEFINED;
  rate_send_abs = UNDEFINED;

  // if you get a connection, report back via serial:
  if (client.connect(Ethernet.gatewayIP(), 49000)) {

    Serial.println("Sending POST request");
    request = "POST /igdupnp/control/WANCommonIFC1 HTTP/1.1\r\n";
    request += "HOST: ";
    request += router_ip;
    request += ":49000\r\n";
    //client.print(Ethernet.gatewayIP());
    request += "User-Agent: fritzboy\r\n";
    request += "Accept: */*\r\n";
    request += "Content-Type: text/xml; charset=utf-8\r\n";
    request += soapAction;
    request += "\r\n";
    // Serial.print(request);
    client.print(request);
    delay(SENDING_DELAY);
    request = "Content-Length: ";
    String xmldata1 = "<?xml version='1.0' encoding='utf-8'?> <s:Envelope s:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/' xmlns:s='http://schemas.xmlsoap.org/soap/envelope/'>\r\n<s:Body> ";
    // ... soapU ...
    String xmldata3 = " </s:Body> </s:Envelope>\r\n\r\n";
    int contentlength = xmldata1.length() + soapU.length() + xmldata3.length();
    request += contentlength;
    request += "\r\n\r\n";

    // Serial.print(request);
    client.print(request);
    delay(SENDING_DELAY);
    // Serial.print(xmldata1);
    client.print(xmldata1);
    delay(SENDING_DELAY);
    // Serial.print(soapU);
    client.print(soapU);
    delay(SENDING_DELAY);
    // Serial.print(xmldata3);
    client.print(xmldata3);
    delay(SENDING_DELAY);
    client.flush();
    delay(SENDING_DELAY);
    Serial.println("Post request sent");
    return true;
  }
  else {
    Serial.println("connection failed");
    return false;
  }
}
