/*
 * FritzBoy Request-Client
 * Handler: Handles resonses from the router
*/

boolean handle_response()
{
  // Parse response for maximum upstream rate
  if (maximum_upstream_long == UNDEFINED) {
    if (strstr(response, "</NewLayer1UpstreamMaxBitRate") != NULL) {
      Serial.print("//Found maximum upstream//");
      pch = strrchr(response, '<');
      lastIntegerPosition = pch - response - 1;
      pch = strrchr(response, '>');
      firstIntegerPosition = pch - response + 1;

      if (lastIntegerPosition - firstIntegerPosition == 0)
        rate_receive_abs = 0;
      else {
        char subbuff[lastIntegerPosition - firstIntegerPosition + 2];
        memcpy(subbuff, &response[firstIntegerPosition], lastIntegerPosition - firstIntegerPosition + 1);
        subbuff[lastIntegerPosition - firstIntegerPosition + 1] = '\0';

        maximum_upstream_long = atol(subbuff);
        if (maximum_upstream_long == UNDEFINED) // Unsigned long
          maximum_upstream_long++;
        maximum_upstream_short = maximum_upstream_long / 1000000;

        Serial.print("Maximum upstream:");
        Serial.println(maximum_upstream_short);
      }
    }
  }

  // Parse response for maximum upstream rate
  if (maximum_downstream_long == UNDEFINED) {
    if (strstr(response, "</NewLayer1DownstreamMaxBitRate") != NULL) {
      Serial.print("//Found maximum downstream//");

      pch = strrchr(response, '<');
      lastIntegerPosition = pch - response - 1;
      pch = strrchr(response, '>');
      firstIntegerPosition = pch - response + 1;

      if (lastIntegerPosition - firstIntegerPosition == 0)
        rate_receive_abs = 0;
      else {
        char subbuff[lastIntegerPosition - firstIntegerPosition + 2];
        memcpy(subbuff, &response[firstIntegerPosition], lastIntegerPosition - firstIntegerPosition + 1);
        subbuff[lastIntegerPosition - firstIntegerPosition + 1] = '\0';

        maximum_downstream_long = atol(subbuff);
        if (maximum_downstream_long == UNDEFINED) // Unsigned long
          maximum_downstream_long++;
        maximum_downstream_short = maximum_downstream_long / 1000000;

        Serial.print("Maximum downstream:");
        Serial.println(maximum_downstream_short);
      }
    }
  }

  // Parse response for receive rate
  if (!response_found_downstream) {
    if (strstr(response, "</NewByteReceiveRate") != NULL) {
      Serial.print("//Found current receive//");

      pch = strrchr(response, '<');
      lastIntegerPosition = pch - response - 1;
      pch = strrchr(response, '>');
      firstIntegerPosition = pch - response + 1;

      if (lastIntegerPosition - firstIntegerPosition == 0)
        rate_receive_abs = 0;
      else {
        char subbuff[lastIntegerPosition - firstIntegerPosition + 2];
        memcpy(subbuff, &response[firstIntegerPosition], lastIntegerPosition - firstIntegerPosition + 1);
        subbuff[lastIntegerPosition - firstIntegerPosition + 1] = '\0';

        rate_receive_abs = atol(subbuff);
        if (rate_receive_abs == UNDEFINED) // Unsigned long
          rate_receive_abs++;
        rate_receive_abs = rate_receive_abs;

        Serial.print("rate_receive_abs (byte/bits):");
        Serial.print(rate_receive_abs);
        Serial.print('/');
        rate_receive_abs = rate_receive_abs * 8; // Convert byte to bit
        Serial.println(rate_receive_abs);

        temp = (unsigned long) rate_receive_abs * (unsigned long) 50;
        rate_receive_percent = temp / maximum_downstream_long;
        if (rate_receive_percent > 49) // catch Maximum
          rate_receive_percent = 49;

        Serial.print("receiverate abs/percent:");
        Serial.print(rate_receive_abs);
        Serial.print('/');
        Serial.println(rate_receive_percent);

      }
      response_found_downstream = true;
    }
  }

  // Parse response for send rate
  if (!response_found_upstream) {
    if (strstr(response, "</NewByteSendRate") != NULL) {
      Serial.print("//Found current send//");
      pch = strrchr(response, '<');
      lastIntegerPosition = pch - response - 1;
      pch = strrchr(response, '>');
      firstIntegerPosition = pch - response + 1;

      if (lastIntegerPosition - firstIntegerPosition == 0)
        rate_send_abs = 0;
      else
      {
        char subbuff[lastIntegerPosition - firstIntegerPosition + 2];
        memcpy(subbuff, &response[firstIntegerPosition], lastIntegerPosition - firstIntegerPosition + 1);
        subbuff[lastIntegerPosition - firstIntegerPosition + 1] = '\0';
        // Serial.print("Delta:");
        // Serial.println(subbuff);

        rate_send_abs = atol(subbuff);
        if (rate_send_abs == UNDEFINED) // Unsigned long
          rate_send_abs++;
        rate_send_abs = rate_send_abs;

        Serial.print("rate_send_abs (byte/bits):");
        Serial.print(rate_send_abs);
        Serial.print('/');
        rate_send_abs = rate_send_abs * 8; // Convert byte to bit
        Serial.println(rate_send_abs);

        // highest_downstream = max(highest_downstream, rate_receive_abs);
        // Serial.print("highest_downstream:");
        // Serial.println(highest_downstream);
        // unsigned long temp = (long) rate_receive_abs * (long) 50;
        // rate_receive_percent = temp / highest_downstream;
        temp = (unsigned long) rate_send_abs * (unsigned long) 50;
        rate_send_percent = temp / maximum_upstream_long;
        if (rate_send_percent > 49) // catch Maximum
          rate_send_percent = 49;

        Serial.print("sendrate abs/percent:");
        Serial.print(rate_send_abs);
        Serial.print('/');
        Serial.println(rate_send_percent);
      }
      response_found_upstream = true;
    }
  }

  if (response_found_downstream && response_found_upstream)
    return true;
  else
    return false;
}

