/*
 * FritzBoy Request-Client
 * Helper functions
*/

String convertAddress(IPAddress address)
{
  return String(address[0]) + "." +
         String(address[1]) + "." +
         String(address[2]) + "." +
         String(address[3]);
}

// Clears the buffer
int resetBuffer() {
  for (int i = 0; i < request.length(); i++)
    request[i] = '-';

  for (int i = 0; i < BUFFER_SIZE; i++)
    response[i] = '-';
}

void errorstate() {
  while (true) {
    digitalWrite(PIN_LED, HIGH);
    delay(100);
    digitalWrite(PIN_LED, LOW);
    delay(100);
  }
}
