package com.example.fencingremotecontroller;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.ToggleButton;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.view.View;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import static android.content.ContentValues.TAG;

public class MainActivity extends AppCompatActivity {

    private String deviceName = null;
    private String deviceAddress;
    public static Handler handler;
    public static BluetoothSocket mmSocket;
    public static ConnectedThread connectedThread;
    public static CreateConnectThread createConnectThread;

    private final static int CONNECTING_STATUS = 1; // used in bluetooth handler to identify message status
    private final static int MESSAGE_READ = 2; // used in bluetooth handler to identify message update
    private static final int REQUEST_ENABLE_BT = 1;

    private TextView score1;
    private TextView score2;
    private TextView time;
    private ToggleButton mute;
    private CountDownTimer countDown;
    private ToggleButton yellow1;
    private ToggleButton yellow2;
    private ToggleButton red1;
    private ToggleButton red2;
    private ToggleButton black1;
    private ToggleButton black2;


    // Function to check and request permission
    public void checkPermission(String permission, int requestCode) {
        // Checking if permission is not granted
        if (ContextCompat.checkSelfPermission(MainActivity.this, permission) == PackageManager.PERMISSION_DENIED) {
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{permission}, requestCode);
        } else {
            //Toast.makeText(MainActivity.this, "Permission already granted", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Check for bluetooth permissions
        checkPermission(Manifest.permission.BLUETOOTH_CONNECT, 1);
        checkPermission(Manifest.permission.BLUETOOTH_SCAN, 1);
        getSupportActionBar().hide();

        final Toolbar toolbar = findViewById(R.id.toolbar);
        final Button buttonConnect = findViewById(R.id.buttonConnect);
        yellow1 = findViewById(R.id.yellow1);
        yellow2 = findViewById(R.id.yellow2);
        red1 = findViewById(R.id.red1);
        red2 = findViewById(R.id.red2);
        black1 = findViewById(R.id.black1);
        black2 = findViewById(R.id.black2);
        score1 = findViewById(R.id.score1);
        score2 = findViewById(R.id.score2);
        time = findViewById(R.id.time);
        mute = findViewById(R.id.mute);
        ImageButton increment1 = findViewById(R.id.increment1);
        ImageButton decrement1 = findViewById(R.id.decrement1);
        ImageButton increment2 = findViewById(R.id.increment2);
        ImageButton decrement2 = findViewById(R.id.decrement2);
        Button timer1 = findViewById(R.id.timer1);
        Button timer3 = findViewById(R.id.timer3);

        // If a bluetooth device has been selected from SelectDeviceActivity
        deviceName = getIntent().getStringExtra("deviceName");
        if (deviceName != null) {
            // Get the device address to make BT Connection
            deviceAddress = getIntent().getStringExtra("deviceAddress");
            // Show progree and connection status
            toolbar.setSubtitle("Connecting to " + deviceName + "...");
            //progressBar.setVisibility(View.VISIBLE);
            buttonConnect.setEnabled(false);

            /*
            This is the most important piece of code. When "deviceName" is found
            the code will call a new thread to create a bluetooth connection to the
            selected device (see the thread code below)
             */
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            createConnectThread = new CreateConnectThread(bluetoothAdapter, deviceAddress);
            createConnectThread.start();
        }

        /*
        Second most important piece of Code. GUI Handler
         */
        handler = new Handler(Looper.getMainLooper()) {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case CONNECTING_STATUS:
                        switch (msg.arg1) {
                            case 1:
                                toolbar.setSubtitle("Connected to " + deviceName);
                                //progressBar.setVisibility(View.GONE);
                                buttonConnect.setEnabled(true);
                                //buttonToggle.setEnabled(true);
                                break;
                            case -1:
                                toolbar.setSubtitle("Device fails to connect");
                                //progressBar.setVisibility(View.GONE);
                                buttonConnect.setEnabled(true);
                                break;
                        }
                        break;

                    case MESSAGE_READ:
                        String arduinoMsg = msg.obj.toString(); // Read message from Arduino
                        switch (arduinoMsg.toLowerCase()) {
                            case "led is turned on":
                                //imageView.setBackgroundColor(getResources().getColor(R.color.colorOn));
                                //textViewInfo.setText("Arduino Message : " + arduinoMsg);
                                break;
                            case "led is turned off":
                                //imageView.setBackgroundColor(getResources().getColor(R.color.colorOff));
                                //textViewInfo.setText("Arduino Message : " + arduinoMsg);
                                break;
                        }
                        break;
                }
            }
        };

        // Select Bluetooth Device
        buttonConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // Move to adapter list
                Intent intent = new Intent(MainActivity.this, SelectDeviceActivity.class);
                startActivity(intent);
            }
        });
    /*
        // Button to ON/OFF LED on Arduino Board
        buttonToggle.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String cmdText = null;
                String btnState = buttonToggle.getText().toString().toLowerCase();
                switch (btnState){
                    case "turn on":
                        buttonToggle.setText("Turn Off");
                        // Command to turn on LED on Arduino. Must match with the command in Arduino code
                        cmdText = "<turn on>";
                        break;
                    case "turn off":
                        buttonToggle.setText("Turn On");
                        // Command to turn off LED on Arduino. Must match with the command in Arduino code
                        cmdText = "<turn off>";
                        break;
                }
                // Send command to Arduino board
                connectedThread.write(cmdText);
            }
        });
*/

        yellow1.setOnCheckedChangeListener((a, b) -> {
            if (yellow1.isChecked()) {
                yellow1.setBackgroundColor(Color.parseColor("#FFFF00"));
            } else {
                yellow1.setBackgroundColor(Color.parseColor("#FBFBCE"));
            }
        });
        yellow2.setOnCheckedChangeListener((a, b) -> {
            if (yellow2.isChecked()) {
                yellow2.setBackgroundColor(Color.parseColor("#FFFF00"));
            } else {
                yellow2.setBackgroundColor(Color.parseColor("#FBFBCE"));
            }
        });
        red1.setOnCheckedChangeListener((a, b) -> {
            if (red1.isChecked()) {
                red1.setBackgroundColor(Color.parseColor("#FF0000"));
            } else {
                red1.setBackgroundColor(Color.parseColor("#FFCECE"));
            }
        });
        red2.setOnCheckedChangeListener((a, b) -> {
            if (red2.isChecked()) {
                red2.setBackgroundColor(Color.parseColor("#FF0000"));
            } else {
                red2.setBackgroundColor(Color.parseColor("#FFCECE"));
            }
        });
        black1.setOnCheckedChangeListener((a, b) -> {
            if (black1.isChecked()) {
                black1.setBackgroundColor(Color.parseColor("#000000"));
            } else {
                black1.setBackgroundColor(Color.parseColor("#D8D8D8"));
            }
        });
        black2.setOnCheckedChangeListener((a, b) -> {
            if (black2.isChecked()) {
                black2.setBackgroundColor(Color.parseColor("#000000"));
            } else {
                black2.setBackgroundColor(Color.parseColor("#D8D8D8"));
            }
        });
        increment1.setOnClickListener(view -> {
            int score = Integer.parseInt(score1.getText().toString());
            score += 1;
            score1.setText(String.valueOf(score));
            //send new score via bluetooth
        });
        increment2.setOnClickListener(view -> {
            int score = Integer.parseInt(score2.getText().toString());
            score += 1;
            score2.setText(String.valueOf(score));
            //send new score via bluetooth
        });
        decrement1.setOnClickListener(view -> {
            int score = Integer.parseInt(score1.getText().toString());
            if (score > 0) {
                score -= 1;
                score1.setText(String.valueOf(score));
                //send new score via bluetooth
            }
        });
        decrement2.setOnClickListener(view -> {
            int score = Integer.parseInt(score2.getText().toString());
            if (score > 0) {
                score -= 1;
                score2.setText(String.valueOf(score));
                //send new score via bluetooth
            }
        });
        mute.setOnClickListener(view -> {
            if (mute.getText().toString().equals("Mute")) {
                //mute via bluetooth
            } else {
                //unmute via bluetooth
            }
        });
        timer1.setOnClickListener(view -> {
            try {
                countDown.cancel();
            } catch (NullPointerException ignored) {
            }

            countDown = new CountDownTimer(600000, 1000) {
                private int minutes = 1;
                private int seconds = 0;

                public void onTick(long millis) {
                    if (seconds == 0) {
                        if (minutes != 0) {
                            seconds = 59;
                            minutes -= 1;
                        }
                    } else {
                        seconds -= 1;
                    }

                    @SuppressLint("DefaultLocale") String min_sec = String.format("%02d:%02d",
                            minutes, seconds);

                    time.setText(min_sec);
                }

                public void onFinish() {
                    time.setText(R.string.time);
                }
            }.start();
            //send signal to reset clock via bluetooth
        });
        timer3.setOnClickListener(view -> {
            try {
                countDown.cancel();
            } catch (NullPointerException ignored) {
            }

            countDown = new CountDownTimer(1800000, 1000) {
                private int minutes = 3;
                private int seconds = 0;

                public void onTick(long millis) {
                    if (seconds == 0) {
                        if (minutes != 0) {
                            seconds = 59;
                            minutes -= 1;
                        }
                    } else {
                        seconds -= 1;
                    }

                    @SuppressLint("DefaultLocale") String min_sec = String.format("%02d:%02d",
                            minutes, seconds);

                    time.setText(min_sec);
                }

                public void onFinish() {
                    time.setText(R.string.time);
                }
            }.start();
            //send signal to reset clock via bluetooth
        });
    }

    /* ============================ Thread to Create Bluetooth Connection =================================== */
    public static class CreateConnectThread extends Thread {

        public CreateConnectThread(BluetoothAdapter bluetoothAdapter, String address) {
            /*
            Use a temporary object that is later assigned to mmSocket
            because mmSocket is final.
             */
            BluetoothDevice bluetoothDevice = bluetoothAdapter.getRemoteDevice(address);
            BluetoothSocket tmp = null;
            UUID uuid = bluetoothDevice.getUuids()[0].getUuid();

            try {
                /*
                Get a BluetoothSocket to connect with the given BluetoothDevice.
                Due to Android device varieties,the method below may not work fo different devices.
                You should try using other methods i.e. :
                tmp = device.createRfcommSocketToServiceRecord(MY_UUID);
                 */
                tmp = bluetoothDevice.createInsecureRfcommSocketToServiceRecord(uuid);

            } catch (IOException e) {
                Log.e(TAG, "Socket's create() method failed", e);
            }
            mmSocket = tmp;
        }

        public void run() {
            // Cancel discovery because it otherwise slows down the connection.
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            bluetoothAdapter.cancelDiscovery();
            try {
                // Connect to the remote device through the socket. This call blocks
                // until it succeeds or throws an exception.
                mmSocket.connect();
                Log.e("Status", "Device connected");
                handler.obtainMessage(CONNECTING_STATUS, 1, -1).sendToTarget();
            } catch (IOException connectException) {
                // Unable to connect; close the socket and return.
                try {
                    mmSocket.close();
                    Log.e("Status", "Cannot connect to device");
                    handler.obtainMessage(CONNECTING_STATUS, -1, -1).sendToTarget();
                } catch (IOException closeException) {
                    Log.e(TAG, "Could not close the client socket", closeException);
                }
                return;
            }

            // The connection attempt succeeded. Perform work associated with
            // the connection in a separate thread.
            connectedThread = new ConnectedThread(mmSocket);
            connectedThread.run();
        }

        // Closes the client socket and causes the thread to finish.
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "Could not close the client socket", e);
            }
        }
    }

    /* =============================== Thread for Data Transfer =========================================== */
    public static class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            // Get the input and output streams, using temp objects because
            // member streams are final
            try {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run() {
            byte[] buffer = new byte[1024];  // buffer store for the stream
            int bytes = 0; // bytes returned from read()
            // Keep listening to the InputStream until an exception occurs
            while (true) {
                try {
                    /*
                    Read from the InputStream from Arduino until termination character is reached.
                    Then send the whole String message to GUI Handler.
                     */
                    buffer[bytes] = (byte) mmInStream.read();
                    String readMessage;
                    if (buffer[bytes] == '\n') {
                        readMessage = new String(buffer, 0, bytes);
                        Log.e("Arduino Message", readMessage);
                        handler.obtainMessage(MESSAGE_READ, readMessage).sendToTarget();
                        bytes = 0;
                    } else {
                        bytes++;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                    break;
                }
            }
        }

        /* Call this from the main activity to send data to the remote device */
        public void write(String input) {
            byte[] bytes = input.getBytes(); //converts entered String into bytes
            try {
                mmOutStream.write(bytes);
            } catch (IOException e) {
                Log.e("Send Error", "Unable to send message", e);
            }
        }

        /* Call this from the main activity to shutdown the connection */
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
            }
        }
    }

    /* ============================ Terminate Connection at BackPress ====================== */
    @Override
    public void onBackPressed() {
        // Terminate Bluetooth Connection and close app
        if (createConnectThread != null) {
            createConnectThread.cancel();
        }
        Intent a = new Intent(Intent.ACTION_MAIN);
        a.addCategory(Intent.CATEGORY_HOME);
        a.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(a);
    }
}


