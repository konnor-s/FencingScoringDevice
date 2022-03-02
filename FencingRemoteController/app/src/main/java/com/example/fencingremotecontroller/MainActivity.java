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
    int minutes;
    int seconds;
    private final static int CONNECTING_STATUS = 1; // used in bluetooth handler to identify message status
    private final static int MESSAGE_READ = 2; // used in bluetooth handler to identify message update
    private static final int REQUEST_ENABLE_BT = 1;

    private TextView score1;
    private TextView score2;
    private TextView time;
    private ToggleButton mute;
    private CountDownTimer countDown;
    private TextView mode;
    private ToggleButton onoff;
    private Button playpause;

    // Function to check and request permission
    public void checkPermission(String permission, int requestCode) {
        // Checking if permission is not granted
        if (ContextCompat.checkSelfPermission(MainActivity.this, permission) == PackageManager.PERMISSION_DENIED) {
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{permission}, requestCode);
        } else {
            //Toast.makeText(MainActivity.this, "Permission already granted", Toast.LENGTH_SHORT).show();
        }
    }

    public void openDeviceList() {
        Intent intent = new Intent(MainActivity.this, SelectDeviceActivity.class);
        startActivity(intent);
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
        score1 = findViewById(R.id.score1);
        score2 = findViewById(R.id.score2);
        time = findViewById(R.id.time);
        mute = findViewById(R.id.mute);
        mode = findViewById(R.id.mode);
        onoff = findViewById(R.id.onoff);
        ImageButton increment1 = findViewById(R.id.increment1);
        ImageButton decrement1 = findViewById(R.id.decrement1);
        ImageButton increment2 = findViewById(R.id.increment2);
        ImageButton decrement2 = findViewById(R.id.decrement2);
        Button timer1 = findViewById(R.id.timer1);
        Button timer3 = findViewById(R.id.timer3);
        Button modeButton = findViewById(R.id.modeButton);
        playpause = findViewById(R.id.playpauseButton);
        // If a bluetooth device has been selected from SelectDeviceActivity
        deviceName = getIntent().getStringExtra("deviceName");
        if (deviceName != null) {
            deviceAddress = getIntent().getStringExtra("deviceAddress");
            toolbar.setSubtitle("Connecting to " + deviceName + "...");
            buttonConnect.setEnabled(false);
            BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            createConnectThread = new CreateConnectThread(bluetoothAdapter, deviceAddress);
            createConnectThread.start();
        }

        handler = new Handler(Looper.getMainLooper()) {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case CONNECTING_STATUS:
                        switch (msg.arg1) {
                            case 1:
                                toolbar.setSubtitle("Connected to " + deviceName);
                                buttonConnect.setEnabled(true);
                                break;
                            case -1:
                                toolbar.setSubtitle("Device fails to connect");
                                buttonConnect.setEnabled(true);
                                break;
                        }
                        break;

                    case MESSAGE_READ:
                        String arduinoMsg = msg.obj.toString(); // Read message from Arduino
                        switch (arduinoMsg) {
                            case "inc1": {
                                int newScore = Integer.parseInt(score1.getText().toString()) + 1;
                                score1.setText(String.valueOf(newScore));
                                break;
                            }
                            case "inc2": {
                                int newScore = Integer.parseInt(score2.getText().toString()) + 1;
                                score2.setText(String.valueOf(newScore));
                                break;
                            }
                            case "dec1": {
                                int newScore = Integer.parseInt(score1.getText().toString()) - 1;
                                score1.setText(String.valueOf(newScore));
                                break;
                            }
                            case "dec2": {
                                int newScore = Integer.parseInt(score2.getText().toString()) - 1;
                                score2.setText(String.valueOf(newScore));
                                break;
                            }
                            case "mode": {
                                if (mode.getText().equals("Foil")) {
                                    mode.setText("Epee");
                                } else if (mode.getText().equals("Epee")) {
                                    mode.setText("Sabre");
                                } else if (mode.getText().equals("Sabre")) {
                                    mode.setText("Foil");
                                }
                                break;
                            }
                            case "timer1": {
                                try {
                                    countDown.cancel();
                                } catch (NullPointerException ignored) {
                                }
                                minutes = 1;
                                seconds = 0;
                                countDown = new CountDownTimer(600000, 1000) {
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
                                        playpause.setText("---");
                                    }
                                }.start();
                                break;
                            }
                            case "timer3": {
                                try {
                                    countDown.cancel();
                                } catch (NullPointerException ignored) {
                                }
                                minutes = 3;
                                seconds = 0;
                                countDown = new CountDownTimer(1800000, 1000) {

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
                                        playpause.setText("---");
                                    }
                                }.start();
                                break;
                            }

                            case "play": { //receive the message to start timer from current timer value
                                String timetxt = time.getText().toString();
                                String[] parts = timetxt.split(":");
                                minutes = Integer.parseInt(parts[0]);
                                seconds = Integer.parseInt(parts[1]);
                                int timeMillis = ((minutes * 60) + seconds) * 1000;

                                countDown = new CountDownTimer(timeMillis, 1000) {
                                    @Override
                                    public void onTick(long l) {
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

                                    @Override
                                    public void onFinish() {
                                        time.setText(R.string.time);
                                        playpause.setText("---");
                                    }
                                };
                                countDown.start();
                                playpause.setText("pause");
                            }
                            default: {
                                if (arduinoMsg.contains("pause")) { //check for the pause message
                                    //get the time from the arduino and update the app's time
                                    String arduinoTime = arduinoMsg.substring(5);
                                    int arduinoTimeInt = Integer.parseInt(arduinoTime);
                                    minutes = arduinoTimeInt % 60;
                                    seconds = arduinoTimeInt - minutes * 60;
                                    @SuppressLint("DefaultLocale") String min_sec = String.format("%02d:%02d",
                                            minutes, seconds);

                                    time.setText(min_sec);
                                    if (minutes != 0 && seconds != 0) {
                                        playpause.setText("play");
                                    }
                                } else {
                                    Log.e("msg", "Invalid message");
                                }
                                break;
                            }
                        }
                        break;

                }
            }
        };
        modeButton.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
                if (mode.getText().equals("Foil")) {
                    connectedThread.write("foil\n");
                    mode.setText("Epee");
                } else if (mode.getText().equals("Epee")) {
                    connectedThread.write("epee\n");
                    mode.setText("Sabre");
                } else if (mode.getText().equals("Sabre")) {
                    connectedThread.write("sabre\n");
                    mode.setText("Foil");
                }
            } else {
                openDeviceList();
            }
        });
        // Select Bluetooth Device
        buttonConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                openDeviceList();
            }
        });


        onoff.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
                String btnState = onoff.getText().toString().toLowerCase();
                connectedThread.write("Hello\n");
            } else {
                openDeviceList();
            }
        });

        increment1.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
                int score = Integer.parseInt(score1.getText().toString());
                score += 1;
                score1.setText(String.valueOf(score));
                connectedThread.write("inc1\n");
            } else {
                openDeviceList();
            }
        });
        increment2.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
                int score = Integer.parseInt(score2.getText().toString());
                score += 1;
                score2.setText(String.valueOf(score));
                connectedThread.write("inc2\n");
            } else {
                openDeviceList();
            }
        });
        decrement1.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
                int score = Integer.parseInt(score1.getText().toString());
                if (score > 0) {
                    score -= 1;
                    score1.setText(String.valueOf(score));
                    connectedThread.write("dec1\n");
                }
            } else {
                openDeviceList();
            }
        });
        decrement2.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
                int score = Integer.parseInt(score2.getText().toString());
                if (score > 0) {
                    score -= 1;
                    score2.setText(String.valueOf(score));
                    connectedThread.write("dec2\n");
                }
            } else {
                openDeviceList();
            }
        });
        mute.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
                if (mute.getText().toString().equals("Mute")) {
                    connectedThread.write("mute\n");
                } else {
                    connectedThread.write("mute\n");
                }
            } else {
                openDeviceList();
            }
        });
        playpause.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
                try {
                    countDown.cancel();
                } catch (NullPointerException ignored) {
                }
                if (!countDown.toString().equals("00:00") && playpause.getText().equals("pause")) // timer is going
                {
                    connectedThread.write("pause\n");
                    playpause.setText("play");
                } else if (!countDown.toString().equals("00:00") && playpause.getText().equals("play")) //timer is not going and is not at zero
                {
                    String timetxt = time.getText().toString();
                    String[] parts = timetxt.split(":");
                    minutes = Integer.parseInt(parts[0]);
                    seconds = Integer.parseInt(parts[1]);
                    int timeMillis = ((minutes * 60) + seconds) * 1000;

                    countDown = new CountDownTimer(timeMillis, 1000) {
                        @Override
                        public void onTick(long l) {
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

                        @Override
                        public void onFinish() {
                            time.setText(R.string.time);
                            playpause.setText("---");
                        }
                    };
                    countDown.start();
                    connectedThread.write("play\n");
                    playpause.setText("pause");
                }
            }
        });
        timer1.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
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
                        playpause.setText("pause");

                    }

                    public void onFinish() {
                        time.setText(R.string.time);
                        playpause.setText("---");
                    }
                }.start();
                connectedThread.write("timer1\n");
            } else {
                openDeviceList();
            }
        });
        timer3.setOnClickListener(view -> {
            if (connectedThread != null && connectedThread.isConnected()) {
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
                        playpause.setText("pause");
                    }

                    public void onFinish() {
                        time.setText(R.string.time);
                        playpause.setText("---");
                    }
                }.start();
                connectedThread.write("timer3\n");
            } else {
                openDeviceList();
            }
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
            Log.e("Status", "Connected Thread started");
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

        public boolean isConnected() {
            return mmSocket.isConnected();
        }

        /* Call this from the main activity to shutdown the connection */
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
            }
        }
    }

}


