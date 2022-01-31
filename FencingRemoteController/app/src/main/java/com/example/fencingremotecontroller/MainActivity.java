package com.example.fencingremotecontroller;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.ToggleButton;

import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {
    private TextView score1;
    private TextView score2;
    private TextView time;
    private ToggleButton mute;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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
            score -= 1;
            score1.setText(String.valueOf(score));
            //send new score via bluetooth
        });
        decrement2.setOnClickListener(view -> {
            int score = Integer.parseInt(score2.getText().toString());
            score -= 1;
            score2.setText(String.valueOf(score));
            //send new score via bluetooth
        });
        mute.setOnClickListener(view -> {
            if (mute.getText().toString().equals("Mute")){
                //mute via bluetooth
            }
            else {
                //unmute via bluetooth
            }
        });
        timer1.setOnClickListener(view -> {
            new CountDownTimer(600000, 1000) {
                private int minutes = 0;
                private int seconds = 10;
                public void onTick(long millis) {
                    if (seconds == 0){
                        if (minutes !=0){
                            seconds = 59;
                            minutes -= 1;
                        }
                    }
                    else{
                        seconds -= 1;
                    }

                    String min_sec = String.format("%02d:%02d",
                            minutes, seconds);

                    time.setText(String.valueOf(min_sec));
                }

                public void onFinish() {
                    time.setText(R.string.time);
                }
            }.start();
            //send signal to reset clock via bluetooth
        });
        timer3.setOnClickListener(view -> {
            new CountDownTimer(1800000, 1000) {
                private int minutes = 0;
                private int seconds = 10;
                public void onTick(long millis) {
                    if (seconds == 0){
                        if (minutes !=0){
                            seconds = 59;
                            minutes -= 1;
                        }
                    }
                    else{
                        seconds -= 1;
                    }

                    String min_sec = String.format("%02d:%02d",
                            minutes, seconds);

                    time.setText(String.valueOf(min_sec));
                }

                public void onFinish() {
                    time.setText(R.string.time);
                }
            }.start();
            //send signal to reset clock via bluetooth
        });


    }
}