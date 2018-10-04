Code for wall sensor distance detection

'''
void loop() {
    // Read sensor value for analog pin A1
    float sensorValue = analogRead(A1);
    
    // convert to a distance
    float dis = 2076/(SensorValue - 11);
    
    if (dis < 4 || dis >30) {
        Serial.print( "Out of Range.\n");
    }
    else {
        Serial.print(dis);
        Serial.print(" cm\n");
    }
    delay(500);
}
'''
