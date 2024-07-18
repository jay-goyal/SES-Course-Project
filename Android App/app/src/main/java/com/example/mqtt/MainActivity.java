package com.example.mqtt;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

import android.os.Handler;
import android.os.Looper;

import javax.net.ssl.SSLSocketFactory;

public class MainActivity extends AppCompatActivity {
    private MqttClient client;
    private static final String BROKER_URL = "tcp://192.168.83.132:1883";
    TextView temp_data;
    TextView humidity_data;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        temp_data = (TextView) findViewById(R.id.temp_data);
        humidity_data = (TextView) findViewById(R.id.humidity_data);

        String clientID = MqttClient.generateClientId();
        try {
            // Set up the persistence layer
            MemoryPersistence persistence = new MemoryPersistence();

            // Initialize the MQTT client
            client = new MqttClient(BROKER_URL, clientID, persistence);
            MqttConnectOptions connectOptions = new MqttConnectOptions();
            client.connect(connectOptions);
            if(client.isConnected()) {
                client.setCallback(new MqttCallback() {
                    @Override
                    public void connectionLost(Throwable cause) {
                        Log.d("connection_lost", cause.toString());
                    }

                    @Override
                    public void messageArrived(String topic, MqttMessage message) throws Exception {
                        String msg = new String(message.getPayload());
                        String[] sensorData = msg.split(",");
                        Log.d("sensor data", sensorData[0] + "," + sensorData[1]);
                        new Handler(Looper.getMainLooper()).post(new Runnable() {
                            @Override
                            public void run() {
                                temp_data.setText(sensorData[0]);
                                humidity_data.setText(sensorData[1]);
                            }
                        });
                    }

                    @Override
                    public void deliveryComplete(IMqttDeliveryToken token) {
                        Log.d("delivery_complete", token.toString());

                    }
                });
            }
        } catch (MqttException e) {

            e.printStackTrace();
        }
        subscribe("g4/sensor_data");
        Toast.makeText(this, "Client connected and subscribed to g4/sensor_data", Toast.LENGTH_LONG).show();
    }

    @Override
    protected void onDestroy() {
        disconnect();
        super.onDestroy();
    }
    public void disconnect() {
        try {
            client.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void publish(View v) {
        try {
            EditText inputTargetTemp = (EditText)findViewById(R.id.temp_target);
            float targetTemp = Float.parseFloat(inputTargetTemp.getText().toString());
            EditText inputTargetHum = (EditText)findViewById(R.id.hum_target);
            float targetHum = Float.parseFloat(inputTargetHum.getText().toString());
            String message = String.format("%.2f,%.2f", targetTemp, targetHum);
            MqttMessage mqttMessage = new MqttMessage(message.getBytes());
            client.publish("g4/target_data", mqttMessage);
            Toast.makeText(this, "Published successfully", Toast.LENGTH_LONG).show();
            Log.d("publish_success", "Publish successful -> " + message);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
    public void subscribe(String topic) {
        try {
            client.subscribe(topic);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}