/*
 * FritzBoy Request-Client
 * Send messages to Gameboy
*/

boolean pushValues() {
  // Send current values to gameboy
  if (maximum_downstream_short != UNDEFINED && maximum_upstream_short != UNDEFINED && rate_receive_percent != UNDEFINED && rate_send_percent != UNDEFINED) {
    Serial.print("Sending maximum and current bandwidth curDown%/CurUp% | curDownA/curUpA | maxDown/maxUp: ");
    Serial.print(rate_receive_percent);
    Serial.print("/");
    Serial.print(rate_send_percent);
    Serial.print(" | ");
    temp_a = rate_receive_abs / 1000000;
    Serial.print(temp_a);
    Serial.print("/");
    temp_b = rate_send_abs / 1000000;
    Serial.print(temp_b);
    Serial.print(" | ");
    Serial.print(maximum_downstream_short);
    Serial.print("/");
    Serial.println(maximum_upstream_short);

    sendValuesToGB(rate_receive_percent, rate_send_percent, temp_a, temp_b, maximum_downstream_short, maximum_upstream_short);
    return true;
  }
  return false;
}

// Format: [DOWNSTREAM-GRAPH][UPSTREAM-GRAPH][DOWNSTREAM-ABSOLUTE][UPSTREAM-ABSOLUTE][DOWNSTREAM-MAXIMUM][UPSTREAM-MAXIMUM]
void sendValuesToGB(byte downstream_percent, byte upstream_percent, byte downstream_absolute, byte upstream_absolute, byte maximum_downstream_short, byte maximum_upstream_short) {
  // i.e. "123456";
  outbuffer[0] = downstream_percent;
  outbuffer[1] = upstream_percent;
  outbuffer[2] = downstream_absolute;
  outbuffer[3] = upstream_absolute;
  outbuffer[4] = maximum_downstream_short;
  outbuffer[5] = maximum_upstream_short;
  sendBytesToGB(outbuffer, 6);
  delay(400);
}

// Converts the values to a bytestream and sends it
void sendBytesToGB(byte* out, int out_length) {
  Serial.print("Sending:");

  for (int i = 0; i < out_length; i++) {
    Serial.print(out[i]);
    Serial.print('-');
    byte dataToSend = out[i];
    if (dataToSend == 255) // End-byte!
      dataToSend = 254;
    sendByte(dataToSend);
    delay(DELAYBETWEENBITS);
  }

  // Send end-byte
  sendByte(255);
  Serial.println("E");
  delay(DELAYBETWEENBITS);
  digitalWrite(PIN_LED, LOW);
}

// Send a single byte
void sendByte(byte out) {
  for (byte counter = 0; counter < 8; counter++ )
  {
    if ( out & 0x80 ) { // Logical 1
      digitalWrite(PIN_LED, HIGH);
      digitalWrite(PIN_SO, HIGH );
      digitalWrite(PIN_CLOCK, LOW );
      delayMicroseconds(DELAYBETWEENBITS);
      digitalWrite(PIN_CLOCK, HIGH );
      delayMicroseconds(DELAYBETWEENBITS);
    } else { // Logical 0
      digitalWrite(PIN_LED, LOW);
      digitalWrite(PIN_SO, LOW );
      digitalWrite(PIN_CLOCK, LOW );
      delayMicroseconds(DELAYBETWEENBITS);
      digitalWrite( PIN_CLOCK, HIGH );
      delayMicroseconds(DELAYBETWEENBITS);
    }
    out = out << 1; // Next bit
  }
}
